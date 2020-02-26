/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "fixture_manager.hpp"
#include "fixture.hpp"

namespace lxmax
{
	void fixture_manager::register_fixture(fixture* fixture, const fixture_patch_info& patch_info)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		_fixtures.insert(std::make_pair(fixture, patch_info));

		update_fixture_overlaps();
	}

	void fixture_manager::unregister_fixture(fixture* fixture)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		assert(_fixtures.erase(fixture));

		update_fixture_overlaps();
	}

	void fixture_manager::write_to_buffer(universe_buffer_map& buffers, bool is_force)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		universe_updated_set updated_universes;

		std::vector<fixture_map_entry> sorted_fixtures;
		sorted_fixtures.reserve(_fixtures.size());
		std::copy(_fixtures.begin(), _fixtures.end(), std::back_inserter(sorted_fixtures));

		std::sort(sorted_fixtures.begin(), sorted_fixtures.end(), 
			[](const fixture_map_entry& x, const fixture_map_entry& y){ return x.first->last_updated() > y.first->last_updated(); });

		std::unordered_set<fixture*> fixtures_written_to_buffer;
		
		for (fixture_map_entry entry : sorted_fixtures)
		{
			auto it = _fixture_overlaps.find(entry.first);
			assert(it != std::end(_fixture_overlaps));

			bool can_write = true;

			if (!entry.second.is_htp)
			{
				for (fixture* overlapping_fixture : it->second)
				{
					if (fixtures_written_to_buffer.find(overlapping_fixture) != std::end(fixtures_written_to_buffer))
					{
						can_write = false;
						break;
					}
				}
			}

			if (!can_write)
				break;
			
			const bool did_write = entry.first->write_to_buffer(entry.second, buffers, updated_universes, is_force);
			if (did_write)
				fixtures_written_to_buffer.insert(entry.first);
		}
	}

	void fixture_manager::update_fixture_overlaps()
	{
		// TODO: Create more efficient versions of this for registering and unregistering fixtures
		
		_fixture_overlaps.clear();
		
		for (const auto& entry : _fixtures)
		{
			std::unordered_set<fixture*> overlapping_fixtures;
			
			for (const auto& other_entry : _fixtures)
			{
				if (entry.first == other_entry.first)
					continue;

				if (entry.second.channel_range.is_overlapping_with(other_entry.second.channel_range))
					overlapping_fixtures.insert(other_entry.first);
			}

			_fixture_overlaps.insert(std::make_pair(entry.first, overlapping_fixtures));
		}
	}
}
