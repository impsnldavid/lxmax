/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <string>
#include <unordered_map>
#include <cassert>

#include "precision_helpers.hpp"

namespace lxmax
{
    enum class color_component
    {
        none,
        red,
        blue,
        green,
        cyan,
        magenta,
        yellow,
        hue,
        saturation,
        brightness,
        lightness,
        value,
        white,
        amber,
        intensity,
        dimmer
    };

    namespace color_component_helper
    {
        const std::unordered_map<color_component, std::string> k_color_to_string_map
        {
            { color_component::none , "_" },
            { color_component::red , "R" },
            { color_component::green , "G" },
            { color_component::blue , "B" },
            { color_component::cyan , "C" },
            { color_component::magenta , "M" },
            { color_component::yellow , "Y" },
            { color_component::hue , "H" },
            { color_component::saturation , "S" },
            { color_component::brightness , "Br" },
            { color_component::lightness , "L" },
            { color_component::value , "V" },
            { color_component::white , "W" },
            { color_component::amber , "A" },
            { color_component::intensity , "I" },
            { color_component::dimmer , "D" }
        };
    
        const std::unordered_map<std::string, color_component> k_string_to_color_map
        {
            { "_", color_component::none },
            { "R", color_component::red },
            { "G", color_component::green },
            { "B", color_component::blue },
            { "C", color_component::cyan },
            { "M", color_component::magenta },
            { "Y", color_component::yellow },
            { "H", color_component::hue },
            { "S", color_component::saturation },
            { "Br", color_component::brightness },
            { "L", color_component::lightness },
            { "V", color_component::value },
            { "W", color_component::white },
            { "A", color_component::amber },
            { "I", color_component::intensity },
            { "D", color_component::dimmer }
        };

        inline bool from_string(const std::string& stringValue, color_component& value)
        {
	        const auto it = k_string_to_color_map.find(stringValue);
            if (it == k_string_to_color_map.end())
            {
                value = color_component::none;
                return false;
            }
            
            value = it->second;
            return true;
        }

        inline std::string to_string(color_component value)
        {
	        const auto it = k_color_to_string_map.find(value);
            assert(it != k_color_to_string_map.end(),
                            "All valid color components should exist in map");
            
            return it->second;
        }
    };
}
