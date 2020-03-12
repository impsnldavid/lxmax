/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "version_info.hpp"
#include "dmx_output_service.hpp"
#include "fixture_manager.hpp"

#include <Poco/Logger.h>
#include <Poco/Delegate.h>

#include "c74_min.h"
#include "c74_min_flags.h"
#include "common.hpp"

#include "dict_edit.hpp"
#include "max_console_channel.hpp"
#include "preferences_manager.hpp"

using namespace c74;
using namespace min;

void on_max_quit(void* a)
{
	max::object_free(a);
}

class lxmax_service : public object<lxmax_service>
{
	static const inline string k_preferences_filename { "lxmaxpreferences.json" };
	static const inline string k_universes_dict_name { "___lxmax_universes" };

	const vector<dict_edit_column> k_editor_columns
	{
		{ "Label", 200, column_type::_text },
		{ "Type", 70, atoms { "Output", "Input" } },
		{ "On", 40, column_type::_toggle },
		{ "Protocol / Device", 155, atoms { "Art-Net", "sACN", "None" } },
		{ "Internal Universe", 90, 1, 63999 },
		{ "Protocol Universe", 90, 1, 63999 },
		{ "Settings", 200, column_type::_static }
	};

	dict_edit _universes_editor { k_universes_dict_name, k_editor_columns };
	bool _is_ignore_editor_notifications { false };
	void* _registered_obj = nullptr;

	std::unique_ptr<lxmax::preferences_manager> _preferences_manager;
	std::unique_ptr<lxmax::dmx_output_service> _dmx_output_service;
	std::unique_ptr<lxmax::fixture_manager> _fixture_manager;

	static std::string get_preference_path()
	{
		short pref_path;
		max::preferences_path(nullptr, true, &pref_path);

		char file_path[MAX_PATH_CHARS];
		max::path_toabsolutesystempath(pref_path, k_preferences_filename.c_str(), file_path);

		return file_path;
	}

	void update_editor_from_universes_config()
	{
		_is_ignore_editor_notifications = true;

		_universes_editor.clear_entries();
		
		const auto& universes = _preferences_manager->get_universe_configs();

		for(const auto& pair : universes)
		{
			const atoms a
			{
				pair.second->label,
				dmx_universe_type_to_string(pair.second->universe_type()),
				pair.second->is_enabled ? 1 : 0,
				dmx_protocol_to_string(pair.second->protocol),
				pair.second->internal_universe,
				pair.second->protocol_universe,
				pair.second->summary()
			};
			
			_universes_editor.add_entry(pair.first, a);
		}

		_is_ignore_editor_notifications = false;
	}

	void update_dmx_universe_config_from_editor()
	{
		_preferences_manager->set_is_events_disabled(true);
		
		auto entries = _universes_editor.get_entries();
		auto& configs = _preferences_manager->get_universe_configs();

		for(auto& e : entries)
		{
			auto config = configs.find(e.first);

			if (config == std::end(configs))
				continue;

			const lxmax::dmx_universe_type universe_type = lxmax::dmx_universe_type_from_string(e.second[1]);
			
			if (config->second->universe_type() != universe_type)
			{
				_preferences_manager->remove_universe(e.first);

				if (universe_type == lxmax::dmx_universe_type::input)
					_preferences_manager->add_universe(e.first, std::make_unique<lxmax::dmx_input_universe_config>());
				else
					_preferences_manager->add_universe(e.first, std::make_unique<lxmax::dmx_output_universe_config>());

				config = configs.find(e.first);
				assert(config != std::end(configs));
			}
			
			config->second->label = std::string(e.second[0]);
			config->second->is_enabled = e.second[2];
			config->second->protocol = lxmax::dmx_protocol_from_string(e.second[3]);
			config->second->internal_universe = e.second[4];
			config->second->protocol_universe = e.second[5];

			e.second[6] = config->second->summary();
			_universes_editor.add_entry(e.first, e.second);
		}

		_preferences_manager->save();

		_preferences_manager->set_is_events_disabled(false);
		_preferences_manager->fire_universe_config_changed();
	}

public:
	MIN_DESCRIPTION { "LXMax service object." };
	MIN_TAGS { "lxmax" };
	MIN_AUTHOR { "David Butler / The Impersonal Stereo" };
	MIN_RELATED { "lx.config, lx.dimmer, lx.colorfixture, lx.dmxwrite" };

