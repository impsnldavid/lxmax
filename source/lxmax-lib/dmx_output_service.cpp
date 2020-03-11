/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "dmx_output_service.hpp"

namespace lxmax
{
	void dmx_output_service::update_global_config(const void* pSender)
	{
		std::lock_guard<std::mutex> lock(_config_mutex);

		_global_config = reinterpret_cast<const preferences_manager*>(pSender)->get_global_config();

		if (_sacn_socket)
			_sacn_socket.reset();

		if (_artnet_socket)
			_artnet_socket.reset();

		const int framerate = std::max(1, _global_config.is_allow_nondmx_framerate
			                                  ? _global_config.framerate
			                                  : std::min(_global_config.framerate, k_dmx_framerate_max));

		_timer.setPeriodicInterval(static_cast<long>(std::round(1000. / framerate)));

		{
			Poco::Net::IPAddress artnet_nic_address = Poco::Net::IPAddress("0.0.0.0");
			
			if (!_global_config.artnet_network_adapter.isWildcard())
			{
				try
				{
					const Poco::Net::NetworkInterface artnet_nic = Poco::Net::NetworkInterface::forAddress(
						_global_config.artnet_network_adapter);
					
					artnet_nic.firstAddress(artnet_nic_address);

					int index = 0;
					for(const auto& t : artnet_nic.addressList())
					{
						if (t.get<0>() == artnet_nic_address)
							break;

						++index;
					}
					
					_artnet_broadcast_address = artnet_nic.broadcastAddress(index);
				}
				catch (const Poco::NotFoundException& ex)
				{
					poco_warning(
						_log,
						"Failed to find network adapter with IP '%s' for Art-Net. Please select a new network adapter in LXMax preferences.",
						_global_config.artnet_network_adapter.toString());
				}
			}
			else
			{
				_artnet_broadcast_address = Poco::Net::IPAddress("255.255.255.255");
			}

			_artnet_socket = std::make_unique<Poco::Net::DatagramSocket>();

			_artnet_socket->bind(Poco::Net::SocketAddress(artnet_nic_address, k_artnet_port), true, true);
			_artnet_socket->setBroadcast(true);
		}
		
		{
			Poco::Net::NetworkInterface sacn_nic;
			
			if (!_global_config.sacn_network_adapter.isWildcard())
			{
				try
				{
					sacn_nic = Poco::Net::NetworkInterface::forAddress(_global_config.sacn_network_adapter);
				}
				catch (const Poco::NotFoundException& ex)
				{
					poco_warning(
						_log,
						"Failed to find network adapter with IP '%s' for sACN. Please select a new network adapter in LXMax preferences.",
						_global_config.sacn_network_adapter.toString());
				}
			}

			_sacn_socket = std::make_unique<Poco::Net::MulticastSocket>();

			Poco::Net::IPAddress sacn_nic_address;
			sacn_nic.firstAddress(sacn_nic_address);

			_sacn_socket->bind(Poco::Net::SocketAddress(sacn_nic_address, 0), true, true);

			if (!sacn_nic_address.isWildcard())
				_sacn_socket->setInterface(sacn_nic);

			_sacn_socket->setLoopback(true);
		}
	}

	void dmx_output_service::update_universe_configs(const void* pSender)
	{
		std::lock_guard<std::mutex> lock(_config_mutex);

		const auto& universes = reinterpret_cast<const preferences_manager*>(pSender)->get_universe_configs();

		_configs.clear();

		for (const auto& u : universes)
		{
			if (u.second->is_enabled && u.second->universe_type() == dmx_universe_type::output)
				_configs.push_back(*dynamic_cast<dmx_output_universe_config*>(u.second.get()));
		}
	}

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

		bool is_artnet_packet_sent = false;
		bool is_sacn_packet_sent = false;
		
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
					
					if (!is_artnet_packet_sent)
					{
						_artnet_sequence = _artnet_sequence >= 255 ? 1 : _artnet_sequence + 1;
						is_artnet_packet_sent = true;
					}
					
					dmx_packet_artnet packet(config.protocol_universe, _artnet_sequence, it->second);
					const auto packet_buffer = packet.serialize();

					if (config.is_use_global_destination)
					{
						if (_global_config.is_artnet_global_destination_broadcast)
						{
							Poco::Net::SocketAddress address { _artnet_broadcast_address, k_artnet_port };
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
							Poco::Net::SocketAddress address { _artnet_broadcast_address, k_artnet_port };
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
					
					if (!is_sacn_packet_sent)
					{
						_sacn_sequence = _sacn_sequence >= 255 ? 0 : _sacn_sequence + 1;
						is_sacn_packet_sent = true;
					}
					
					dmx_packet_sacn packet(_system_id, _system_name, config.priority, 
						_global_config.is_send_sacn_sync_packets ? _global_config.sacn_sync_address : 0,
					               _sacn_sequence, sacn_options_flags::none, config.protocol_universe, it->second);
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

		if (_global_config.is_send_artnet_sync_packets && is_artnet_packet_sent)
		{
			sync_packet_artnet packet;
			const auto packet_buffer = packet.serialize();

			Poco::Net::SocketAddress address { _artnet_broadcast_address, k_artnet_port };
	        _artnet_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);
		}
	
		if (_global_config.is_send_sacn_sync_packets && is_sacn_packet_sent)
		{
			sync_packet_sacn packet(_system_id, _sacn_sync_sequence, _global_config.sacn_sync_address);
			const auto packet_buffer = packet.serialize();

			Poco::Net::SocketAddress address { get_sacn_multicast_address(_global_config.sacn_sync_address), k_sacn_port };
	        _sacn_socket->sendTo(packet_buffer.data(), packet_buffer.size(), address);

			_sacn_sync_sequence = _sacn_sync_sequence >= 255 ? 1 : _sacn_sync_sequence + 1;
		}
	}
}
