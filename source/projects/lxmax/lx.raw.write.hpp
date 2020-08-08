/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "version_info.hpp"
#include "c74_min.h"
#include "fixture.hpp"
#include "precision_helpers.hpp"
#include "common.hpp"

using namespace c74::min;

class lx_raw_write : public object<lx_raw_write> {

	instance _lxmax_service { };
	
	std::mutex _value_mutex;
    atoms _values;

	int _channel_count { 0 };

public:

	MIN_DESCRIPTION	{"Write raw DMX data to an LXMax universe"};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler"};
	MIN_RELATED		{"lx.raw.read"};

	inlet<>  input	{ this, "(list) list of dimmer values" };

	lx_raw_write(const atoms& args = {})
    {
        if (dummy())
			return;
		
        _lxmax_service = get_lxmax_service_and_check_version(*this, lxmax::GIT_VERSION_STR);
    }
    
    attribute<int, threadsafe::no, limit::clamp> attr_num_channels { this, "num_channels", 1,
        range { 1, 512 },
        title { "Number of Channels" },
        description { "Number of channels to write" },
        category {"lx.raw.write"}, order { 1 },
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
        category {"lx.raw.write"}, order { 2 }
	};
    
    attribute<int, threadsafe::no, limit::clamp> attr_universe { this, "universe", 1,
        range { lxmax::k_universe_min, lxmax::k_universe_max },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.raw.write"}, order { 3 }
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
};

LXMAX_EXTERNAL(lx_raw_write)