	MIN_FLAGS { behavior_flags::nobox };

	lxmax_service(const atoms& args = { })
	{
		if (!maxobj())
			return;
		
		Poco::Logger& root_logger = Poco::Logger::root();
		root_logger.setChannel(Poco::AutoPtr<max_console_channel>(new max_console_channel(maxobj())));
		
#ifndef NDEBUG
		root_logger.setLevel(Poco::Message::Priority::PRIO_TRACE);
		root_logger.information("%s - DEBUG BUILD - %s - %s", 
			lxmax::GIT_VERSION_STR, lxmax::VERSION_COPYRIGHT_STR, lxmax::VERSION_URL_STR);
#else
		root_logger.setLevel(Poco::Message::Priority::PRIO_INFORMATION);
		root_logger.information("%s - %s - %s",
			lxmax::GIT_VERSION_STR, lxmax::VERSION_COPYRIGHT_STR, lxmax::VERSION_URL_STR);
#endif

		
		_preferences_manager = std::make_unique<lxmax::preferences_manager>(Poco::Logger::get("Preferences Manager"), get_preference_path()),
		_dmx_output_service = std::make_unique<lxmax::dmx_output_service>(Poco::Logger::get("DMX Output Service"));
		_fixture_manager = std::make_unique<lxmax::fixture_manager>();

		_preferences_manager->global_config_changed += Poco::delegate(_dmx_output_service.get(), &lxmax::dmx_output_service::update_global_config);
		_preferences_manager->universe_config_changed += Poco::delegate(_dmx_output_service.get(), &lxmax::dmx_output_service::update_universe_configs);
		
		_preferences_manager->load();

		_registered_obj = object_register(k_sym_nobox, k_lxmax_service_registration, this->maxobj());

		object_attach_byptr_register(maxobj(), _universes_editor, k_sym_box);

		update_editor_from_universes_config();

		_dmx_output_service->start();
	}

	~lxmax_service()
	{
		if (_dmx_output_service)
			_dmx_output_service->stop();

		object_detach_byptr(maxobj(), _universes_editor);

		if (_registered_obj != nullptr)
			max::object_unregister(_registered_obj);
	}

	message<> add_universe {
		this, "add_universe", "Adds a universe to the configuration", message_type::no_argument,
		MIN_FUNCTION
		{
			const auto& last_config = _preferences_manager->get_universe_configs().rbegin();

			if (last_config != std::rend(_preferences_manager->get_universe_configs()))
			{
				std::unique_ptr<lxmax::dmx_universe_config> config = last_config->second->clone();
				
				config->internal_universe++;
				config->protocol_universe++;
				config->label = "Universe " + std::to_string(config->internal_universe);

				_preferences_manager->add_universe(last_config->first + 1, std::move(config));
			}
			else
			{
				_preferences_manager->add_universe(1, std::make_unique<lxmax::dmx_output_universe_config>());
			}

			update_editor_from_universes_config();
			
			return { };
		}
	};

	message<> remove_universe {
		this, "remove_universe", "Removes a universe from the configuration", message_type::int_argument,
		MIN_FUNCTION
		{
			const int index = args[0];
			
			if (_preferences_manager->get_universe_configs().find(index) != std::end(_preferences_manager->get_universe_configs()))
				_preferences_manager->remove_universe(index);

			update_editor_from_universes_config();

			return { };
		}
	};

	message<> clear_universes {
		this, "clear_universes", "Removes all universes from the configuration", message_type::no_argument,
		MIN_FUNCTION
		{
			_preferences_manager->clear_universes();

			update_editor_from_universes_config();

			return { };
		}
	};

