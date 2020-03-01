/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <vector>
#include <Poco/Net/IPAddress.h>

namespace lxmax
{
	struct global_config
	{
		bool is_output_empty_universes;
		bool is_fixed_framerate_enabled;
		int fixed_framerate;
		bool is_allow_nondmx_fixed_framerate;
		
		Poco::Net::IPAddress art_net_network_adapter;
		bool artnet_global_destination_broadcast;
		std::vector<Poco::Net::IPAddress> artnet_global_destination_unicast_addresses;

		Poco::Net::IPAddress sacn_network_adapter;
		bool sacn_global_desination_multicast;
		std::vector<Poco::Net::IPAddress> sacn_global_destination_unicast_addresses;
	};
}