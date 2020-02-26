/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cstdint>
#include <tuple>
#include <array>
#include <map>
#include <chrono>
#include <unordered_set>

namespace lxmax
{
	const int k_universe_length = 512;
	const int k_universe_min = 1;
	const int k_universe_max = 65535;
	const int k_dmx_framerate_max = 44;

	const uint32_t k_dmx_8bit_max = UINT8_MAX;
	const uint32_t k_dmx_16bit_max = UINT16_MAX;
	const uint32_t k_dmx_24bit_max = 16777215;
	const uint32_t k_dmx_32bit_max = UINT_MAX;

	using dmx_value = uint8_t;
	using universe_address = int;
	using channel_address = int;
	using local_channel_address = int;

	using universe_buffer = std::array<dmx_value, k_universe_length>;
	using universe_buffer_map = std::map<universe_address, universe_buffer>;
	using universe_buffer_map_entry = std::pair<universe_address, universe_buffer>;
	using universe_updated_set = std::unordered_set<universe_address>;

	using clock = std::chrono::high_resolution_clock;
	using timestamp = clock::time_point;
	using std::chrono::milliseconds;
}
