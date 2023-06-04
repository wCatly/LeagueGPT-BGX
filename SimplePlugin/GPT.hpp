#include <future>
#include <regex>
#include <thread>

#include "Helpers.hpp"
#include "permashow.hpp"

namespace GPT
{
#include <utility>

#include "../plugin_sdk/plugin_sdk.hpp"

	std::vector<std::thread*> tasks;
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

		TreeEntry* dont_if_dead;
		TreeEntry* open_ai_key;
		TreeEntry* modal;
		TreeEntry* temperature;

		TreeEntry* promt;
		TreeEntry* check_ai;

		std::string selected_promt;
		std::string custom_ignore_key;

		TreeEntry* game_events;

		TreeEntry* onkill;
		TreeEntry* on_assist;
		TreeEntry* on_surrender;
		TreeEntry* on_neturalcamp;

		TreeEntry* quiet_hotkey;
		TreeEntry* quiet_hotkey_team;
	}

	int old_kills = 0;
	int old_assists = 0;


	int last_chat_index = 0;


	int ALLY_KILLS = 0;
	int ENEMY_KILLS = 0;

	void make_request_new(std::string prompt, ChatType type)
	{
		try
		{
			std::string command = "LeagueGPTHelper.exe --model=\"" + get_modal(settings::modal->get_int()) + "\" --key=\"" + settings::open_ai_key->get_string() + "\" --system=\"" + settings::selected_promt + "\" --user=\"" + prompt + "\"";
			std::string response_str = exec(command.c_str());


			if (response_str.find("ERROR:") != std::string::npos) {
				std::string errorPrint = "<font color='#FF69B4'>[LeagueGPT]</font><font color='#FF0000'> [ERROR]:</font></font><font color='#FFFFFF'> " + response_str + "</font>";

				myhero->print_chat(1, errorPrint.c_str());
				return;
			}

			try
			{
				json response = json::parse(response_str);
				std::string text = response["choices"][0]["message"]["content"];

				sendMessage(text, type, settings::custom_ignore_key);
			}
			catch (const std::exception& e) {
				myhero->print_chat(1, "something went wrong");
				return;
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

			std::string message_name = removeSpecialCharacters(std::get<2>(message_tuple));
			std::string normalized_hero_name = removeSpecialCharacters(myhero_name);

			bool isFromMe = message_name == normalized_hero_name;

			return { messageType, formattedMessage, isFromMe };
		}

		return { ChatType::Unknown, "", false };
	}

	inline void new_chat_message(std::string msg)
	{
		// Replace all double quotes with single quotes in the input string
		std::replace(msg.begin(), msg.end(), '\"', '\'');

		ChatType chatType;
		std::string formattedMessage;
		bool isFromMe;

		// Pass 'msg' directly to parseChatMessage
		std::tie(chatType, formattedMessage, isFromMe) = parseChatMessage(msg, myhero->get_base_skin_name(), ParseType::Optimized);

		//console->print(("\nMessage: " + formattedMessage).c_str());
		//console->print(("\nIs from me: " + std::string(isFromMe ? "Yes" : "No")).c_str());

		// Example usage for ally_all message
		if (chatType != ChatType::Unknown && !formattedMessage.empty() && !isFromMe) {

			tasks.push_back(new std::thread(make_request_new, formattedMessage, chatType));

		}

	}

	std::string last_chat_message;

	std::tuple<int, int> GetKills()
	{
		int enemyKills = 0;
		int allyKills = 0;

		for (auto&& enemy : entitylist->get_enemy_heroes())
		{
			int kills = enemy->get_hero_stat(int_hero_stat::CHAMPIONS_KILLED);
			enemyKills += kills;
		}

		for (auto&& ally : entitylist->get_ally_heroes())
		{
			int kills = ally->get_hero_stat(int_hero_stat::CHAMPIONS_KILLED);
			allyKills += kills;
		}

		return std::make_tuple(enemyKills, allyKills);
	}

	void on_object_dead(const game_object_script sender)
	{
		if(sender->is_ai_hero())
		{
			if (sender->is_enemy())
			{
				const int kill = myhero->get_hero_stat(int_hero_stat::CHAMPIONS_KILLED);
				const int assists = myhero->get_hero_stat(int_hero_stat::ASSISTS);
				if (kill != old_kills || assists != old_assists)
				{
					if (settings::onkill->get_bool() && kill != old_kills)
					{
						std::string KillMessage = "you got an kill on the enemy " + sender->get_base_skin_name() + " and now disrespect him in all chat (use his name and harass the enemy on his champion)";

						tasks.push_back(new std::thread(make_request_new, KillMessage, ChatType::All));
					}
					if (settings::on_assist->get_bool() && assists != old_assists)
					{
						std::string AssistlMessage = "you got an assist on the enemy " + sender->get_base_skin_name() + " and now disrespect him in all chat (use his name and harass the enemy on his champion)";

						tasks.push_back(new std::thread(make_request_new, AssistlMessage, ChatType::All));
					}



					old_kills = kill;
					old_assists = assists;
				}
				ALLY_KILLS = ALLY_KILLS + 1;
			}
			if(sender->is_ally())
			{
				ENEMY_KILLS = ENEMY_KILLS + 1;
			}
		}
		
	}

	void on_vote(const on_vote_args& args)
	{

		if(args.vote_type == on_vote_type::surrender)
		{
			if (args.sender && settings::on_surrender->get_bool())
			{
				if(args.sender->is_me()) return;

				std::string surrenderPrompt = "Surrender: " + args.sender->get_base_skin_name() + " (" + args.sender->get_name_cstr() + ") voted " + (args.success ? "YES" : "NO") + " what you wanna say? (use his name and say something to ally on his champion)";
				tasks.push_back(new std::thread(make_request_new, surrenderPrompt, ChatType::All));
			}
		}

		
	}


	void on_update()
	{

		std::tuple<int, int> kills = GetKills();
		int enemyKills = std::get<0>(kills);  // Total kills for enemies
		int allyKills = std::get<1>(kills);  // Total kills for allies

		console->print("ENEMY KILLS: %i", enemyKills);
		console->print("ally KILLS:  %i", allyKills);
		auto current_chat_message = gui->get_last_chat_message();

		if(current_chat_message == nullptr) return;


			if (current_chat_message != last_chat_message && settings::enabled->get_bool())
			{
				new_chat_message(current_chat_message);
				last_chat_message = current_chat_message;
			}

	}



	void load(TreeTab* main)
	{
		struct CommunityPrompt {
			std::string title;
			std::string icon;
			std::string description;
			std::string ignore_key;
		};

		std::vector<CommunityPrompt> community_prompts;
		std::string file_path = "CommunityPrompts.json";
		// Convert JSON content to CommunityPrompt struct objects
		for (const auto& prompt_json : read_json_from_file(file_path)) {
			CommunityPrompt prompt;
			prompt.title = prompt_json["title"].get<std::string>();
			prompt.icon = prompt_json["icon"].get<std::string>();
			prompt.description = prompt_json["description"].get<std::string>();

			prompt.ignore_key = prompt_json["ignore_key"].get<std::string>();
			community_prompts.push_back(prompt);
		}

		size_t community_prompts_count = community_prompts.size();
		old_kills = myhero->get_hero_stat(int_hero_stat::CHAMPIONS_KILLED);
		old_assists = myhero->get_hero_stat(int_hero_stat::ASSISTS);


		last_chat_index = gui->get_chat_current_message_index();

		main->add_separator("x", "League GPT - Unleash the Power of AI in League with LeagueGPT");

		settings::enabled = main->add_checkbox("enabled", "Enable AI Chat", true);
		main->set_assigned_active(settings::enabled);

		settings::dont_if_dead = main->add_checkbox("deadcheck", "Don't send message if myhero dead", true);

		main->add_separator("value_separator", "AI customization settings");

		settings::open_ai_key = main->add_text_input("OPENAI_KEY_HERE", "OpenAI API Key", "KEY HERE");
		settings::open_ai_key->set_tooltip("You should use BGX Config Tool for this ");
		settings::modal = main->add_combobox("mode", "AI Modal", { {"gpt-4", nullptr}, {"gpt-4-0314", nullptr}, {"gpt-3.5-turbo", nullptr}, {"gpt-3.5-turbo-0301", nullptr} }, 0);
		settings::temperature = main->add_slider("temperature", "Temperature", 0.0f, 1.0f, 0.5f, 2);

		main->add_separator("value_separator", "AI Promt settings");

		std::vector<std::pair<std::string, void*>> combo_elements;

		for (size_t i = 0; i < community_prompts_count; i++)
		{
			std::string item_label = community_prompts[i].title;
			std::string item_tooltip = community_prompts[i].description;

			combo_elements.push_back(std::make_pair(item_label, nullptr)); // You may replace nullptr with the appropriate value if needed
		}

		settings::promt = main->add_combobox("key", "Choose a community prompt", combo_elements, 0);

		PropertyChangeCallback update_fun = [](TreeEntry* entry)
		{
			auto num = entry->get_int();

			struct CommunityPrompt {
				std::string title;
				std::string icon;
				std::string description;
				std::string ignore_key;
			};

			std::vector<CommunityPrompt> community_prompts;
			std::string file_path = "CommunityPrompts.json";
			for (const auto& prompt_json : read_json_from_file(file_path)) {
				CommunityPrompt prompt;
				prompt.title = prompt_json["title"].get<std::string>();
				prompt.icon = prompt_json["icon"].get<std::string>();
				prompt.description = prompt_json["description"].get<std::string>();

				prompt.ignore_key = prompt_json["ignore_key"].get<std::string>();
				community_prompts.push_back(prompt);
			}

			std::map<std::string, std::string> variable_map = {
           {"{champName}", myhero->get_base_skin_name()},
            // add more variables here
			};

			std::string prompt_text_with_vars = community_prompts[num].description;
			settings::selected_promt = replace_variables(prompt_text_with_vars, variable_map);

			entry->set_tooltip(settings::selected_promt);

			settings::custom_ignore_key = community_prompts[num].ignore_key;
		};

		settings::promt->add_property_change_callback(update_fun);


		std::map<std::string, std::string> variable_map = {
	   {"{champName}", myhero->get_base_skin_name()},
	   // add more variables here
		};

		std::string item_tooltip = community_prompts[settings::promt->get_int()].description;
		settings::promt->set_tooltip(settings::selected_promt = replace_variables(item_tooltip, variable_map)); // This may need adjustment depending on how your tooltips are handled

		settings::selected_promt = replace_variables(item_tooltip, variable_map);
		settings::custom_ignore_key = community_prompts[settings::promt->get_int()].ignore_key;

		main->add_separator("evetns", "Misc Stuff");
		settings::quiet_hotkey = main->add_button("quiet_hotkey", "Quiet Hotkey");
		settings::quiet_hotkey_team = main->add_button("quiet_hotkey_team", "Quiet Hotkey (Team)");
		const auto events_tab = main->add_tab("events_tab", "Game Events");
		{

			settings::onkill = events_tab->add_checkbox("on_kill_event", "On Kill", false);
			settings::on_assist = events_tab->add_checkbox("on_assistt", "On Assist", false);
			settings::on_surrender = events_tab->add_checkbox("on_sur", "On Surrender", false);
		}

		settings::check_ai = main->add_button("ai_button", "Make test request");

		PropertyChangeCallback Button = [](TreeEntry* entry)
		{
			std::string prompt = "Say this is a test!";
			std::string command = "LeagueGPTHelper.exe --model=\"" + get_modal(settings::modal->get_int()) + "\" --key=\"" + settings::open_ai_key->get_string() + "\" --system=\"" + settings::selected_promt + "\" --user=\"" + prompt + "\"";
			std::string response_str = exec(command.c_str());

			if (response_str.find("ERROR:") != std::string::npos) {
				std::string errorPrint = "<font color='#FF69B4'>[LeagueGPT]</font><font color='#FF0000'> [ERROR]:</font></font><font color='#FFFFFF'> " + response_str + "</font>";

				myhero->print_chat(1, errorPrint.c_str());
				return;
			}

			try {
				json response = json::parse(response_str);
				std::string text = response["choices"][0]["message"]["content"];
				std::string SucsessPrint = "<font color='#FF69B4'>[LeagueGPT]</font><font color='#00FF00'> [TEST MESSAGE SUCSESS!]:</font></font><font color='#FFFFFF'> " + text + "</font>";
				myhero->print_chat(1, SucsessPrint.c_str());
				return;
			}
			catch (const std::exception& e) {
				/*std::string errorPrint = "<font color='#FF69B4'>[LeagueGPT]</font><font color='#FF0000'> [ERROR]:</font></font><font color='#FFFFFF'> " + response_str + "</font>";*/
				myhero->print_chat(1, "something wrong");
				return;
			}
		};
		settings::check_ai->add_property_change_callback(Button);

		PropertyChangeCallback QuietButton = [](TreeEntry* entry)
		{
			std::string SleepMessage = "the game is so quiet and you want to engage in a conversation while mentioning that you are " + myhero->get_base_skin_name();
			tasks.push_back(new std::thread(make_request_new, SleepMessage, ChatType::All));
		};
		settings::quiet_hotkey->add_property_change_callback(QuietButton);

		PropertyChangeCallback QuietButtonTeam = [](TreeEntry* entry)
		{
			std::string SleepMessage = "the game is so quiet and you want to engage in a conversation with your teammates while talking about how to win the game with " + myhero->get_base_skin_name();
			tasks.push_back(new std::thread(make_request_new, SleepMessage, ChatType::Team));
		};
		settings::quiet_hotkey_team->add_property_change_callback(QuietButtonTeam);

		GetPermashow().Init("LeagueGPT");
		GetPermashow().AddElement("AI Chat", settings::enabled);

		event_handler<events::on_update>::add_callback(on_update);
		event_handler<events::on_object_dead>::add_callback(on_object_dead);
		event_handler<events::on_vote>::add_callback(on_vote);
	}

	void unload()
	{
		event_handler<events::on_update>::remove_handler(on_update);
		event_handler<events::on_object_dead>::remove_handler(on_object_dead);
		event_handler<events::on_vote>::remove_handler(on_vote);
	}


}
