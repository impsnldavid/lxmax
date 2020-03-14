/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <string>
#include "common.hpp"
#include "dmx_channel_range.hpp"

namespace lxmax
{
	struct fixture_patch_info
	{
		std::string label;
		bool is_htp;
		dmx_channel_range channel_range;
	};
}
