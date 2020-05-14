/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <cassert>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <Poco/Logger.h>

#include "common.hpp"
#include "dmx_buffer_manager.hpp"
#include "fixture_patch_info.hpp"

namespace lxmax
{
	class fixture;

	struct fixture_info
	{
		explicit fixture_info(fixture_patch_info info)
			: patch_info(std::move(info))
		{
		}
		
		fixture_patch_info patch_info;
		std::vector<fixture*> overlaps;
	};

	using fixture_map = std::unordered_map<fixture*, fixture_info>;
	using fixture_map_entry = std::pair<fixture*, fixture_info>;
	

	struct fixture_last_updated_compare
	{
		bool operator()(const fixture_map_entry& lhs, const fixture_map_entry& rhs) const;
	};
	
	class fixture_manager
	{
		Poco::Logger& _log;

		std::shared_ptr<dmx_buffer_manager> _buffer_manager;
		
		std::mutex _mutex;
		fixture_map _fixtures;

	public:
		fixture_manager(Poco::Logger& log, std::shared_ptr<dmx_buffer_manager> buffer_manager)
			:_log(log),
			_buffer_manager(std::move(buffer_manager))
		{
			
		}
		
		void register_fixture(fixture* fixture, const fixture_patch_info& patch_info);

		void unregister_fixture(fixture* fixture);

		universe_updated_list write_to_buffer(bool is_force = false);

	private:
		void update_fixture_overlaps();
	};
}
