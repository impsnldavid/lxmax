/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <vector>
#include <Poco/Net/IPAddress.h>
#include <Poco/Util/AbstractConfiguration.h>

#include "config_helpers.hpp"

#define MEMBER_WITH_KEY(type, name, default_value) static const inline std::string key_##name = #name;\
							  type name = default_value;  

namespace lxmax
{
	struct global_config
	{
		global_config() = default;

		MEMBER_WITH_KEY(bool, is_output_empty_universes, false)
		MEMBER_WITH_KEY(bool, is_force_output_at_framerate, false)
		MEMBER_WITH_KEY(int, framerate, 44)
		MEMBER_WITH_KEY(bool, is_allow_nondmx_framerate, false)

		MEMBER_WITH_KEY(Poco::Net::IPAddress, artnet_network_adapter, Poco::Net::IPAddress("0.0.0.0"))
		MEMBER_WITH_KEY(bool, is_artnet_global_destination_broadcast, false);
		MEMBER_WITH_KEY(std::vector<Poco::Net::IPAddress>, artnet_global_destination_unicast_addresses, { Poco::Net::IPAddress("127.0.0.1") })
		MEMBER_WITH_KEY(bool, is_send_artnet_sync_packets, true)

		MEMBER_WITH_KEY(Poco::Net::IPAddress, sacn_network_adapter, Poco::Net::IPAddress("0.0.0.0"))
		MEMBER_WITH_KEY(bool, is_sacn_global_destination_multicast, true)
		MEMBER_WITH_KEY(std::vector<Poco::Net::IPAddress>, sacn_global_destination_unicast_addresses, { Poco::Net::IPAddress("127.0.0.1") })
		MEMBER_WITH_KEY(bool, is_send_sacn_sync_packets, false)
		MEMBER_WITH_KEY(int, sacn_sync_address, 1)

		void read_from_configuration(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config)
		{
			is_output_empty_universes = config->getBool(key_is_output_empty_universes);
			is_force_output_at_framerate = config->getBool(key_is_force_output_at_framerate);
			framerate = config->getInt(key_framerate);
			is_allow_nondmx_framerate = config->getBool(key_is_allow_nondmx_framerate);
			
			artnet_network_adapter = config_helpers::get_ip_address(config, key_artnet_network_adapter);
			is_artnet_global_destination_broadcast = config->getBool(key_is_artnet_global_destination_broadcast);
			artnet_global_destination_unicast_addresses = config_helpers::get_ip_address_vector(config, key_artnet_global_destination_unicast_addresses);
			is_send_artnet_sync_packets = config->getBool(key_is_send_artnet_sync_packets);
			
			sacn_network_adapter = config_helpers::get_ip_address(config, key_sacn_network_adapter);
			is_sacn_global_destination_multicast = config->getBool(key_is_sacn_global_destination_multicast);
			sacn_global_destination_unicast_addresses = config_helpers::get_ip_address_vector(config, key_sacn_global_destination_unicast_addresses);
			is_send_sacn_sync_packets = config->getBool(key_is_send_sacn_sync_packets);
			sacn_sync_address = config->getInt(key_sacn_sync_address);
		}
		
		void write_to_configuration(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config) const
		{
			config->setBool(key_is_output_empty_universes, is_output_empty_universes);
			config->setBool(key_is_force_output_at_framerate, is_force_output_at_framerate);
			config->setInt(key_framerate, framerate);
			config->setBool(key_is_allow_nondmx_framerate, is_allow_nondmx_framerate);

			config_helpers::set_ip_address(config, key_artnet_network_adapter, artnet_network_adapter);
			config->setBool(key_is_artnet_global_destination_broadcast, is_artnet_global_destination_broadcast);
			config_helpers::set_ip_address_vector(config, key_artnet_global_destination_unicast_addresses, artnet_global_destination_unicast_addresses);
			config->setBool(key_is_send_artnet_sync_packets, is_send_artnet_sync_packets);

			config_helpers::set_ip_address(config, key_sacn_network_adapter, sacn_network_adapter);
			config->setBool(key_is_sacn_global_destination_multicast, is_sacn_global_destination_multicast);
			config_helpers::set_ip_address_vector(config, key_sacn_global_destination_unicast_addresses, sacn_global_destination_unicast_addresses);
			config->setBool(key_is_send_sacn_sync_packets, is_send_sacn_sync_packets);
			config->setInt(key_sacn_sync_address, sacn_sync_address);
		}
	};
}

#undef MEMBER_WITH_KEY