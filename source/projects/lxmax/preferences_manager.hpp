/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <string>
#include <utility>
#include <Poco/File.h>

#include "c74_min.h"
#include "dict_edit.hpp"
#include "dmx_output_service.hpp"
#include "version_info.hpp"
#include "dmx_universe_config.hpp"

class preferences_manager
{
	const std::string _preferences_file_name;

	lxmax::global_config _global_config;
	std::map<int, lxmax::dmx_input_universe_config> _input_universes;
	std::map<int, lxmax::dmx_output_universe_config> _output_universes;
	
	const c74::min::symbol k_sym_lxmax_version_major{"lxmax_version_major"};
	const c74::min::symbol k_sym_lxmax_version_minor{"lxmax_version_minor"};
	const c74::min::symbol k_sym_lxmax_version_bugfix{"lxmax_version_bugfix"};

	const c74::min::symbol k_sym_global_preferences{"global_preferences"};
	const c74::min::symbol k_sym_input_universes{"input_universes"};
	const c74::min::symbol k_sym_output_universes{"output_universes"};

public:
	preferences_manager(std::string preferences_file_name)
		: _preferences_file_name(std::move(preferences_file_name))
	{
		
	}
	
	void create_default()
	{
		_global_config = lxmax::global_config {};
		_input_universes.clear();
		_output_universes.clear();

		write_to_disk();
	}

