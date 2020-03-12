/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <string>
#include <utility>
#include <vector>
#include <map>
#include <Poco/Logger.h>
#include <Poco/FileStream.h>
#include <Poco/File.h>
#include <Poco/LineEndingConverter.h>
#include <Poco/Util/JSONConfiguration.h>
#include <Poco/NumberParser.h>
#include <Poco/BasicEvent.h>

#include "version_info.hpp"
#include "global_config.hpp"
#include "dmx_universe_config.hpp"

namespace lxmax
{
	using dmx_universe_configs = std::map<int, std::unique_ptr<dmx_universe_config>>;
	
	class preferences_manager
	{
		const std::string k_lxmax_version_major{"lxmax_version_major"};
		const std::string k_lxmax_version_minor{"lxmax_version_minor"};
		const std::string k_lxmax_version_bugfix{"lxmax_version_bugfix"};

		const std::string k_global_preferences{"global_preferences"};
		const std::string k_universes{"universes"};
		
		Poco::Logger& _log;
		
		const std::string _preferences_path;

		global_config _global_config;
		dmx_universe_configs _universe_configs;

		bool _is_events_disabled { false };
		bool _is_pending_global_config_event { false };
		bool _is_pending_universe_config_event { false };

		template<class T>
		static std::string get_json_string(T config)
		{
			Poco::AutoPtr<Poco::Util::JSONConfiguration> json(new Poco::Util::JSONConfiguration());
			auto c = json.cast<Poco::Util::AbstractConfiguration>();
			config->write_to_configuration(c);

			std::stringstream ss;
			Poco::OutputLineEndingConverter lec(ss);
			json->save(lec);
			lec.flush();
			ss.flush();

			return ss.str();
		}

		
	public:
		Poco::BasicEvent<void> global_config_changed;
		Poco::BasicEvent<void> universe_config_changed;
		
		preferences_manager(Poco::Logger& log, std::string preferences_path)
			: _log(log),
			_preferences_path(std::move(preferences_path))
		{
			
		}

		bool is_events_disabled() const
		{
			return _is_events_disabled;
		}

		void set_is_events_disabled(bool value, bool is_fire_pending = false)
		{
			if (value == _is_events_disabled)
				return;

			_is_events_disabled = value;

			if (!_is_events_disabled && _is_pending_global_config_event)
			{
				if (is_fire_pending)
					global_config_changed(this);
				_is_pending_global_config_event = false;
			}

			if (!_is_events_disabled && _is_pending_universe_config_event)
			{
				if (is_fire_pending)
					universe_config_changed(this);
				_is_pending_universe_config_event = false;
			}
		}

		void fire_global_config_changed()
		{
			if (_is_events_disabled)
				_is_pending_global_config_event = true;
			else
				global_config_changed(this);
		}

		void fire_universe_config_changed()
		{
			if (_is_events_disabled)
				_is_pending_universe_config_event = true;
			else
				universe_config_changed(this);
		}
		
		void create_default()
		{
			_global_config = { };
			_universe_configs.clear();

			save();
			fire_global_config_changed();
			fire_universe_config_changed();
		}

