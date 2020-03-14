/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "fixture_manager.hpp"

#include <set>

#include "fixture.hpp"

namespace lxmax
{
	bool fixture_last_updated_compare::operator()(const fixture_map_entry& lhs, const fixture_map_entry& rhs) const
	{
		return lhs.first->last_updated() < rhs.first->last_updated();
	}

	void fixture_manager::register_fixture(fixture* fixture, const fixture_patch_info& patch_info)
	{
		std::lock_guard<std::mutex> lock(_mutex);

		_fixtures.insert(std::make_pair(fixture, fixture_info(patch_info)));

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

		std::set<fixture_map_entry, fixture_last_updated_compare> sorted_fixtures(_fixtures.begin(), _fixtures.end());

		std::vector<fixture*> fixtures_written_to_buffer;
		
		for (const fixture_map_entry& entry : sorted_fixtures)
		{
			bool can_write = true;

			if (!entry.second.patch_info.is_htp)
			{
				for (fixture* overlapping_fixture : entry.second.overlaps)
				{
					auto it = std::find(fixtures_written_to_buffer.begin(), 
						fixtures_written_to_buffer.end(), overlapping_fixture);
					if (it != std::end(fixtures_written_to_buffer))
					{
						can_write = false;
						break;
					}
				}
			}

			if (!can_write)
				break;
			
			const bool did_write = entry.first->write_to_buffer(entry.second.patch_info, buffers, updated_universes, is_force);
			if (did_write)
				fixtures_written_to_buffer.push_back(entry.first);
		}
	}

	void fixture_manager::update_fixture_overlaps()
	{
		// TODO: Create more efficient versions of this for registering and unregistering fixtures
		
		for (auto& entry : _fixtures)
		{
			entry.second.overlaps.clear();
			
			for (const auto& other_entry : _fixtures)
			{
				if (entry.first == other_entry.first)
					continue;

				if (entry.second.patch_info.channel_range.is_overlapping_with(other_entry.second.patch_info.channel_range))
					entry.second.overlaps.push_back(other_entry.first);
			}
		}
	}
}
