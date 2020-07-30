/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>
#include <array>
#include <map>
#include <chrono>
#include <unordered_set>

namespace lxmax
{
	const int k_universe_length = 512;
	const int k_universe_min = 1;
	const int k_universe_max = 63999;
	const int k_universe_artnet_min = 0;
	const int k_universe_artnet_max = 32767;
	const int k_universe_sacn_min = 1;
	const int k_universe_sacn_max = 63999;
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
	using universe_updated_list = std::vector<universe_address>;

	using clock = std::chrono::high_resolution_clock;
	using timestamp = clock::time_point;
	using std::chrono::milliseconds;

	enum class dmx_protocol
	{
		none,
		artnet,
		sacn
	};

	inline std::string dmx_protocol_to_string(dmx_protocol value)
	{
		switch(value)
		{
			default:
			case dmx_protocol::none:
				return "None";

			case dmx_protocol::artnet:
				return "Art-Net";

			case dmx_protocol::sacn:
				return "sACN";
		}
	}

	inline dmx_protocol dmx_protocol_from_string(const std::string& value)
	{
		if (value == "Art-Net")
			return dmx_protocol::artnet;
		else if (value == "sACN")
			return dmx_protocol::sacn;
		else
			return dmx_protocol::none;
	}

	enum class dmx_universe_type
	{
		none,
		input,
		output
	};

	inline std::string dmx_universe_type_to_string(dmx_universe_type value)
	{
		switch(value)
		{
			default:
			case dmx_universe_type::none:
				return "None";

			case dmx_universe_type::input:
				return "Input";

			case dmx_universe_type::output:
				return "Output";
		}
	}

	inline dmx_universe_type dmx_universe_type_from_string(const std::string& value)
	{
		if (value == "Input")
			return dmx_universe_type::input;
		else if (value == "Output")
			return dmx_universe_type::output;
		else
			return dmx_universe_type::none;
	}

}
