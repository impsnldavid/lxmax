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

#include "version_info.hpp"
#include "global_config.hpp"
#include "dmx_universe_config.hpp"

namespace lxmax
{
	class preferences_manager
	{
		const std::string k_lxmax_version_major{"lxmax_version_major"};
		const std::string k_lxmax_version_minor{"lxmax_version_minor"};
		const std::string k_lxmax_version_bugfix{"lxmax_version_bugfix"};

		const std::string k_global_preferences{"global_preferences"};
		const std::string k_input_universes{"input_universes"};
		const std::string k_output_universes{"output_universes"};
		
		Poco::Logger& _log;
		
		const std::string _preferences_path;

		global_config _global_config;
		std::map<int, dmx_input_universe_config> _input_universe_configs;
		std::map<int, dmx_output_universe_config> _output_universe_configs;
		
		
	public:
		preferences_manager(Poco::Logger& log, std::string preferences_path)
			: _log(log),
			_preferences_path(std::move(preferences_path))
		{
			
		}

		void create_default()
		{
			_global_config = { };
			_input_universe_configs.clear();
			_output_universe_configs.clear();

			save();
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

				std::vector<std::string> input_universe_keys;
				preferences.keys(k_input_universes, input_universe_keys);

				for(const auto& k : input_universe_keys)
				{
					int index;
					if (!Poco::NumberParser::tryParse(k, index))
					{
						poco_error_f(_log, "Failed to parse input universe configuration key '%s'", k);
						continue;
					}

					dmx_input_universe_config config;
					config.read_from_configuration(preferences.createView(k));

					_input_universe_configs.insert(std::make_pair(index, config));
				}

				std::vector<std::string> output_universe_keys;
				preferences.keys(k_output_universes, output_universe_keys);\

				for(const auto& k : output_universe_keys)
				{
					int index;
					if (!Poco::NumberParser::tryParse(k, index))
					{
						poco_error_f(_log, "Failed to parse output universe configuration key '%s'", k);
						continue;
					}

					dmx_output_universe_config config;
					config.read_from_configuration(preferences.createView(k));

					_output_universe_configs.insert(std::make_pair(index, config));
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
		}
		
		void save()
		{
			Poco::Util::JSONConfiguration preferences;

			preferences.setInt(k_lxmax_version_major, GIT_VERSION_MAJOR);
			preferences.setInt(k_lxmax_version_minor, GIT_VERSION_MINOR);
			preferences.setInt(k_lxmax_version_bugfix, GIT_VERSION_BUGFIX);

			auto global_preferences = preferences.createView(k_global_preferences);
			_global_config.write_to_configuration(global_preferences);

			auto input_universes_preferences = preferences.createView(k_input_universes);
			for(const auto& pair : _input_universe_configs)
			{
				auto p = input_universes_preferences->createView(std::to_string(pair.first));
				pair.second.write_to_configuration(p);
			}

			auto output_universes_preferences = preferences.createView(k_output_universes);
			for(const auto& pair : _output_universe_configs)
			{
				auto p = output_universes_preferences->createView(std::to_string(pair.first));
				pair.second.write_to_configuration(p);
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

		void set_global_config(global_config config)
		{
			_global_config = std::move(config);
			save();
		}

		const std::map<int, dmx_input_universe_config>& get_input_universes() const
		{
			return _input_universe_configs;
		}

		void set_input_universes(std::map<int, dmx_input_universe_config> configs)
		{
			_input_universe_configs = std::move(configs);
			save();
		}

		const std::map<int, dmx_output_universe_config>& get_output_universes() const
		{
			return _output_universe_configs;
		}

		void set_output_universes(std::map<int, dmx_output_universe_config> configs)
		{
			_output_universe_configs = std::move(configs);
			save();
		}
	};
}