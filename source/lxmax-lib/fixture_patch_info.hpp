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

		fixture_patch_info() = default;
		
		fixture_patch_info(std::string label, bool is_htp, dmx_channel_range channel_range)
			: label(label),
			is_htp(is_htp),
			channel_range(channel_range)
		{
			
		}

		friend bool operator==(const fixture_patch_info& lhs, const fixture_patch_info& rhs)
		{
			return lhs.label == rhs.label
				&& lhs.is_htp == rhs.is_htp
				&& lhs.channel_range == rhs.channel_range;
		}

		friend bool operator!=(const fixture_patch_info& lhs, const fixture_patch_info& rhs)
		{
			return !(lhs == rhs);
		}
	};
}
