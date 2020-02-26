/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
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
		std::atomic<bool> _is_updated { true };
		timestamp _last_updated { timestamp::min() };

	protected:
		bool is_updated() const { return _is_updated; }

	public:
		fixture() = default;

		fixture(const fixture& other) = delete;
		fixture(fixture&& other) noexcept = delete;
		fixture& operator=(const fixture& other) = delete;
		fixture& operator=(fixture&& other) noexcept = delete;

		virtual ~fixture();

		void set_manager(std::shared_ptr<fixture_manager> manager, const fixture_patch_info& patch_info);

		virtual bool write_to_buffer(const fixture_patch_info& patch_info, universe_buffer_map& buffer_map, universe_updated_set& updated_universes, bool is_force) = 0;

		void set_updated();

		void clear_updated()
		{
			_is_updated = false;
		}

		timestamp last_updated() const
		{
			return _last_updated;
		}
	};
}