	message<> quick_patch {
		this, "quick_patch", "Quickly add a number of universes to the configuration",
		MIN_FUNCTION
		{
			if (args.size() != 6)
				return { };

			const bool should_clear_config = args[0];
			const int patch_count = args[1];
			const lxmax::dmx_universe_type type = lxmax::dmx_universe_type_from_string(args[2]);
			const lxmax::dmx_protocol protocol = lxmax::dmx_protocol_from_string(args[3]);
			const int start_internal_universe = args[4];
			const int start_protocol_universe = args[5];

			if (patch_count < 1 || patch_count > 65535)
				return { };

			if (type == lxmax::dmx_universe_type::none)
				return { };

			if (protocol == lxmax::dmx_protocol::none)
				return { };

			if (start_internal_universe < lxmax::k_universe_min || start_internal_universe > lxmax::k_universe_max)
			{
				return { };
			}
			
			if (protocol == lxmax::dmx_protocol::artnet && 
				(start_protocol_universe < lxmax::k_universe_artnet_min || start_protocol_universe > lxmax::k_universe_artnet_max))
			{
				return { };
			}

			if (protocol == lxmax::dmx_protocol::sacn && 
				(start_protocol_universe < lxmax::k_universe_sacn_min || start_protocol_universe > lxmax::k_universe_sacn_max))
			{
				return { };
			}

			if (should_clear_config)
				_preferences_manager->clear_universes();

			int index = 1;
			const auto& highest_universe = _preferences_manager->get_universe_configs().rbegin();
			if (highest_universe != std::rend(_preferences_manager->get_universe_configs()))
				index = highest_universe->first;
			
			int protocol_universe_max = 0;
			switch(protocol)
			{
				case lxmax::dmx_protocol::artnet:
					protocol_universe_max = lxmax::k_universe_artnet_max;
					break;
				case lxmax::dmx_protocol::sacn:
					protocol_universe_max = lxmax::k_universe_sacn_max;
					break;
				default:
				case lxmax::dmx_protocol::none:
					assert(false);
			}

			for (int i = 0; i < patch_count; ++i)
			{
				if (start_internal_universe + i > lxmax::k_universe_max)
					break;

				if (start_protocol_universe + i > protocol_universe_max)
					break;

				std::unique_ptr<lxmax::dmx_universe_config> config;
				if (type == lxmax::dmx_universe_type::input)
					config = std::make_unique<lxmax::dmx_input_universe_config>();
				else
					config = std::make_unique<lxmax::dmx_output_universe_config>();

				config->label = "Universe " + std::to_string(start_internal_universe + i);
				config->protocol = protocol;
				config->internal_universe = start_internal_universe + i;
				config->protocol_universe = start_protocol_universe + i;
					
				_preferences_manager->add_universe(index++, std::move(config));				
			}

			update_editor_from_universes_config();

			return { };
		}
	};

	message<> get_fixture_manager {
		this, "get_fixture_manager", "Gets a pointer to the fixture manager", message_type::gimmeback,
		MIN_FUNCTION
		{
			max::t_object* obj = (max::t_object*)_fixture_manager.get();
			
			return { obj };
		}
	};

	message<> get_global_preferences {
		this, "get_global_preferences", "Gets a dictionary containing the global preferences", message_type::gimmeback,
		MIN_FUNCTION
		{
			const std::string json = _preferences_manager->get_global_config_as_json_string();

			max::t_dictionary* d = nullptr;
			char error_string[256];
			dictobj_dictionaryfromstring(&d, json.c_str(), true, error_string);
			
			return { d };
		}
	};

	message<> get_universe_preferences {
		this, "get_universe_preferences", "Gets a dictionary containing a specified universe's preferences", message_type::gimmeback,
		MIN_FUNCTION
		{
			const int index = args[0];
			
			const std::string json = _preferences_manager->get_universe_config_as_json_string(index);

			max::t_dictionary* d = nullptr;
			char error_string[256];
			dictobj_dictionaryfromstring(&d, json.c_str(), true, error_string);
			
			return { d };
		}
	};

	message<> notify {
		this, "notify",
		MIN_FUNCTION
		{
			notification n { args };
			symbol attr_name { n.attr_name() };

			if (!_is_ignore_editor_notifications && n.source() == _universes_editor && n.name() == k_sym_modified)
			{
				update_dmx_universe_config_from_editor();
			}

			return { };
		}
	};

private:
	message<> maxclass_setup = {
		this, "maxclass_setup",
		MIN_FUNCTION
		{
			void* obj = max::object_new_typed(k_sym_nobox, symbol("lxmax"), 0, nullptr);
			max::quittask_install(reinterpret_cast<max::method>(on_max_quit), obj);

			return { };
		}
	};
};

MIN_EXTERNAL_CUSTOM(lxmax_service, "lxmax")
