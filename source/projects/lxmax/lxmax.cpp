/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "dmx_output_service.hpp"
#include "fixture_manager.hpp"

#include "c74_min.h"
#include "c74_min_flags.h"
#include "common.hpp"
#include "dict_edit.hpp"



using namespace c74::min;

void on_max_quit(void* a)
{
	c74::max::object_free(a);
}


class lxmax_service : public object<lxmax_service> {
    
    const string k_preferences_filename { "lxmaxpreferences.json" };
    const string k_preferences_dict_name { "___lxmax_preferences" };
    
    const vector<dict_edit_column> k_editor_columns
    {
        { "Label", 200, column_type::_text },
        { "Type", 70, atoms{ "Output", "Input" } },
        { "On", 40, column_type::_toggle },
        { "Protocol / Device", 155, atoms{ "Art-Net", "sACN" } },
        { "Internal Universe", 90, 1, 65535 },
        { "Settings", 200, column_type::_static }
    };
    
    dict_edit _preferences { k_preferences_dict_name, k_editor_columns };
    void* _registered_obj = nullptr;

	lxmax::dmx_output_service _dmx_output_service;

	lxmax::fixture_manager _fixture_manager;

    void write_preferences()
    {
        _preferences.write_to_max_preferences(k_preferences_filename);
    }

	void update_dmx_service_config()
    {
    	auto entries = _preferences.get_entries();

        std::vector<lxmax::dmx_output_universe_config> output_configs;
    	
    	for(auto entry : entries)
    	{
            std::string label = entry.second[0];
    		std::string type = entry.second[1];
    		bool isEnabled = entry.second[2];
    		std::string protocol = entry.second[3];
    		int internalUniverse = entry.second[4];

            if (!isEnabled)
                continue;
    		
            if (type == "Input")
            {
	            
            }
            else if (type == "Output")
            {
	            lxmax::dmx_output_universe_config config;

            	if (protocol == "Art-Net")
                    config.protocol = lxmax::dmx_protocol::artnet;
                else if (protocol == "sACN")
                    config.protocol = lxmax::dmx_protocol::sacn;
                else
					assert(false);

            	config.internal_universe = internalUniverse;
            	config.protocol_universe = internalUniverse;
            	config.send_address = Poco::Net::IPAddress("127.0.0.1");

            	output_configs.push_back(config);
            }
            else
            {
	            assert(false);
            }
    	}

    	_dmx_output_service.update_configs(output_configs);
    }
    
public:
	MIN_DESCRIPTION	{"LXMax service object."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler / The Impersonal Stereo"};
	MIN_RELATED		{"lx.dimmer, lx.colorfixture"};
    
    MIN_FLAGS       { behavior_flags::nobox };

    lxmax_service(const atoms& args = {})
    {
        _preferences.read_from_max_preferences(k_preferences_filename);
            
        _registered_obj = c74::max::object_register(k_lxmax_namespace, k_lxmax_service_registration, this->maxobj());

        c74::max::object_attach_byptr_register(maxobj(), _preferences, k_sym_box);
    	
        update_dmx_service_config();
    	
		_dmx_output_service.start();
    }
            
    ~lxmax_service()
    {
		_dmx_output_service.stop();

    	c74::max::object_detach_byptr(maxobj(), _preferences);

        write_preferences();

        if (_registered_obj != nullptr)
            c74::max::object_unregister(_registered_obj);
    }

    message<> add_universe { this, "add_universe", "Adds a universe to the configuration",
        MIN_FUNCTION {
            
        	const auto entries = _preferences.get_entries();
        	
        	if (entries.empty())
        	{
	            _preferences.add_entry(1, { "Universe 1", "Output", 1, "sACN", 1, "[prefs]"});
            }
            else
            {
	            std::string type = entries.rbegin()->second[1];
	            std::string protocol = entries.rbegin()->second[3];
	            int internal_universe = entries.rbegin()->second[4];

	            _preferences.add_entry(entries.rbegin()->first + 1, 
	                                   {"Universe " + std::to_string(internal_universe + 1), type, 1, protocol, internal_universe + 1, "[prefs]"});
            }
            
            update_dmx_service_config();
        	
            return {};
        }
    };
    
    message<> remove_universe { this, "remove_universe", "Removes a universe from the configuration",
        MIN_FUNCTION {
            
            const int index = args[0];
            _preferences.remove_entry(index);

        	 update_dmx_service_config();
            
            return {};
        }
    };
            
    message<> clear_universes { this, "clear_universes", "Removes all universes from the configuration",
        MIN_FUNCTION {
            
            _preferences.clear_entries();

        	 update_dmx_service_config();
            
            return {};
        }
    };

	message<> quickpatch { this, "quickpatch", "Quickly add a number of universes to the configuration",
        MIN_FUNCTION {

			if (args.size() != 5)
				return {};

			const bool should_clear_config = args[0];
			const int patch_count = args[1];
			const symbol type = args[2];
			const symbol protocol = args[3];
			const int start_internal_universe = args[4];

            if (patch_count < 1 || patch_count > 65535)
                return {};

        	if (type != "Input" && type != "Output")
                return {};

        	if (protocol != "Art-Net" && protocol != "sACN")
                return {};

        	if (start_internal_universe < 1 || start_internal_universe > lxmax::k_universe_max)
                return {};
        	
        	if (should_clear_config)
				_preferences.clear_entries();

        	int index = _preferences.highest_index() + 1;

        	for(int i = 0; i < patch_count; ++i)
        	{
                if (start_internal_universe + i > lxmax::k_universe_max)
                    break;
        		
        		_preferences.add_entry(index++, { "Universe " + std::to_string(start_internal_universe + i),
                    type, start_internal_universe + i, protocol, start_internal_universe + i, "[prefs]"});
        	}

        	 update_dmx_service_config();
            
            return {};
        }
    };

	message<> get_fixture_manager { this, "get_fixture_manager", "Gets a pointer to the fixture manager",
		MIN_FUNCTION {
			
            return { &_fixture_manager };
        }
	};

	message<> notify { this, "notify", 
		MIN_FUNCTION {
			
			notification n { args };
			symbol attr_name { n.attr_name() };

            if (n.source() == _preferences && n.name() == k_sym_modified)
            {
				update_dmx_service_config();
            }

			return {};
		}
	};

private:
	message<> maxclass_setup = { this, "maxclass_setup", 
		MIN_FUNCTION {

			cout << "LXMax - v0.0.1 - David Butler / The Impersonal Stereo" << endl;

			void* obj = c74::max::object_new_typed(symbol("nobox"), symbol("lxmax"), 0, nullptr);
			c74::max::quittask_install((c74::max::method)on_max_quit, obj);
			
			return {};
		}
	};
};

MIN_EXTERNAL_CUSTOM(lxmax_service, "lxmax")