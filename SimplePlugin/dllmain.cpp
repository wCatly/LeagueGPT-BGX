
#include "../plugin_sdk/plugin_sdk.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <string>
#include <stdexcept>
#include <array>
#include <cstdio>
#include <iostream>
#include <memory>

using json = nlohmann::json;


std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

enum class ChatType
{
    All,
    Team
};


void make_request_new(std::string prompt, ChatType type)
{
    // Replace these placeholders with actual values or variables from your code.
    std::string prompt_text = "replace";
    std::string ai_model = "replace";
    std::string api_key = "replace";
    std::string custom_ignore_key = "replace";
	

    console->print("\n Making request");
    console->print("\nPromt came: %s", prompt.c_str());

    std::string system_promt = prompt_text;

    json data = {
        {"model", ai_model},
        {"messages", json::array({
            {
                {"role", "system"},
                {"content", system_promt},
            },
            {
                {"role", "user"},
                {"content", prompt},
            }
        })},
        {"temperature", 0.7}
    };

    std::string json_data = data.dump();

    console->print("JSON: %s", json_data.c_str());
    std::string api = std::string(api_key);
    std::string cmd = "curl -s -X POST -H \"Authorization: Bearer " + api + "\" -H \"Content-Type: application/json\" -d '" + json_data + "' https://api.openai.com/v1/chat/completions";

    try
    {
        std::string resp_str = exec(cmd.c_str());
        console->print("\nResponse: %s", resp_str.c_str());

        json response = json::parse(resp_str);

        if (response.contains("error")) {
            std::string errorMessage = response["error"]["message"];
            std::string errorType = response["error"]["type"];
            std::string errorPrint = "<font color='#FF69B4'>[LeagueGPT]</font><font color='#FF0000'> [ERROR]:</font></font><font color='#FFFFFF'> " + errorMessage + " (" + errorType + ")</font>";
            (errorPrint.c_str());

            myhero->print_chat(1, errorPrint.c_str());
            return;
        }

        std::string text = response["choices"][0]["message"]["content"];

        std::string lower_text = text;

        size_t pos = text.find(custom_ignore_key);
        if (pos != std::string::npos) {

            console->print("\nI cant:");
            return; // Return without sending the message
        }

        if (type == ChatType::All) {
            myhero->send_chat(("/all " + text).c_str());
            console->print("\nI need to send message here:");
        }
        else if (type == ChatType::Team) {
            myhero->send_chat(text.c_str());
            console->print("\nI need to send message here:");
        }
    }
    catch (const std::runtime_error& e)
    {
        console->print(("Error: " + std::string(e.what())).c_str());
    }
}

				
PLUGIN_NAME("LeagueGPT");
PLUGIN_TYPE(plugin_type::utility);
TreeTab* main_tab;

PLUGIN_API bool on_sdk_load( plugin_sdk_core* plugin_sdk_good )
{
    DECLARE_GLOBALS( plugin_sdk_good );

	loaded_texture* image;
	image = draw_manager->load_texture_from_file(L"leaguegpt.png");

	//=========================================================================//
    // Menu Creation                                                           //
    //=========================================================================//
	main_tab = menu->create_tab("league_gpt", "LeagueGPT");
	main_tab->set_assigned_texture(image->texture);
	//=========================================================================//
	// Load Modules                                                            //
	//=========================================================================//

    std::string prompt = "Your prompt text";
    ChatType type = ChatType::All;

    make_request_new(prompt, type);



    return true;
}

PLUGIN_API void on_sdk_unload( )
{
	menu->delete_tab(main_tab);
}   