/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "common.hpp"

namespace lxmax
{
	enum class value_precision
	{
	    _8bit,
	    _16bit,
	    _24bit,
	    _32bit,
	    _16bit_le,
	    _24bit_le,
	    _32bit_le,
	    enum_count
	};

	namespace precision_helper
    {
        const std::unordered_map<value_precision, std::string> k_precision_to_string_map
        {
            { value_precision::_8bit , "8" },
            { value_precision::_16bit , "16" },
            { value_precision::_24bit , "24" },
            { value_precision::_32bit , "32" },
            { value_precision::_16bit_le , "16!" },
            { value_precision::_24bit_le , "24!" },
            { value_precision::_32bit_le , "32!" },
		};
    
        const std::unordered_map<std::string, value_precision> k_string_to_precision_map
        {
            { "8", value_precision::_8bit },
            { "16", value_precision::_16bit },
            { "24", value_precision::_24bit },
            { "32", value_precision::_32bit },
            { "16!", value_precision::_16bit_le },
            { "24!", value_precision::_24bit_le },
            { "32!", value_precision::_32bit_le },
        };

        inline bool from_string(const std::string& stringValue, value_precision& value)
        {
	        const auto it = k_string_to_precision_map.find(stringValue);
            if (it == std::end(k_string_to_precision_map))
            {
                value = value_precision::_8bit;
                return false;
            }
            
            value = it->second;
            return true;
        }

        inline std::string to_string(value_precision value)
        {
	        const auto it = k_precision_to_string_map.find(value);
            assert(it != std::end(k_precision_to_string_map),
                            "All valid value precisions should exist in map");
            
            return it->second;
        }
    }

	inline void write_with_precision_ltp(double value, double max, dmx_value* data, value_precision precision)
	{
		const double norm_value = value / max;

		uint32_t ranged_value = 0;
		
		switch(precision)
		{
			case value_precision::_8bit:
				*data = (dmx_value)std::round(norm_value * k_dmx_8bit_max);
				break;
			case value_precision::_16bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_16bit_max);
				*data++ =  ranged_value >> 8;
				*data =  ranged_value & 0x00FF;
				break;
			case value_precision::_24bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_24bit_max);
				*data++ =  ranged_value >> 16;
				*data++ =  (ranged_value & 0x00FF00) >> 8;
				*data =  ranged_value & 0x0000FF;
				break;
			case value_precision::_32bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_32bit_max);
				*data++ =  ranged_value >> 24;
				*data++ =  (ranged_value & 0x00FF0000) >> 16;
				*data++ =  (ranged_value & 0x0000FF00) >> 8;
				*data =  ranged_value & 0x000000FF;
				break;
			case value_precision::_16bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_16bit_max);
				*data++ =  ranged_value & 0x00FF;
				*data =  ranged_value >> 8;
				break;
			case value_precision::_24bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_24bit_max);
				*data =  ranged_value & 0x0000FF;
				*data++ =  (ranged_value & 0x00FF00) >> 8;
				*data++ =  ranged_value >> 16;
				break;
			case value_precision::_32bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_32bit_max);
				*data++ =  ranged_value & 0x000000FF;
				*data++ =  (ranged_value & 0x0000FF00) >> 8;
				*data++ =  (ranged_value & 0x00FF0000) >> 16;
				*data =  ranged_value >> 24;
				break;
			default:
				break;
		}
	}

	inline void write_with_precision_htp(double value, double max, dmx_value* data, value_precision precision)
	{
		const double norm_value = value / max;

		uint32_t ranged_value = 0;
		
		switch(precision)
		{
			case value_precision::_8bit:
				*data = std::max(*data, (dmx_value)std::round(norm_value * k_dmx_8bit_max));
				break;
			case value_precision::_16bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_16bit_max);
				*data++ = std::max(*data, (dmx_value)(ranged_value >> 8));
				*data = std::max(*data, (dmx_value)(ranged_value & 0x00FF));
				break;
			case value_precision::_24bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_24bit_max);
				*data++ = std::max(*data, (dmx_value)(ranged_value >> 16));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x00FF00) >> 8));
				*data = std::max(*data, (dmx_value)(ranged_value & 0x0000FF));
				break;
			case value_precision::_32bit:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_32bit_max);
				*data++ = std::max(*data, (dmx_value)(ranged_value >> 24));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x00FF0000) >> 16));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x0000FF00) >> 8));
				*data = std::max(*data, (dmx_value)(ranged_value & 0x000000FF));
				break;
			case value_precision::_16bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_16bit_max);
				*data++ = std::max(*data, (dmx_value)(ranged_value & 0x00FF));
				*data = std::max(*data, (dmx_value)(ranged_value >> 8));
				break;
			case value_precision::_24bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_24bit_max);
				*data = std::max(*data, (dmx_value)(ranged_value & 0x0000FF));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x00FF00) >> 8));
				*data++ = std::max(*data, (dmx_value)(ranged_value >> 16));
				break;
			case value_precision::_32bit_le:
				ranged_value = (uint32_t)std::round(norm_value * k_dmx_32bit_max);
				*data++ = std::max(*data, (dmx_value)(ranged_value & 0x000000FF));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x0000FF00) >> 8));
				*data++ = std::max(*data, (dmx_value)((ranged_value & 0x00FF0000) >> 16));
				*data = std::max(*data, (dmx_value)(ranged_value >> 24));
				break;
			default:
				break;
		}
	}
}
