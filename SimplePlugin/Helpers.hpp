#include "../plugin_sdk/plugin_sdk.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>
#include <string>
#include <array>
#include <cstdio>
#include <fstream>
#include <memory>
#include <sstream>



using json = nlohmann::json;

enum class ChatType {
    All,
    Team,
    Unknown
};

enum class ParseType {
    Direct,
    Optimized
};


inline int CalculateSmartDelay(const std::string& message)
{
    // Average typing speed is roughly 40 words per minute,
    // or about 200 characters per minute (assuming 5 characters per word),
    // or about 3.33 characters per second.
    const double charactersPerSecond = 3.33;

    // Calculate the delay in seconds.
    int delay = static_cast<int>(std::round(message.length() / charactersPerSecond));

    // Ensure the delay is at least the minimum value (1 second in this case).
    // You can also set a maximum value if desired.
    delay = std::max(delay, 1);

    return delay;
}

bool hasQuotesAtStartAndEnd(const std::string& str) {
    if (str.length() < 2) {
        return false;
    }
    return str.front() == '\"' && str.back() == '\"';
}

std::string removeQuotesAtStartAndEnd(std::string str) {
    if (str.length() < 2) {
        return str;
    }
    if (str.front() == '\"') {
        str.erase(0, 1); // Remove the first character
    }
    if (str.back() == '\"') {
        str.erase(str.length() - 1); // Remove the last character
    }
    return str;
}

inline void sendMessage(std::string prompt, ChatType type, std::string ignoreKey ) {
    if (hasQuotesAtStartAndEnd(prompt)) {
        prompt = removeQuotesAtStartAndEnd(prompt);
    }

    // Check if the ignore key is found in the prompt. If found, return without sending the message.
    size_t pos = prompt.find(ignoreKey);
    if (pos != std::string::npos) return;


    std::string message;

    switch (type) {
    case ChatType::All:
        message = "/all " + prompt;
        break;
    case ChatType::Team:
        message = prompt;
        break;
    default:
        return; // If type is not handled, don't send a message
    }

    myhero->send_chat(message.c_str());
}


std::string removeSpecialCharacters(const std::string& str) {
    std::string result;
    std::remove_copy_if(str.begin(), str.end(), std::back_inserter(result),
        [](char c) { return !std::isalnum(c); });

    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });

    result.erase(std::remove(result.begin(), result.end(), ' '), result.end());

    return result;
}


std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
    if (!pipe)
    {
        console->print("popen() failed.");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    if (result.empty())
    {
        return "0"; // Return "0" as a string
    }
    return result;
}

inline json read_json_from_file(const std::string& file_path) {
    std::ifstream file(file_path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return json::parse(buffer.str());
}

inline std::string replace_variables(const std::string& prompt_text, std::map<std::string, std::string> variable_map) {
    std::string result = prompt_text;
    for (const auto& [placeholder, value] : variable_map) {
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), value);
            pos += value.length();
        }
    }
    return result;
}

inline std::string get_modal(int value) {
    std::string result;

    switch (value) {
    case 0:
        result = "gpt-4";
        break;
    case 1:
        result = "gpt-4-0314";
        break;
    case 2:
        result = "gpt-3.5-turbo";
        break;
    case 3:
        result = "gpt-3.5-turbo-0301";
        break;
    default:
        result = ""; // or any other default value
        break;
    }

    return result;
}