	void read_from_disk()
	{
		short pref_path;
		c74::max::preferences_path(nullptr, true, &pref_path);

		char file_path[MAX_PATH_CHARS];
		c74::max::path_toabsolutesystempath(pref_path, _preferences_file_name.c_str(), file_path);

        Poco::File file(file_path);
		if (!file.exists())
		{
			create_default();
			return;
		}

		c74::max::t_dictionary* d;
		c74::max::dictionary_read(const_cast<char*>(_preferences_file_name.c_str()), pref_path, &d);

		if (d == nullptr)
		{
			std::cerr << "Unable to read LXMax preferences file. Resetting to default preferences..." << std::endl;
			create_default();
			return;
		}
		
		c74::min::dict preferences{d};

		try
		{
			const c74::min::atom version_major = preferences.at(k_sym_lxmax_version_major);
			const c74::min::atom version_minor = preferences.at(k_sym_lxmax_version_minor);
			const c74::min::atom version_bugfix = preferences.at(k_sym_lxmax_version_bugfix);

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

		c74::min::dict global_preferences {c74::min::atom(preferences[k_sym_global_preferences])};
		parse_global_config(global_preferences);
	}

	void write_to_disk()
	{
		c74::min::dict preferences;

		preferences[k_sym_lxmax_version_major] = lxmax::GIT_VERSION_MAJOR;
		preferences[k_sym_lxmax_version_minor] = lxmax::GIT_VERSION_MINOR;
		preferences[k_sym_lxmax_version_bugfix] = lxmax::GIT_VERSION_BUFGIX;

		const c74::min::dict global_preferences = global_config_to_dict(_global_config);		
		preferences[k_sym_global_preferences] = global_preferences;

		const c74::min::dict input_universes = input_universes_to_dict(_input_universes);
		preferences[k_sym_input_universes] = input_universes;

		const c74::min::dict output_universes = output_universes_to_dict(_output_universes);
		preferences[k_sym_output_universes] = output_universes;
		
        short pref_path;
        c74::max::preferences_path(nullptr, true, &pref_path);
        c74::max::dictionary_write(preferences, const_cast<char*>(_preferences_file_name.c_str()), pref_path);
	}
	
	void set_global_config(const lxmax::global_config& config)
	{
		_global_config = config;
		write_to_disk();
	}


	void update_universes_editor(dict_edit& editor)
	{
		
	}

	void add_universe(lxmax::dmx_output_universe_config& config)
	{
		
	}

	void remove_universe(int index)
	{
		
	}



private:
	static c74::min::dict global_config_to_dict(const lxmax::global_config& config)
	{
		c74::min::dict global_preferences;

		global_preferences[lxmax::global_config::key_is_output_empty_universes] = config.is_output_empty_universes;
		global_preferences[lxmax::global_config::key_is_force_output_at_framerate] = config.is_force_output_at_framerate;
		global_preferences[lxmax::global_config::key_framerate] = config.framerate;
		global_preferences[lxmax::global_config::key_is_allow_nondmx_framerate] = config.is_allow_nondmx_framerate;

		global_preferences[lxmax::global_config::key_artnet_network_adapter] = config.artnet_network_adapter.toString();
		global_preferences[lxmax::global_config::key_is_artnet_global_destination_broadcast] = config.is_artnet_global_destination_broadcast;

		std::vector<c74::max::t_atom> artnet_ip_atoms(config.artnet_global_destination_unicast_addresses.size());
		for(size_t i = 0; i < config.artnet_global_destination_unicast_addresses.size(); ++i)
			c74::max::atom_setsym(&artnet_ip_atoms[i], c74::min::symbol(config.artnet_global_destination_unicast_addresses[i].toString()));

		c74::max::dictionary_appendatoms(global_preferences, c74::min::symbol(lxmax::global_config::key_artnet_global_destination_unicast_addresses),
			artnet_ip_atoms.size(), artnet_ip_atoms.data());

		global_preferences[lxmax::global_config::key_is_send_artnet_sync_packets] = config.is_send_artnet_sync_packets;
		
		global_preferences[lxmax::global_config::key_sacn_network_adapter] = config.sacn_network_adapter.toString();
		global_preferences[lxmax::global_config::key_is_sacn_global_destination_multicast] = config.is_sacn_global_destination_multicast;

		std::vector<c74::max::t_atom> sacn_ip_atoms(config.sacn_global_destination_unicast_addresses.size());
		for(size_t i = 0; i < config.sacn_global_destination_unicast_addresses.size(); ++i)
			c74::max::atom_setsym(&sacn_ip_atoms[i], c74::min::symbol(config.sacn_global_destination_unicast_addresses[i].toString()));

		c74::max::dictionary_appendatoms(global_preferences, c74::min::symbol(lxmax::global_config::key_sacn_global_destination_unicast_addresses),
			sacn_ip_atoms.size(), sacn_ip_atoms.data());

		global_preferences[lxmax::global_config::key_is_send_sacn_sync_packets] = config.is_send_sacn_sync_packets;

		return global_preferences;
	}

	void parse_global_config(c74::min::dict& d)
	{
		lxmax::global_config config;

		try
		{
			config.is_output_empty_universes
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_output_empty_universes)));
			config.is_force_output_at_framerate \
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_force_output_at_framerate)));
			config.framerate
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_framerate)).begin());
			config.is_allow_nondmx_framerate
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_allow_nondmx_framerate)));

			const std::string artnet_network_adapter_raw
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_artnet_network_adapter)));
			if (!Poco::Net::IPAddress::tryParse(artnet_network_adapter_raw, config.artnet_network_adapter))
			{
				std::cerr << "Unable to parse LXMax sACN network adapter local address '" << artnet_network_adapter_raw
					<< "' from preferences. Resetting to default..." << std::endl;
				config.artnet_network_adapter = Poco::Net::IPAddress("0.0.0.0");
			}
			
			config.is_artnet_global_destination_broadcast
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_artnet_global_destination_broadcast)));

			for(auto const& a : d.at(c74::min::symbol(lxmax::global_config::key_artnet_global_destination_unicast_addresses)))
			{
				std::string ip_string = c74::min::atom(a);
				Poco::Net::IPAddress ip;
				if (!Poco::Net::IPAddress::tryParse(ip_string, ip))
				{
					std::cerr << "Unable to parse LXMax Art-Net unicast IP address '" << ip_string << "' from preferences." << std::endl;
					continue;
				}
				
				config.artnet_global_destination_unicast_addresses.push_back(ip);
			}

			config.is_send_artnet_sync_packets
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_send_artnet_sync_packets)));


			const std::string sacn_network_adapter_raw
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_sacn_network_adapter)));
			if (!Poco::Net::IPAddress::tryParse(sacn_network_adapter_raw, config.sacn_network_adapter))
			{
				std::cerr << "Unable to parse LXMax sACN network adapter local address '" << sacn_network_adapter_raw
					<< "' from preferences. Resetting to default..." << std::endl;
				config.sacn_network_adapter = Poco::Net::IPAddress("0.0.0.0");
			}

			config.is_sacn_global_destination_multicast
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_sacn_global_destination_multicast)));

			for(auto const& a : d.at(c74::min::symbol(lxmax::global_config::key_sacn_global_destination_unicast_addresses)))
			{
				std::string ip_string = c74::min::atom(a);
				Poco::Net::IPAddress ip;
				if (!Poco::Net::IPAddress::tryParse(ip_string, ip))
				{
					std::cerr << "Unable to parse LXMax sACN unicast IP address '" << ip_string << "' from preferences." << std::endl;
					continue;
				}
				
				config.sacn_global_destination_unicast_addresses.push_back(ip);
			}

			config.is_send_sacn_sync_packets
				= c74::min::atom(d.at(c74::min::symbol(lxmax::global_config::key_is_send_sacn_sync_packets)));
		}
		catch(const std::runtime_error& ex)
		{
			std::cerr << "Unable to parse LXMax global preferences. Resetting to default global preferences..." << std::endl;
			config = lxmax::global_config {};
		}

		_global_config = config;
	}

	static c74::min::dict input_universe_to_dict(const lxmax::dmx_input_universe_config& universe)
	{
		
	}

	static c74::min::dict output_universe_to_dict(const lxmax::dmx_output_universe_config& universe)
	{
		
	}
	
	static c74::min::dict input_universes_to_dict(const std::map<int, lxmax::dmx_input_universe_config>& universes)
	{
		c74::min::dict d;
		
		for(const auto& pair : universes)
			d[pair.first] = input_universe_to_dict(pair.second);

		return d;
	}

	static c74::min::dict output_universes_to_dict(const std::map<int, lxmax::dmx_output_universe_config>& universes)
	{
		c74::min::dict d;
		
		for(const auto& pair : universes)
			d[pair.first] = output_universe_to_dict(pair.second);

		return d;
	}

	

	static lxmax::dmx_input_universe_config dict_to_input_universe(const c74::min::dict& d)
	{
		
	}

	static lxmax::dmx_output_universe_config dict_to_output_universe(const c74::min::dict& d)
	{
		
	}

	void parse_input_universes(c74::min::dict& d)
	{
		_input_universes.clear();
		
		long num_keys;
		c74::max::t_symbol** keys;
		c74::max::dictionary_getkeys(d, &num_keys, &keys);

		for(int i = 0; i < num_keys; ++i)
		{
			int key = c74::min::symbol(keys[i]);
			_input_universes.insert(std::pair(key, dict_to_input_universe(c74::min::atom(d.at(key)))));
		}
	}

	void parse_output_universes(c74::min::dict& d)
	{
		_output_universes.clear();
		
		long num_keys;
		c74::max::t_symbol** keys;
		c74::max::dictionary_getkeys(d, &num_keys, &keys);

		for(int i = 0; i < num_keys; ++i)
		{
			int key = c74::min::symbol(keys[i]);
			_output_universes.insert(std::pair(key, dict_to_output_universe(c74::min::atom(d.at(key)))));
		}
	}


};
