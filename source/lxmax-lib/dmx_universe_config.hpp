/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cassert>
#include <vector>
#include <sstream>
#include <Poco/Net/IPAddress.h>

#include "dmx_channel_range.hpp"

namespace lxmax
{
	struct dmx_input_universe_config
	{
		dmx_input_universe_config() = default;
		
		dmx_protocol protocol = dmx_protocol::artnet;

		universe_address internal_universe = 1;
		universe_address protocol_universe = 1;

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
	};
	
	struct dmx_output_universe_config
	{
		dmx_output_universe_config() = default;
		
		dmx_protocol protocol = dmx_protocol::artnet;

		universe_address internal_universe = 1;
		universe_address protocol_universe = 1;
		universe_address sync_address = 1;
		int priority = 100;

		bool is_use_global_destination = true;
		bool is_broadcast_or_multicast = true;
		bool is_artnet_alt_broadcast_address = false;
		std::vector<Poco::Net::IPAddress> unicast_addresses;

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
	};

}
