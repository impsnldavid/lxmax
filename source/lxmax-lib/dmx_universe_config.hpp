/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <vector>
#include <Poco/Net/IPAddress.h>

#include "dmx_channel_range.hpp"

namespace lxmax
{
	struct dmx_output_universe_config
	{
		dmx_protocol protocol;

		universe_address internal_universe;
		universe_address protocol_universe;
		universe_address sync_address;
		int priority;

		bool is_use_global_destination;
		bool is_broadcast_or_multicast;
		std::vector<Poco::Net::IPAddress> unicast_addresses;

		void set_artnet_protocol_address(int net, int subnet, int universe)
		{
			internal_universe = ((net & 0x80) << 8) + ((subnet & 0x0F) << 4) + (universe & 0x0F);
		}
	};

}
