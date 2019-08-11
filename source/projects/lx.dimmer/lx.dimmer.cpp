/// @file
/// @ingroup     lxmax
/// @copyright    Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "double_buffer.hpp"

using namespace c74::min;

enum class lx_dimmer_precision {
    _8bit,
    _16bit,
    _24bit,
    _32bit,
    _16bit_le,
    _24bit_le,
    _32bit_le,
    enum_count
};

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

class lx_dimmer : public object<lx_dimmer> {
    
    atoms _values;
    
    number _value_max;
    bool _is_little_endian;
    bool _is_int_only;
    
    void update_range(lx_dimmer_range r, lx_dimmer_precision p)
    {
        number old_max = _value_max;
        
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
                    case lx_dimmer_precision::_8bit:
                        _value_max = UCHAR_MAX;
                        break;
                    case lx_dimmer_precision::_16bit:
                    case lx_dimmer_precision::_16bit_le:
                        _value_max = USHRT_MAX;
                        break;
                    case lx_dimmer_precision::_24bit:
                    case lx_dimmer_precision::_24bit_le:
                        _value_max = 16777215;
                        break;
                    case lx_dimmer_precision::_32bit:
                    case lx_dimmer_precision::_32bit_le:
                        _value_max = UINT_MAX;
                        break;
                }
                break;
        }
        
        _is_little_endian = (p == lx_dimmer_precision::_16bit_le
            || p == lx_dimmer_precision::_24bit_le
            || p == lx_dimmer_precision::_32bit_le);
        
        if (old_max != _value_max)
        {
            for(int i = 0; i < _values.size(); ++i)
            {
                _values[i] = scale<number>(_values[i], 0., old_max, 0., _value_max);
                if (_is_int_only)
                    _values[i] = round(double(_values[i]));
            }
        }
    }
    
public:
	MIN_DESCRIPTION	{"Send DMX control data to one or more dimmers."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler / The Impersonal Stereo"};
	MIN_RELATED		{"lx.colorfixture"};

	inlet<>  input	{ this, "(list) list of dimmer values" };

    argument<number> num_dimmers_arg { this, "number of dimmers", "Number of dimmers to control",
        MIN_ARGUMENT_FUNCTION {
            num_dimmers = arg;
        }
    };
    
	argument<number> channel_arg { this, "channel", "DMX start channel for dimmer(s)",
		MIN_ARGUMENT_FUNCTION {
			channel = arg;
		}
	};
    
    argument<number> universe_arg { this, "universe", "DMX universe number",
        MIN_ARGUMENT_FUNCTION {
            universe = arg;
        }
    };


    
    attribute<int, threadsafe::no, limit::clamp> num_dimmers { this, "num_dimmers", 1,
        range { 1, 512 },
        title { "Number of Dimmers" },
        description { "Number of dimmers to control" },
        category {"lx.dimmer"}, order { 1 },
        getter { MIN_GETTER_FUNCTION {
            return { (int)_values.size() };
        }},
        setter { MIN_FUNCTION {
            int value = args[0];
            int old_size = _values.size();
            _values.resize(value);
            
            if (value > old_size)
            {
                for (int i = old_size; i < value; ++i)
                    _values[i] = 0.;
            }
            
            return { };
        }}
    };
    
    attribute<int, threadsafe::no, limit::clamp> channel { this, "channel", 1,
        range { 1, 512 },
        title { "DMX Channel" },
		description { "DMX start channel for dimmer(s)" },
        category {"lx.dimmer"}, order { 2 },
        setter { MIN_FUNCTION {
            int value = args[0];
            return { MIN_CLAMP(value, 1, 512) };
        }}
	};
    
    attribute<int, threadsafe::no, limit::clamp> universe { this, "universe", 1,
        range { 1, 65535 },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.dimmer"}, order { 3 }
    };
                                    
    attribute<lx_dimmer_precision> precision { this, "precision",
        lx_dimmer_precision::_8bit, lx_dimmer_precision_info,
        title { "Precision" },
        description { "Input value range" },
        category {"lx.dimmer"}, order { 4 },
        setter { MIN_FUNCTION {
            
            update_range(input_range, (lx_dimmer_precision)args[0]);
            return { args[0] };
        }}
    };
    
    attribute<lx_dimmer_range> input_range { this, "range",
        lx_dimmer_range::normalized, lx_dimmer_range_info,
        title { "Range" },
        description { "Input value range" },
        category {"lx.dimmer"}, order { 5 },
        setter { MIN_FUNCTION {
            
            update_range((lx_dimmer_range)args[0], precision);
            return { args[0] };
        }}
    };
    
    attribute<numbers> value { this, "value", { 0 },
        title { "Value" },
        description { "Dimmer value(s)" },
        category {"lx.dimmer"}, order { 6 },
        getter { MIN_GETTER_FUNCTION {
            return _values;
        }},
        setter { MIN_FUNCTION {
            
            for(int i = 0; i < args.size() && i < _values.size(); ++i)
            {
                number v = args[i];
                _values[i] = MIN_CLAMP(v, 0, _value_max);
                 if (_is_int_only)
                     _values[i] = round(double(_values[i]));
            }
            
            return { };
        }}
    };
};


MIN_EXTERNAL(lx_dimmer);
