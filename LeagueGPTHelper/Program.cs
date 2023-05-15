using System;
using System.Collections.Generic;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

class Program
{
    static readonly HttpClient client = new HttpClient();

    static async Task Main(string[] args)
    {
        var parameters = ParseArguments(args);

        if (parameters == null)
        {
            Console.WriteLine("Invalid arguments. Please provide arguments in the format --arg=value");
            return;
        }

        bool debug = parameters.ContainsKey("debug") && parameters["debug"].ToLower() == "true";

        if (debug) Console.WriteLine("Creating request...");
        var request = new
        {
            model = parameters["model"],
            messages = new[] 
            { 
                new { role = "system", content = parameters["system"] },
                new { role = "user", content = parameters["user"] },
            },
            temperature = 0.7
        };

        var json = JsonConvert.SerializeObject(request);
        var data = new StringContent(json, Encoding.UTF8, "application/json");

        if (debug) Console.WriteLine("Adding authorization header...");
        client.DefaultRequestHeaders.Add("Authorization", $"Bearer {parameters["key"]}");

        if (debug) Console.WriteLine("Sending request...");
        var response = await client.PostAsync("https://api.openai.com/v1/chat/completions", data);

        if (debug) Console.WriteLine("Reading response...");
        var result = await response.Content.ReadAsStringAsync();
        
        Console.WriteLine(result);
    }

    static Dictionary<string, string> ParseArguments(string[] args)
    {
        var parameters = new Dictionary<string, string>();

        foreach (var arg in args)
        {
            if (!arg.StartsWith("--"))
            {
                return null!;
            }

            var splitArg = arg.Substring(2).Split('=');

            if (splitArg.Length != 2)
            {
                return null!;
            }

            parameters[splitArg[0]] = splitArg[1];
        }

        if (!parameters.ContainsKey("model") || !parameters.ContainsKey("key") || !parameters.ContainsKey("system") || !parameters.ContainsKey("user"))
        {
            return null!;
        }

        return parameters;
    }
}
