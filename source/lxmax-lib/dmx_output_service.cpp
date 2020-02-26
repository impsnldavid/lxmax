/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "dmx_output_service.hpp"

namespace lxmax
{
	void dmx_output_service::on_timer(Poco::Timer& timer)
	{
		// TODO: Calculate full update time per universe, and implement separate update behaviours for art-net and sacn

		bool is_full_update = false;

		const auto time_now = clock::now();
		if (time_now - _last_full_update_time > k_full_update_interval)
		{
			is_full_update = true;
			_last_full_update_time = time_now;
		}

		std::lock_guard<std::mutex> lock(_config_mutex);

		if (!_configs.empty())
		{
			_sacn_sequence = _sacn_sequence >= 255 ? 0 : _sacn_sequence + 1;
		}

		bool is_artnet_sequence_increased = false;
		bool is_sacn_sequence_increased = false;
		
		for (const auto& config : _configs)
		{
			if (!is_full_update)
			{
				if (_updated_universes.find(config.internal_universe) == std::end(_updated_universes))
					continue;
			}

			const auto it = _universe_buffers.find(config.internal_universe);

			if (it == std::end(_universe_buffers))
				continue;

			std::vector<char> buffer;

			switch (config.protocol)
			{
			case dmx_protocol::artnet:
				{
					if (!is_artnet_sequence_increased)
					{
						_artnet_sequence = _artnet_sequence >= 255 ? 0 : _artnet_sequence + 1;
						is_artnet_sequence_increased = true;
					}
					
					dmx_packet_artnet packet(config.protocol_universe, _artnet_sequence, it->second);
					const auto packet_buffer = packet.serialize();

					Poco::Net::SocketAddress address{config.send_address, k_artnet_port};
					_socket.sendTo(packet_buffer.data(), packet_buffer.size(), address);
				}
				break;

			case dmx_protocol::sacn:
				{
					if (!is_sacn_sequence_increased)
					{
						_sacn_sequence = _sacn_sequence >= 255 ? 0 : _sacn_sequence + 1;
						is_sacn_sequence_increased = true;
					}
					
					dmx_packet_sacn packet(_system_id, _system_name, config.priority, config.sync_address,
					                       config.protocol_universe, _sacn_sequence, it->second);
					const auto packet_buffer = packet.serialize();

					Poco::Net::SocketAddress address{get_sacn_multicast_address(config.protocol_universe), k_sacn_port};
					_multicast_socket.sendTo(packet_buffer.data(), packet_buffer.size(), address);
				}
				break;

			default:
				break;
			}
		}
	}
}
