/// @file
/// @ingroup     lxmax
/// @copyright    Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include <string>
#include <unordered_map>
#include <Poco/Platform.h>

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
            { ColorComponent::none , "_" },
            { ColorComponent::red , "R" },
            { ColorComponent::green , "G" },
            { ColorComponent::blue , "B" },
            { ColorComponent::cyan , "C" },
            { ColorComponent::magenta , "M" },
            { ColorComponent::yellow , "Y" },
            { ColorComponent::hue , "H" },
            { ColorComponent::saturation , "S" },
            { ColorComponent::brightness , "Br" },
            { ColorComponent::lightness , "L" },
            { ColorComponent::value , "V" },
            { ColorComponent::white , "W" },
            { ColorComponent::amber , "A" },
            { ColorComponent::intensity , "I" },
            { ColorComponent::dimmer , "D" }
        };
    
        const std::unordered_map<std::string, color_component> k_string_to_color_map
        {
            { "_", ColorComponent::none },
            { "R", ColorComponent::red },
            { "G", ColorComponent::green },
            { "B", ColorComponent::blue },
            { "C", ColorComponent::cyan },
            { "M", ColorComponent::magenta },
            { "Y", ColorComponent::yellow },
            { "H", ColorComponent::hue },
            { "S", ColorComponent::saturation },
            { "Br", ColorComponent::brightness },
            { "L", ColorComponent::lightness },
            { "V", ColorComponent::value },
            { "W", ColorComponent::white },
            { "A", ColorComponent::amber },
            { "I", ColorComponent::intensity },
            { "D", ColorComponent::dimmer }
        };
    
        bool from_string(const std::string& stringValue, color_component& value)
        {
            auto it = k_string_to_color_map.find(stringValue);
            if (it == k_string_to_color_map.end())
            {
                value = color_component::none;
                return false;
            }
            
            value = it->second;
            return true;
        }
        
        std::string to_string(color_component value)
        {
            auto it = k_color_to_string_map.find(value);
            poco_assert_msg(it != k_color_to_string_map.end(),
                            "All valid color components should exist in map");
            
            return it->second;
        }
    };
}
