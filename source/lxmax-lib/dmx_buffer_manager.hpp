/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <vector>
#include <mutex>
#include <optional>
#include "common.hpp"
#include "preferences_manager.hpp"

namespace lxmax
{
	/// @brief Manages DMX universe buffers
	/// 
	class dmx_buffer_manager
	{
		Poco::Logger& _log;

		std::mutex _config_mutex;
		
		universe_buffer_map _universe_buffers;
		universe_updated_list _universe_updated;

	public:
		dmx_buffer_manager(Poco::Logger& log)
			: _log(log)
		{
			
		}

		std::tuple<std::unique_lock<std::mutex>, universe_buffer_map&> get_universe_buffers()
		{
			return { std::unique_lock<std::mutex>(_config_mutex), _universe_buffers };
		}

		void update_universe_configs(const void* pSender)
		{
			create_buffers(reinterpret_cast<const preferences_manager*>(pSender)->get_universe_configs());
		}

		std::optional<universe_buffer> get_universe_buffer(universe_address address)
		{
			std::lock_guard<std::mutex> lock(_config_mutex);
			
			const auto it = _universe_buffers.find(address);
			if (it == std::end(_universe_buffers))
				return std::nullopt;
			
			return it->second;
		}

	private:
		void create_buffers(const dmx_universe_configs& universe_configs)
		{
			std::lock_guard<std::mutex> lock(_config_mutex);
			
			_universe_buffers.clear();
			
			for (const auto& u : universe_configs)
			{
				if (u.second->is_enabled && _universe_buffers.find(u.second->internal_universe) == std::end(_universe_buffers))
					_universe_buffers.insert(std::make_pair(u.second->internal_universe, universe_buffer()));
			}
		}
	};
}
