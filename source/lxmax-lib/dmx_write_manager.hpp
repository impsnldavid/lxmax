/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "common.hpp"

namespace lxmax
{
	/// @brief Manages writing DMX to universe buffers
	/// 
	class dmx_write_manager
	{
		universe_buffer_map _universe_buffers;
		universe_updated_list _universe_updated;

	public:
		universe_buffer_map& get_data()
		{
			return _universe_buffers;
		}
	};
}