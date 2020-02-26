/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cmath>
#include <vector>
#include <mutex>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/MulticastSocket.h>
#include <Poco/Timer.h>
#include <Poco/UUIDGenerator.h>

#include "common.hpp"
#include "dmx_packet_artnet.hpp"
#include "dmx_packet_sacn.hpp"



namespace lxmax
{
	enum class dmx_protocol
	{
		none,
		artnet,
		sacn
	};

	struct dmx_output_universe_config
	{
		dmx_protocol protocol;

		universe_address internal_universe;
		universe_address protocol_universe;
		universe_address sync_address;
		int priority;

		Poco::Net::IPAddress send_address;
	};

    class dmx_output_service
    {
		const milliseconds k_full_update_interval { 1000 };

    	bool _isRunning { false };
		Poco::Timer _timer { 0, static_cast<int>(std::round(1000. / k_dmx_framerate_max)) };

		Poco::Net::DatagramSocket _socket;
		Poco::Net::MulticastSocket _multicast_socket;

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
		dmx_output_service()
			: _system_name(Poco::Environment::nodeName()),
			_system_id(Poco::UUIDGenerator::defaultGenerator().createFromName(Poco::UUID(), _system_name))
		{
			Poco::Net::initializeNetwork();

			_socket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress("0.0.0.0"), 0));
			_socket.setBroadcast(true);
			_multicast_socket.bind(Poco::Net::SocketAddress(Poco::Net::IPAddress("0.0.0.0"), 0));
			_multicast_socket.setLoopback(true);

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

    	void update_configs(const std::vector<dmx_output_universe_config>& configs)
		{
			std::lock_guard<std::mutex> lock(_config_mutex);
			_configs = configs;
		}

    private:
    	void on_timer(Poco::Timer& timer);
    };
}
