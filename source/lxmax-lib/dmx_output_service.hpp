/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cmath>
#include <utility>
#include <vector>
#include <mutex>
#include <memory>
#include <Poco/Logger.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Timer.h>
#include <Poco/UUIDGenerator.h>

#include "common.hpp"
#include "hash_functions.hpp"
#include "dmx_packet_artnet.hpp"
#include "dmx_packet_sacn.hpp"
#include "dmx_universe_config.hpp"
#include "global_config.hpp"
#include "preferences_manager.hpp"


namespace lxmax
{
	class dmx_output_service
	{
		const milliseconds k_full_update_interval{1000};

		Poco::Logger& _log;
		
		global_config _global_config;

		bool _isRunning{false};
		Poco::Timer _timer;

		std::unique_ptr<Poco::Net::DatagramSocket> _artnet_socket;
		std::unique_ptr<Poco::Net::MulticastSocket> _sacn_socket;

		Poco::Net::IPAddress _artnet_broadcast_address;

		universe_buffer_map _universe_buffers;
		universe_updated_set _updated_universes;

		std::mutex _config_mutex;
		std::vector<dmx_output_universe_config> _universe_configs;

		const std::string _system_name;
		const Poco::UUID _system_id;

		uint8_t _artnet_sequence = 1;
		uint8_t _sacn_sequence = 0;
		uint8_t _sacn_sync_sequence = 0;

		timestamp _last_full_update_time;


	public:
		dmx_output_service(Poco::Logger& log)
			: _log(log),
			  _system_name(Poco::Environment::nodeName()),
			  _system_id(Poco::UUIDGenerator::defaultGenerator().createFromName(Poco::UUID(), _system_name))
		{
			_universe_buffers.insert(universe_buffer_map_entry(universe_address(1), universe_buffer()));
		}

		void start()
		{
			_timer.start(Poco::TimerCallback<dmx_output_service>(*this, &dmx_output_service::on_timer));
			_isRunning = true;
		}

		void stop()
		{
			_timer.stop();
			_isRunning = false;
		}

		void update_global_config(const void* pSender);

		void update_universe_configs(const void* pSender);

	private:
		void on_timer(Poco::Timer& timer);
	};
}
