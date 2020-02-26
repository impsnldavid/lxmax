/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "color_component.hpp"

namespace lxmax
{
	struct color_personality_element
	{
		color_component component;
		value_precision precision;
	};

	using color_personality = std::vector<color_personality_element>;


}