		void load()
		{
			if (!Poco::File(_preferences_path).exists())
			{
				poco_information(_log, "No preference file found. Creating default LXMax preferences...");
				create_default();
				return;
			}
			
			try
			{
				const Poco::Util::JSONConfiguration preferences(_preferences_path);

				const int version_major = preferences.getInt(k_lxmax_version_major);
				const int version_minor = preferences.getInt(k_lxmax_version_minor);
				const int version_bugfix = preferences.getInt(k_lxmax_version_bugfix);

				if (int(version_major) != GIT_VERSION_MAJOR
					|| int(version_minor) != GIT_VERSION_MAJOR
					|| int(version_bugfix) != GIT_VERSION_BUGFIX)
				{
					poco_warning_f(_log, "Preferences data is for version %i.%i.%i of LXMax. Current version is %s. Resetting to default preferences...",
						version_major, version_minor, version_bugfix, GIT_VERSION_SHORT_STR);
					
					create_default();
					return;
				}

				_global_config.read_from_configuration(preferences.createView(k_global_preferences));

				std::vector<std::string> universe_keys;
				auto universes_config = preferences.createView(k_universes);
				universes_config->keys(universe_keys);

				for(const auto& k : universe_keys)
				{
					int index;
					if (!Poco::NumberParser::tryParse(k, index))
					{
						poco_error_f(_log, "Failed to parse input universe configuration key '%s'", k);
						continue;
					}

					std::unique_ptr<dmx_universe_config> config = dmx_universe_config::create_from_configuration(universes_config->createView(k));
					_universe_configs.insert(std::make_pair(index, std::move(config)));
				}
			}
			catch(const Poco::NotFoundException& ex)
			{
				poco_error_f(_log, "Failed to find property '%s' in saved preferences. Reseting to default preferences...", ex.name());
				
				create_default();
			}
			catch(const Poco::SyntaxException& ex)
			{
				poco_error_f(_log, "Failed to pase property '%s' from saved preferences. Reseting to default preferences...", ex.name());
				
				create_default();
			}
			catch(...)
			{
				poco_error(_log, "Failed to read preferences file. Reseting to default preferences...");
				
				create_default();
			}

			fire_global_config_changed();
			fire_universe_config_changed();
		}
		
		void save()
		{
			Poco::Util::JSONConfiguration preferences;

			preferences.setInt(k_lxmax_version_major, GIT_VERSION_MAJOR);
			preferences.setInt(k_lxmax_version_minor, GIT_VERSION_MINOR);
			preferences.setInt(k_lxmax_version_bugfix, GIT_VERSION_BUGFIX);

			auto global_preferences = preferences.createView(k_global_preferences);
			_global_config.write_to_configuration(global_preferences);

			auto universe_preferences = preferences.createView(k_universes);
			for(const auto& pair : _universe_configs)
			{
				auto p = universe_preferences->createView(std::to_string(pair.first));
				pair.second->write_to_configuration(p);
			}

			try
			{
				Poco::FileOutputStream fs(_preferences_path);
				if (fs.good())
				{
					Poco::OutputLineEndingConverter lec(fs);
					preferences.save(lec);
					lec.flush();
					fs.flush();
					
					if (!fs.good())
					{
						poco_error(_log, "Failed to save preferences to file");
					}
				}
				else
				{
					poco_error(_log, "Failed to open/create preferences file");
				}
			}
			catch(const Poco::FileException& ex)
			{
				poco_error_f(_log, "Failed to save preferences to file - %s", ex.message());
			}
		}

		global_config get_global_config() const
		{
			return _global_config;
		}

		std::string get_global_config_as_json_string() const
		{
			return get_json_string(&_global_config);
		}

		void set_global_config(global_config config)
		{
			_global_config = std::move(config);
			save();
			fire_global_config_changed();
		}

		const dmx_universe_configs& get_universe_configs() const
		{
			return _universe_configs;
		}

		std::string get_universe_config_as_json_string(int index)
		{
			const auto& universe = _universe_configs.find(index);

			if (universe == std::end(_universe_configs))
				return "";
			
			return get_json_string(universe->second.get());
		}

		void set_universes_configs(dmx_universe_configs configs)
		{
			_universe_configs = std::move(configs);
			save();
			fire_universe_config_changed();
		}

		void add_universe(int key, std::unique_ptr<dmx_universe_config> config)
		{
			_universe_configs.insert_or_assign(key, std::move(config));
			save();
			fire_universe_config_changed();
		}

		void remove_universe(int key)
		{
			_universe_configs.erase(key);
			save();
			fire_universe_config_changed();
		}

		void clear_universes()
		{
			_universe_configs.clear();
			save();
			fire_universe_config_changed();
		}

		
	};
}