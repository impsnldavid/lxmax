/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cassert>
#include <vector>
#include <sstream>
#include <Poco/Net/IPAddress.h>
#include <Poco/Util/AbstractConfiguration.h>

#include "dmx_channel_range.hpp"
#include "config_helpers.hpp"

#define MEMBER_WITH_KEY(type, name, default_value) static const inline std::string key_##name = #name;\
							  type name = default_value;  

namespace lxmax
{
	struct dmx_input_universe_config
	{
		dmx_input_universe_config() = default;
		
		MEMBER_WITH_KEY(dmx_protocol, protocol, dmx_protocol::artnet)

		MEMBER_WITH_KEY(universe_address, internal_universe, 1)
		MEMBER_WITH_KEY(universe_address, protocol_universe, 1)

		std::string summary() const
		{
			std::stringstream s;
			
			switch(protocol)
			{
				case dmx_protocol::artnet:
				
					s <<  "Net " << ((protocol_universe & 0x8000) >> 8)
					  << ", Sub-net " << ((protocol_universe & 0xF0) >> 4)
					  << ", Universe " << (protocol_universe & 0x0F);
				
					break;

				case dmx_protocol::sacn:
					break;

				default:
					assert(false);
			}

			return s.str();
		}

		void read_from_configuration(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config)
		{
			protocol = static_cast<dmx_protocol>(config->getInt(key_protocol));
			
			internal_universe = config->getInt(key_internal_universe);
			protocol_universe = config->getInt(key_protocol_universe);
		}

		void write_to_configuration(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config) const
		{
			config->setInt(key_protocol, static_cast<int>(protocol));

			config->setInt(key_internal_universe, internal_universe);
			config->setInt(key_protocol_universe, protocol_universe);
		}
	};
	
	struct dmx_output_universe_config
	{
		dmx_output_universe_config() = default;
		
		MEMBER_WITH_KEY(dmx_protocol, protocol, dmx_protocol::artnet)

		MEMBER_WITH_KEY(universe_address, internal_universe, 1)
		MEMBER_WITH_KEY(universe_address, protocol_universe, 1)
		MEMBER_WITH_KEY(universe_address, sync_address, 1)
		MEMBER_WITH_KEY(int, priority, 100)

		MEMBER_WITH_KEY(bool, is_use_global_destination, true)
		MEMBER_WITH_KEY(bool, is_broadcast_or_multicast, true)
		MEMBER_WITH_KEY(bool, is_artnet_alt_broadcast_address, false)
		MEMBER_WITH_KEY(std::vector<Poco::Net::IPAddress>, unicast_addresses, { })

		void set_artnet_protocol_address(int net, int subnet, int universe)
		{
			protocol_universe = ((net & 0x80) << 8) + ((subnet & 0x0F) << 4) + (universe & 0x0F);
		}

		std::string summary() const
		{
			std::stringstream s;
			
			switch(protocol)
			{
				case dmx_protocol::artnet:
				
					s <<  "Net " << ((protocol_universe & 0x8000) >> 8)
					  << ", Sub-net " << ((protocol_universe & 0xF0) >> 4)
					  << ", Universe " << (protocol_universe & 0x0F);

					if (!is_use_global_destination)
					{
						if (is_broadcast_or_multicast)
						{
							s << ", Broadcast to " << (is_artnet_alt_broadcast_address ? "10.255.255.255" : "2.255.255.255");
						}
						else
						{
							s << ", Unicast to ";

							bool is_first = true;
							for(const auto& a : unicast_addresses)
							{
								s << a.toString();
								is_first = false;
								if (!is_first)
									s << ", ";
							}
						}
					}
				
					break;
					
				case dmx_protocol::sacn:

					s << "Priority " << priority
					  << "Sync Address " << sync_address;

					if (!is_use_global_destination)
					{
						if (is_broadcast_or_multicast)
						{
							s << ", Multicast to " << get_sacn_multicast_address(protocol_universe).toString();
						}
						else
						{
							s << ", Unicast to ";

							bool is_first = true;
							for(const auto& a : unicast_addresses)
							{
								s << a.toString();
								is_first = false;
								if (!is_first)
									s << ", ";
							}
						}
					}		

					break;
				
				default:
					assert(false);
			}

			return s.str();
		}

		void read_from_configuration(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config)
		{
			protocol = static_cast<dmx_protocol>(config->getInt(key_protocol));
			
			internal_universe = config->getInt(key_internal_universe);
			protocol_universe = config->getInt(key_protocol_universe);
			sync_address = config->getInt(key_sync_address);
			priority = config->getInt(key_priority);

			is_use_global_destination = config->getBool(key_is_use_global_destination);
			is_broadcast_or_multicast = config->getBool(key_is_broadcast_or_multicast);
			is_artnet_alt_broadcast_address = config->getBool(key_is_artnet_alt_broadcast_address);
			unicast_addresses = config_helpers::getIpAddressVector(config, key_unicast_addresses);
		}

		void write_to_configuration(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config) const
		{
			config->setInt(key_protocol, static_cast<int>(protocol));

			config->setInt(key_internal_universe, internal_universe);
			config->setInt(key_protocol_universe, protocol_universe);
			config->setInt(key_sync_address, sync_address);
			config->setInt(key_priority, priority);

			config->setBool(key_is_use_global_destination, is_use_global_destination);
			config->setBool(key_is_broadcast_or_multicast, is_broadcast_or_multicast);
			config->setBool(key_is_artnet_alt_broadcast_address, is_artnet_alt_broadcast_address);
			config_helpers::setIpAddressVector(config, key_unicast_addresses, unicast_addresses);
		}
	};

}

#undef MEMBER_WITH_KEY
