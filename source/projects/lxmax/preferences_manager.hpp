/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <string>
#include <filesystem>

#include "c74_min.h"
#include "version_info.hpp"

class preferences_manager
{
	c74::min::dict _preferences;

	const char* k_preferences_filename = "lxmaxpreferences.json";
	
	const c74::min::symbol k_sym_lxmax_version_major{"lxmax_version_major"};
	const c74::min::symbol k_sym_lxmax_version_minor{"lxmax_version_minor"};
	const c74::min::symbol k_sym_lxmax_version_bugfix{"lxmax_version_bugfix"};

	const c74::min::symbol k_sym_global_preferences{"global_preferences"};
	const c74::min::symbol k_sym_universes{"universes"};

	void create_default()
	{
		_preferences.clear();

		_preferences[k_sym_lxmax_version_major] = lxmax::GIT_VERSION_MAJOR;
		_preferences[k_sym_lxmax_version_minor] = lxmax::GIT_VERSION_MINOR;
		_preferences[k_sym_lxmax_version_bugfix] = lxmax::GIT_VERSION_BUFGIX;
#
		c74::min::dict global_preferences;
		global_preferences["artnet_network_adapter"] = "0.0.0.0";
		global_preferences["sacn_network_adapter"] = "0.0.0.0";
		global_preferences["artnet_unicast_addresses"] = "127.0.0.1";
		global_preferences["sacn_unicast_addresses"] = "127.0.0.1";
		global_preferences["send_artnet_sync_packets"] = 1;
		global_preferences["send_sacn_sync_packets"] = 1;
		global_preferences["send_sacn_sync_packets"] = 1;
		global_preferences["output_empty_universes"] = 0;
		global_preferences["fixed_framerate_enabled"] = 0;
		global_preferences["fixed_framerate"] = 44;
		global_preferences["fixed_framerate_nondmx_enabled"] = 0;

		_preferences[k_sym_global_preferences] = global_preferences;
		
		c74::min::dict universes;

		_preferences[k_sym_universes] = universes;
	}

	void read_from_disk()
	{
		short pref_path;
		c74::max::preferences_path(nullptr, true, &pref_path);

		char file_path[MAX_PATH_CHARS];
		c74::max::path_toabsolutesystempath(pref_path, k_preferences_filename, file_path);

		if (!std::filesystem::exists(file_path))
		{
			create_default();
			return;
		}

		c74::max::t_dictionary* d;
		c74::max::dictionary_read((char*)k_preferences_filename, pref_path, &d);

		if (d == nullptr)
		{
			std::cerr << "Unable to read LXMax preferences file. Resetting to default preferences..." << std::endl;
			create_default();
			return;
		}
		
		c74::min::dict to_import{d};

		try
		{
			const c74::min::atom version_major = to_import.at(k_sym_lxmax_version_major);
			const c74::min::atom version_minor = to_import.at(k_sym_lxmax_version_minor);
			const c74::min::atom version_bugfix = to_import.at(k_sym_lxmax_version_bugfix);

			if (int(version_major) != lxmax::GIT_VERSION_MAJOR
				|| int(version_minor) != lxmax::GIT_VERSION_MAJOR
				|| int(version_bugfix) != lxmax::GIT_VERSION_BUFGIX)
			{
				std::cerr << "Preferences data is for version "
					<< version_major << "." << version_minor << "." << version_bugfix
					<< " of LXMax. Current version is "
					<< lxmax::GIT_VERSION_MAJOR << "." << lxmax::GIT_VERSION_MAJOR << "." << lxmax::
					GIT_VERSION_BUFGIX
					<< "Resetting to default preferences..." << std::endl;
				
				create_default();
				return;
			}
		}
		catch (std::runtime_error& ex)
		{
			std::cerr << "Unable to read LXMax preferences file version. Resetting to default preferences..." << std
				::endl;
			
			create_default();
			return;
		}

		_preferences.clear();
		_preferences.copyunique(to_import);
	}

	void write_to_disk()
	{
        short pref_path;
        c74::max::preferences_path(nullptr, true, &pref_path);
        c74::max::dictionary_write(_preferences, (char*)k_preferences_filename, pref_path);
	}
};
