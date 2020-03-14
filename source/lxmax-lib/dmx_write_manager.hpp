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
	};
}