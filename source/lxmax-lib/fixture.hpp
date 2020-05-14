/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <atomic>
#include "common.hpp"
#include "fixture_patch_info.hpp"

namespace lxmax
{
	class fixture_manager;
	
	class fixture
	{
		std::shared_ptr<fixture_manager> _manager;
		fixture_patch_info _patch_info;
		std::atomic<bool> _is_updated { true };
		timestamp _last_updated { timestamp::min() };

	protected:
		bool is_updated() const { return _is_updated; }

		void set_patch_info(fixture_patch_info info);

	public:
		fixture() = default;

		fixture(const fixture& other) = delete;
		fixture(fixture&& other) noexcept = delete;
		fixture& operator=(const fixture& other) = delete;
		fixture& operator=(fixture&& other) noexcept = delete;

		virtual ~fixture();

		void set_manager(std::shared_ptr<fixture_manager> manager);

		virtual bool write_to_buffer(const fixture_patch_info& patch_info, universe_buffer_map& buffer_map, universe_updated_list& updated_universes, bool is_force) = 0;

		void set_updated();

		void clear_updated()
		{
			_is_updated = false;
		}

		timestamp last_updated() const
		{
			return _last_updated;
		}

		int channel() const
		{
			return _patch_info.channel_range.start_local();
		}

		int universe() const
		{
			return _patch_info.channel_range.start_universe();
		}

		bool is_htp() const
		{
			return _patch_info.is_htp;
		}
	};
}
