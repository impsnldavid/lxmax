/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "dmx_output_service.hpp"

namespace lxmax
{
	void dmx_output_service::on_timer(Poco::Timer& timer)
	{
		// TODO: Calculate full update time per universe
		
		bool is_full_update = false;

		const auto time_now = clock::now();
		if (_global_config.is_force_output_at_framerate || time_now - _last_full_update_time > k_full_update_interval)
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
					if (!_artnet_socket)
						continue;
					
					if (!is_artnet_sequence_increased)
					{
						_artnet_sequence = _artnet_sequence >= 255 ? 1 : _artnet_sequence + 1;
						is_artnet_sequence_increased = true;
					}
					
					dmx_packet_artnet packet(config.protocol_universe, _artnet_sequence, it->second);
					const auto packet_buffer = packet.serialize();

					if (config.is_use_global_destination)
					{
						if (_global_config.is_artnet_global_destination_broadcast)
						{
							Poco::Net::SocketAddress address { k_artnet_broadcast_address, k_artnet_port };
		                    _artnet_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
						}
						else
						{
							for(const auto& a : _global_config.artnet_global_destination_unicast_addresses)
		                    {
		                        Poco::Net::SocketAddress address { a, k_artnet_port };
		                        _artnet_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
		                    }
						}
					}
					else
					{
						if (config.is_broadcast_or_multicast)
						{
							Poco::Net::SocketAddress address { k_artnet_broadcast_address, k_artnet_port };
		                    _artnet_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
						}
						else
						{
							for(const auto& a : config.unicast_addresses)
		                    {
		                        Poco::Net::SocketAddress address { a, k_artnet_port };
		                        _artnet_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
		                    }
						}
					}
				}
				break;

			case dmx_protocol::sacn:
				{
					if (!_sacn_socket)
						continue;
					
					if (!is_sacn_sequence_increased)
					{
						_sacn_sequence = _sacn_sequence >= 255 ? 0 : _sacn_sequence + 1;
						is_sacn_sequence_increased = true;
					}
					
					dmx_packet_sacn packet(_system_id, _system_name, config.priority, config.sync_address,
					                       config.protocol_universe, _sacn_sequence, it->second);
					const auto packet_buffer = packet.serialize();

					if (config.is_use_global_destination)
					{
						if (_global_config.is_sacn_global_destination_multicast)
						{
							Poco::Net::SocketAddress address { get_sacn_multicast_address(config.protocol_universe), k_sacn_port };
		                    _sacn_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
						}
						else
						{
							for(const auto& a : _global_config.sacn_global_destination_unicast_addresses)
		                    {
		                        Poco::Net::SocketAddress address { a, k_sacn_port };
		                        _sacn_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
		                    }
						}
					}
					else
					{
						if (config.is_broadcast_or_multicast)
						{
							Poco::Net::SocketAddress address { get_sacn_multicast_address(config.protocol_universe), k_sacn_port };
		                    _sacn_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
						}
						else
						{
							for(const auto& a : config.unicast_addresses)
		                    {
		                        Poco::Net::SocketAddress address { a, k_sacn_port };
		                        _sacn_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
		                    }
						}
					}
				}
				break;

			default:
				break;
			}
		}
	}
}
