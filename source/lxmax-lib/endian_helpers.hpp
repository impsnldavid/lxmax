/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <cstdint>

namespace lxmax
{
	#pragma pack(push, 1)

	/// An explicitly big endian uint16_t
	struct uint16_t_be
	{
		uint8_t bytes[2];

		uint16_t_be() = default;

		uint16_t_be(uint16_t value)
		{
			bytes[0] = value >> 8;
			bytes[1] = value & 0x00FF;
		}

		operator uint16_t() const
		{
			return (bytes[0] << 8) + bytes[1];
		}
    };

	
	/// An explicitly big endian uint32_t
	struct uint32_t_be
	{
		uint8_t bytes[4];

		uint32_t_be() = default;

		uint32_t_be(uint32_t value)
		{
			bytes[0] = value >> 24;
			bytes[1] = (value & 0x00FF0000) >> 16;
			bytes[2] = (value & 0x0000FF00) >> 8;
			bytes[3] = value & 0x00FF;
		}

		operator uint32_t() const
		{
			return (bytes[0] << 24) + (bytes[1] << 16) +(bytes[2] << 8) + bytes[3];
		}
    };

	#pragma pack(pop)
}
