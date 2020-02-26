/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "common.hpp"

namespace lxmax
{
	class dmx_channel_range
	{
		channel_address _start;
		channel_address _end;

	public:
		
		dmx_channel_range(universe_address universe, int channel_count)
			: _start(universe * k_universe_length),
			_end(_start + channel_count)
		{
			
		}

		channel_address start() const
		{
			return _start;
		}

		channel_address end() const
		{
			return _end;
		}

		universe_address start_universe() const
		{
			return std::floor(_start / 512);
		}

		local_channel_address start_local() const
		{
			return _start % 512;
		}

		int channel_count() const
		{
			return _end - _start;
		}
		
		friend bool operator==(const dmx_channel_range& lhs, const dmx_channel_range& rhs)
		{
			return lhs._start == rhs._start
				&& lhs._end == rhs._end;
		}

		friend bool operator!=(const dmx_channel_range& lhs, const dmx_channel_range& rhs)
		{
			return !(lhs == rhs);
		}

		bool is_overlapping_with(const dmx_channel_range& other) const
		{
			return _start <= other._end && other._start <= _end;
		}
	};
}
