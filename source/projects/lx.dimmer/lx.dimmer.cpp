/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "version_info.hpp"
#include "c74_min.h"
#include "fixture.hpp"
#include "precision_helpers.hpp"
#include "common.hpp"

using namespace c74;
using namespace min;

enum_map lx_dimmer_precision_info = {
    "8-Bit",
    "16-Bit",
    "24-Bit",
    "32-Bit",
    "16-Bit (Little Endian)",
    "24-Bit (Little Endian)",
    "32-Bit (Little Endian)",
};

enum class lx_dimmer_range {
    normalized,         // 0-1
    percent,            // 0-100
    _8bit,              // 0-255
    midi,               // 0-127
    raw,                // 0-max precision
    enum_count
};

enum_map lx_dimmer_range_info = {
    "Normalized (0-1)",
    "Percent (0-100)",
    "8 bit (0-255)",
    "MIDI (0-127)",
    "Raw"
};

enum class lx_dimmer_priority {
	htp,
	ltp,
	enum_count
};

enum_map lx_dimmer_priority_info {
	"HTP (Highest Takes Precedence)",
	"LTP (Latest Takes Precedence)"
};

class lx_dimmer : public object<lx_dimmer>, public lxmax::fixture {

	instance _lxmax_service { };
	
	std::mutex _value_mutex;
    atoms _values;

	number _value_max { 1. };
	bool _is_little_endian { false };
	bool _is_int_only { false };
	int _precision_width { 1 };
	int _channel_count { 0 };
	
    void update_range(lx_dimmer_range r, lxmax::value_precision p)
    {
		std::lock_guard<std::mutex> lock(_value_mutex);
    	
	    const number old_max = _value_max;
        
        switch(r)
        {
            default:
            case lx_dimmer_range::normalized:
                _value_max = 1.;
                _is_int_only = false;
                break;
            case lx_dimmer_range::percent:
                _value_max = 100.;
                _is_int_only = false;
                break;
            case lx_dimmer_range::_8bit:
                _value_max = 255;
                _is_int_only = true;
                break;
            case lx_dimmer_range::midi:
                _value_max = 127;
                _is_int_only = true;
                break;
            case lx_dimmer_range::raw:
                _is_int_only = true;
                switch(p)
                {
                    default:
                    case lxmax::value_precision::_8bit:
                        _value_max = UCHAR_MAX;
                        break;
                    case lxmax::value_precision::_16bit:
                    case lxmax::value_precision::_16bit_le:
                        _value_max = USHRT_MAX;
                        break;
                    case lxmax::value_precision::_24bit:
                    case lxmax::value_precision::_24bit_le:
                        _value_max = 16777215;
                        break;
                    case lxmax::value_precision::_32bit:
                    case lxmax::value_precision::_32bit_le:
                        _value_max = UINT_MAX;
                        break;
                }
                break;
        }
        
        _is_little_endian = (p == lxmax::value_precision::_16bit_le
            || p == lxmax::value_precision::_24bit_le
            || p == lxmax::value_precision::_32bit_le);
        
        if (old_max != _value_max)
		{
            for (auto& _value : _values)
            {
	            _value = scale<number>(_value, 0., old_max, 0., _value_max);
                if (_is_int_only)
	                _value = round(double(_value));
            }
        }

		switch(p)
        {
            default:
            case lxmax::value_precision::_8bit:
                _channel_count = attr_num_dimmers.get();
				_precision_width = 1;
                break;
            case lxmax::value_precision::_16bit:
            case lxmax::value_precision::_16bit_le:
                _channel_count = attr_num_dimmers.get() * 2;
				_precision_width = 2;
                break;
            case lxmax::value_precision::_24bit:
            case lxmax::value_precision::_24bit_le:
                _channel_count = attr_num_dimmers.get() * 3;
				_precision_width = 3;
                break;
            case lxmax::value_precision::_32bit:
            case lxmax::value_precision::_32bit_le:
                _channel_count = attr_num_dimmers.get() * 4;
				_precision_width = 4;
                break;
        }

    	set_updated();
    }

public:

