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


namespace lxmax
{
	class dmx_output_service
	{
		const milliseconds k_full_update_interval{1000};

		const Poco::Net::IPAddress k_artnet_broadcast_address{"2.255.255.255"};
		const Poco::Net::IPAddress k_artnet_alt_broadcast_address{"10.255.255.255"};

		Poco::Logger& _log;
		
		global_config _global_config;

		bool _isRunning{false};
		Poco::Timer _timer;

		std::unique_ptr<Poco::Net::DatagramSocket> _artnet_socket;
		std::unique_ptr<Poco::Net::MulticastSocket> _sacn_socket;

		universe_buffer_map _universe_buffers;
		universe_updated_set _updated_universes;

		std::mutex _config_mutex;
		std::vector<dmx_output_universe_config> _configs;

		const std::string _system_name;
		const Poco::UUID _system_id;

		uint8_t _artnet_sequence = 1;
		uint8_t _sacn_sequence = 0;

		timestamp _last_full_update_time;


	public:
		dmx_output_service(Poco::Logger& log)
			: _log(log),
			  _system_name(Poco::Environment::nodeName()),
			  _system_id(Poco::UUIDGenerator::defaultGenerator().createFromName(Poco::UUID(), _system_name))
		{
			Poco::Net::initializeNetwork();

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

		void update_global_config(const global_config& config)
		{
			std::lock_guard<std::mutex> lock(_config_mutex);
			
			_global_config = config;

			if (_sacn_socket)
				_sacn_socket.reset();

			if (_artnet_socket)
				_artnet_socket.reset();

			const int framerate = std::max(1, _global_config.is_allow_nondmx_framerate
				                 ? _global_config.framerate
							: std::min(_global_config.framerate,  k_dmx_framerate_max));

			_timer.setPeriodicInterval(static_cast<long>(std::round(1000. / framerate)));

			

			Poco::Net::NetworkInterface artnet_nic;
			Poco::Net::NetworkInterface sacn_nic;

			if (!_global_config.artnet_network_adapter.isWildcard())
			{
				try
				{
					artnet_nic = Poco::Net::NetworkInterface::forAddress(_global_config.artnet_network_adapter);
				}
				catch (const Poco::NotFoundException& ex)
				{
					poco_warning(_log, "Failed to find network adapter with IP '%s' for Art-Net. Please select a new network adapter in LXMax preferences.", 
						_global_config.artnet_network_adapter.toString());
				}
			}

			_artnet_socket = std::make_unique<Poco::Net::DatagramSocket>();
			_artnet_socket->bind(Poco::Net::SocketAddress(artnet_nic.address(), 0), true, true);
			_artnet_socket->setBroadcast(true);

			if (!_global_config.sacn_network_adapter.isWildcard())
			{
				try
				{
					sacn_nic = Poco::Net::NetworkInterface::forAddress(_global_config.sacn_network_adapter);
				}
				catch (const Poco::NotFoundException& ex)
				{
					poco_warning(_log, "Failed to find network adapter with IP '%s' for sACN. Please select a new network adapter in LXMax preferences.", 
						_global_config.sacn_network_adapter.toString());
				}
			}

			_sacn_socket = std::make_unique<Poco::Net::MulticastSocket>();
			_sacn_socket->bind(Poco::Net::SocketAddress(sacn_nic.address(), 0), true, true);
			_sacn_socket->setInterface(sacn_nic);
			_sacn_socket->setLoopback(true);
		}
		
		void update_universe_configs(const std::vector<dmx_output_universe_config>& configs)
		{
			std::lock_guard<std::mutex> lock(_config_mutex);

			_configs = configs;
		}

	private:
		void on_timer(Poco::Timer& timer);
	};
}
