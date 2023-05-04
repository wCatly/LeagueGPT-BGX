#include <regex>

#include "Helpers.hpp"

namespace GPT
{
#include <utility>

#include "../plugin_sdk/plugin_sdk.hpp"


	namespace settings
	{
		TreeEntry* enabled;
		TreeEntry* enabled2;
		TreeEntry* hotkey;
		TreeEntry* value;
		TreeEntry* delay;
		TreeEntry* mode;
		TreeEntry* hotkey2;
		TreeEntry* max_value;
	}

	int last_chat_index = 1;

	enum class ChatType {
		All,
		Team,
		Unknown
	};

	enum class ParseType {
		Direct,
		Optimized
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


	std::tuple<ChatType, std::string, bool> parseChatMessage(std::string input, std::string myhero_name, ParseType type) {


		std::regex re(R"(\[(.*?)\] (.*?) \((.*?)\): <\/font><font color='#FFFFFF'>(.*?)<\/font>)");
		std::smatch match;

		if (std::regex_search(input, match, re)) {
			std::string message_type = match.str(1);
			std::string player_name = match.str(2);
			std::string message_owner = match.str(3);
			std::string message = match.str(4);
			std::tuple<std::string, std::string, std::string, std::string> message_tuple(message_type, player_name, message_owner, message);

			ChatType messageType;
			if (std::get<0>(message_tuple) == "All") {
				messageType = ChatType::All;
			}
			else if (std::get<0>(message_tuple) == "Team") {
				messageType = ChatType::Team;
			}
			else {
				messageType = ChatType::Unknown;
			}

			std::string formattedMessage;

			if (type == ParseType::Direct) {
				formattedMessage = std::get<0>(message_tuple) + " " + std::get<1>(message_tuple) + " (" + std::get<2>(message_tuple) + "): " + std::get<3>(message_tuple);
			}

			if (type == ParseType::Optimized) {
				formattedMessage = "Message from " + std::get<2>(message_tuple) + ": " + message;
			}

			bool isFromMe = (input.find(myhero_name) != std::string::npos);

			return { messageType, formattedMessage, isFromMe };
		}
		else {
			std::cout << "No match found" << std::endl;
		}

		return { ChatType::Unknown, "", false };
	}

	inline void new_chat_message(std::string msg)
	{

		ChatType chatType;
		std::string formattedMessage;
		bool isFromMe;


		std::tie(chatType, formattedMessage, isFromMe) = parseChatMessage(msg, myhero->get_name(), ParseType::Optimized);


		console->print(fmt::format("\nMessage: {}", formattedMessage).c_str());
		console->print(fmt::format("\nIs from me: {}", std::string(isFromMe ? "Yes" : "No")).c_str());


		// Example usage for ally_all message
		if (chatType != ChatType::Unknown && !formattedMessage.empty()) {

			make_request_new(formattedMessage, chatType);
		}

	}

	void on_update()
	{
		const int chat_index = gui->get_chat_current_message_index();
		/*console->print("chat_index: %i", chat_index);*/
		if (last_chat_index != chat_index)
		{
			for (int i = last_chat_index; i < chat_index; i++)
			{
				new_chat_message(gui->get_chat_message_by_index(i));
			}
			last_chat_index = chat_index;
		}
	}



	void load(TreeTab* main)
	{

		last_chat_index = gui->get_chat_current_message_index();

		settings::enabled = main->add_checkbox("enabled", "Enabled", true);
		main->set_assigned_active(settings::enabled);
		settings::hotkey = main->add_hotkey("hotkey", "Hotkey", TreeHotkeyMode::Hold, 0x07, false);
		main->add_separator("value_separator", "Value Settings");
		settings::max_value = main->add_slider("max", "Max Slider Value", 300, 0, 65535);
		settings::max_value->set_tooltip("Only updated when plugin reload");
		settings::value = main->add_slider("value", "Value", 15, 0, settings::max_value->get_int());
		settings::value->set_tooltip("Max Value only updated when plugin reload");
		settings::delay = main->add_slider("delay", "Delay Chat", 3, 1, 10);
		main->add_separator("secret_separator", "Secret Settings");
		settings::enabled2 = main->add_checkbox("enabled2", "Secret Mode", false);
		settings::hotkey2 = main->add_hotkey("hotkey2", "Secret Troll Message", TreeHotkeyMode::Hold, 0x07, false);
		settings::mode = main->add_combobox("mode", "Mode", { {"NA", nullptr}, {"BR", nullptr}, {"TR", nullptr}, {"Riot", nullptr} }, 0);
		event_handler<events::on_update>::add_callback(on_update);
	}

	void unload()
	{
		event_handler<events::on_update>::remove_handler(on_update);
	}


}
