/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <cassert>
#include <unordered_map>
#include <mutex>
#include <vector>
#include "common.hpp"
#include "fixture_patch_info.hpp"

namespace lxmax
{
	class fixture;

	using fixture_map = std::unordered_map<fixture*, fixture_patch_info>;
	using fixture_map_entry = std::pair<fixture*, fixture_patch_info>;
	
	class fixture_manager
	{
		std::mutex _mutex;
		fixture_map _fixtures;

		std::unordered_map<fixture*, std::unordered_set<fixture*>> _fixture_overlaps;

	public:
		void register_fixture(fixture* fixture, const fixture_patch_info& patch_info);

		void unregister_fixture(fixture* fixture);

		void write_to_buffer(universe_buffer_map& buffers, bool is_force = false);

	private:
		void update_fixture_overlaps();
	};
}
