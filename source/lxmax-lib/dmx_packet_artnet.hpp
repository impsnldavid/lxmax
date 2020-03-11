/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.


#pragma once

#include <cstdint>
#include <vector>

#include "common.hpp"
#include "endian_helpers.hpp"

namespace lxmax
{
	const uint16_t k_artnet_port = 0x1936;

	static const char* k_artnet_id = "Art-Net";
	const uint16_t k_artnet_prot_ver = 14;
	const uint16_t k_artnet_opcode_dmx = 0x5000;
	const uint16_t k_artnet_opcode_sync = 0x5200;

	#pragma pack(push, 1)
    struct artdmx_header
    {
		char id[8];
		uint16_t opcode;
		uint16_t_be prot_ver;
		uint8_t sequence;
		uint8_t physical;
		uint8_t sub_uni;
		uint8_t net;
		uint16_t_be length;

		artdmx_header()
			: opcode(0x5000),
			prot_ver(k_artnet_prot_ver),
			sequence(0),
			physical(0),
			sub_uni(0),
			net(0),
			length(0)
		{
			memcpy(id, k_artnet_id, sizeof(id));
		}
    };
	#pragma pack(pop)

	const size_t k_min_artnet_dmx_packet_length = sizeof(artdmx_header) + 2;
	

	struct dmx_packet_artnet
	{
		artdmx_header header;
		std::vector<uint8_t> dmx_channels;

		dmx_packet_artnet() = default;

		dmx_packet_artnet(universe_address address, uint8_t sequence, const universe_buffer& data)
			: dmx_channels(data.size())
		{
			header.sub_uni = address & 0x00FF;
			header.net = (address & 0x7F00) >> 8;
			header.sequence = sequence;
			header.length = k_universe_length;
			memcpy(dmx_channels.data(), data.data(),std::min(data.size(), dmx_channels.size()));
		}

		static bool deserialize(char* data, size_t length, dmx_packet_artnet& packet) noexcept
		{
			if (length < k_min_artnet_dmx_packet_length)
				return false;

			memcpy(&packet.header, data, sizeof(artdmx_header));

			if (strcmp(packet.header.id, k_artnet_id) != 0)
				return false;

			if (packet.header.prot_ver < k_artnet_prot_ver)
				return false;

			const uint16_t dmx_length = packet.header.length;

			if (dmx_length < 2 || dmx_length > 512)
				return false;

			if (length < sizeof(artdmx_header) + dmx_length)
				return false;

			packet.dmx_channels.resize(dmx_length);
			memcpy(packet.dmx_channels.data(), data + sizeof(artdmx_header), dmx_length);

			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(artdmx_header) + dmx_channels.size());

			memcpy(buffer.data(), &header, sizeof(artdmx_header));
			memcpy(buffer.data() + sizeof(artdmx_header), dmx_channels.data(), dmx_channels.size());

			return buffer;
		}
	};

	#pragma pack(push, 1)
	struct sync_packet_artnet
    {
		char id[8];
		uint16_t opcode;
		uint16_t_be prot_ver;
		uint8_t aux_1;
		uint8_t aux_2;

		sync_packet_artnet()
			: opcode(k_artnet_opcode_sync),
			prot_ver(k_artnet_prot_ver),
			aux_1(0),
			aux_2(0)
		{
			memcpy(id, k_artnet_id, sizeof(id));
		}

		static bool deserialize(char* data, size_t length, sync_packet_artnet& packet)
		{
			if (length < sizeof(sync_packet_artnet))
				return false;

			memcpy(&packet, data, sizeof(sync_packet_artnet));

			if (strcmp(packet.id, k_artnet_id) != 0)
				return false;

			if (packet.prot_ver < k_artnet_prot_ver)
				return false;
			
			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(sync_packet_artnet));

			memcpy(buffer.data(), this, sizeof(sync_packet_artnet));

			return buffer;
		}
    };
	#pragma pack(pop)
}
