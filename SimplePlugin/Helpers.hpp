#include "../plugin_sdk/plugin_sdk.hpp"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <string>
#include <stdexcept>
#include <array>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

using json = nlohmann::json;


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





