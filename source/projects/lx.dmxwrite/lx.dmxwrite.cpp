/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "version_info.hpp"
#include "c74_min.h"
#include "fixture.hpp"
#include "precision_helpers.hpp"
#include "common.hpp"

using namespace c74::min;

class lx_dmxwrite : public object<lx_dmxwrite>, public lxmax::fixture {

	c74::max::t_object* _lxmax_service;
	std::shared_ptr<lxmax::fixture_manager> _fixture_manager;
	
	std::mutex _value_mutex;
    atoms _values;

	int _channel_count { 0 };

public:

	MIN_DESCRIPTION	{"Write raw DMX data to an LXMax universe"};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler"};
	MIN_RELATED		{"lx.dmxread"};

	inlet<>  input	{ this, "(list) list of dimmer values" };

	lx_dmxwrite(const atoms& args = {})
    {
        if (!maxobj())
			return;
		
        _lxmax_service = get_lxmax_service_and_check_version(maxobj(), lxmax::GIT_VERSION_STR);
        _fixture_manager = get_fixture_manager(maxobj(), _lxmax_service);
    }
    
    attribute<int, threadsafe::no, limit::clamp> attr_num_channels { this, "num_channels", 1,
        range { 1, 512 },
        title { "Number of Channels" },
        description { "Number of channels to write" },
        category {"lx.dmxwrite"}, order { 1 },
        getter { MIN_GETTER_FUNCTION {
            return { (int)_values.size() };
        }},
        setter { MIN_FUNCTION {

			std::lock_guard<std::mutex> lock(_value_mutex);
        	
			const int old_size = _values.size();
        	const int new_size = args[0];
            _values.resize(new_size);

        	for(int i = old_size; i < new_size; i++)
				_values[i] = 0;
        	
            return { (int)_values.size() };
        }}
    };
    
    attribute<int, threadsafe::no, limit::clamp> attr_channel { this, "channel", 1,
        range { 1, 512 },
        title { "DMX Channel" },
		description { "DMX start channel to write starting data from" },
        category {"lx.dmxwrite"}, order { 2 }
	};
    
    attribute<int, threadsafe::no, limit::clamp> attr_universe { this, "universe", 1,
        range { lxmax::k_universe_min, lxmax::k_universe_max },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.dmxwrite"}, order { 3 }
    };
	
	argument<number> arg_num_channels { this, "number of channels", "Number of channels write data to",
        MIN_ARGUMENT_FUNCTION {
            attr_num_channels = arg;
        }
    };
    
	argument<number> arg_channel { this, "channel", "DMX start channel to start writing data from",
		MIN_ARGUMENT_FUNCTION {
			attr_channel = arg;
		}
	};
    
    argument<number> arg_universe { this, "universe", "DMX universe number",
        MIN_ARGUMENT_FUNCTION {
            attr_universe = arg;
        }
    };

	

	bool write_to_buffer(const lxmax::fixture_patch_info& patch_info, lxmax::universe_buffer_map& buffer_map,
	                     lxmax::universe_updated_set& updated_universes, bool is_force) override
    {
	    if (!is_force && !is_updated())
			return false;
		
	    const lxmax::universe_address universe = patch_info.channel_range.start_universe();
    	
    	updated_universes.insert(universe);

	    const auto it = buffer_map.find(universe);

    	if (it == std::end(buffer_map))
			return false;

    	lxmax::universe_buffer buffer = it->second;

		{
    		std::lock_guard<std::mutex> lock(_value_mutex);

			int channel = patch_info.channel_range.start_local();

			auto value_it = std::begin(_values);
	    	
			while(channel <= lxmax::k_universe_length && value_it != std::end(_values))
				buffer[channel++] = static_cast<int>(*value_it++);

	    	clear_updated(); 
		}

    	return true;
    }
};


MIN_EXTERNAL(lx_dmxwrite);
