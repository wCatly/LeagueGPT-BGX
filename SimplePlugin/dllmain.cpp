
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

#include "GPT.hpp"
				
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

	GPT::load(main_tab);

    std::string prompt = "Your prompt text";



    return true;
}

PLUGIN_API void on_sdk_unload( )
{
	menu->delete_tab(main_tab);
}   