	MIN_DESCRIPTION	{"Send DMX control data to one or more dimmers."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler"};
	MIN_RELATED		{"lx.colorfixture"};

	inlet<>  input	{ this, "(list) list of dimmer values" };

	lx_dimmer(const atoms& args = {})
	{
        if (!maxobj())
			return;
		
        _lxmax_service = get_lxmax_service_and_check_version(*this, lxmax::GIT_VERSION_STR);
		set_manager(get_fixture_manager(*this, _lxmax_service));
		
		update_range(attr_input_range, attr_precision);
    }
    
    attribute<int, threadsafe::no, limit::clamp> attr_num_dimmers { this, "num_dimmers", 1,
        range { 1, 512 },
        title { "Number of Dimmers" },
        description { "Number of dimmers to control" },
        category {"lx.dimmer"}, order { 1 },
        getter { MIN_GETTER_FUNCTION {
            return { static_cast<int>(_values.size()) };
        }},
        setter { MIN_FUNCTION {

			std::lock_guard<std::mutex> lock(_value_mutex);
        	
			const int old_size = _values.size();
        	const int new_size = args[0];
            _values.resize(new_size);

        	for(int i = old_size; i < new_size; i++)
				_values[i] = 0;
        	
            return { static_cast<int>(_values.size()) };
        }}
    };
    
    attribute<int, threadsafe::no, limit::clamp> attr_channel { this, "channel", 1,
        range { 1, 512 },
        title { "DMX Channel" },
		description { "DMX start channel for dimmer(s)" },
        category {"lx.dimmer"}, order { 2 }
	};
    
    attribute<int, threadsafe::no, limit::clamp> attr_universe { this, "universe", 1,
        range { lxmax::k_universe_min, lxmax::k_universe_max },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.dimmer"}, order { 3 }
    };
                                    
    attribute<lxmax::value_precision> attr_precision { this, "precision",
        lxmax::value_precision::_8bit, lx_dimmer_precision_info,
        title { "Precision" },
        description { "Input value range" },
        category {"lx.dimmer"}, order { 4 },
        setter { MIN_FUNCTION {

            update_range(attr_input_range.get(), static_cast<lxmax::value_precision>(args[0]));
            return { args[0] };
        }}
    };
    
    attribute<lx_dimmer_range> attr_input_range { this, "range",
        lx_dimmer_range::normalized, lx_dimmer_range_info,
        title { "Range" },
        description { "Input value range" },
        category {"lx.dimmer"}, order { 5 },
        setter { MIN_FUNCTION {

            update_range(static_cast<lx_dimmer_range>(args[0]), attr_precision.get());
            return { args[0] };
        }}
    };

	attribute<lx_dimmer_priority> attr_priority { this, "priority",
		lx_dimmer_priority::htp, lx_dimmer_priority_info,
        title { "Priority" },
        description { "Priority mode when merging with other LXMax object's data" },
        category {"lx.dimmer"}, order { 3 }
    };
    
    attribute<numbers> attr_value { this, "value", { 0 },
        title { "Value" },
        description { "Dimmer value(s)" },
        category {"lx.dimmer"}, order { 6 },
        getter { MIN_GETTER_FUNCTION {
            return _values;
		}},
        setter { MIN_FUNCTION {

			std::lock_guard<std::mutex> lock(_value_mutex);
        	
            for(int i = 0; i < args.size() && i < _values.size(); ++i)
            {
                number v = args[i];
                _values[i] = MIN_CLAMP(v, 0, _value_max);
                 if (_is_int_only)
                     _values[i] = round(double(_values[i]));
            }

        	set_updated();
            
            return { _values };
        }}
    };

	
	argument<number> arg_num_dimmers { this, "number of dimmers", "Number of dimmers to control",
        MIN_ARGUMENT_FUNCTION {
            attr_num_dimmers = arg;
        }
    };
    
	argument<number> arg_channel { this, "channel", "DMX start channel for dimmer(s)",
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
	                     lxmax::universe_updated_list& updated_universes, bool is_force) override
    {
	    if (!is_force && !is_updated())
			return false;
		
	    const lxmax::universe_address universe = patch_info.channel_range.start_universe();
    	
    	updated_universes.push_back(universe);

	    const auto it = buffer_map.find(universe);

    	if (it == std::end(buffer_map))
			return false;

    	lxmax::universe_buffer buffer = it->second;

		{
    		std::lock_guard<std::mutex> lock(_value_mutex);

			int channel = patch_info.channel_range.start_local();

			auto value_it = std::begin(_values);
	    	
			while(channel + _precision_width <= lxmax::k_universe_length && value_it != std::end(_values))
			{
				write_with_precision(*value_it, _value_max, &buffer[channel], attr_precision);		
				++value_it;
				channel += _precision_width;
			}

	    	clear_updated(); 
		}

    	return true;
    }
};


MIN_EXTERNAL(lx_dimmer);
