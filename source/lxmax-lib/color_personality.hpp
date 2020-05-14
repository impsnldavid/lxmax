/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <optional>
#include <Poco/RegularExpression.h>
#include "color_component.hpp"

namespace lxmax
{
	struct color_personality_element
	{
		static Poco::RegularExpression pattern;
		
		color_component component;
		value_precision precision;
		std::optional<int64_t> data;

		color_personality_element() = default;

		color_personality_element(color_component component, value_precision precision = value_precision::_8bit, int64_t data = 0);

		static bool from_string(const std::string& string_value, color_personality_element& value);

		std::string to_string() const;
	};

	using color_personality = std::vector<color_personality_element>;
	
	std::string to_string(const color_personality& value);

	bool from_string(const std::string& string_value, color_personality& value);

	namespace color_personality_presets
	{
		const color_personality k_rgb = { { color_component::red }, { color_component::green }, { color_component::blue } };
	}
}
