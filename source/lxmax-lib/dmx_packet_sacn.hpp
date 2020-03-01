/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <cstdint>
#include <vector>
#include <string.h>
#include <Poco/UUID.h>
#include <Poco/Net/IPAddress.h>

#include "endian_helpers.hpp"

namespace lxmax
{
	// TODO: More error checking for sACN header data

	const uint16_t k_sacn_port = 5568;

	const uint8_t k_sacn_id[] { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };

	const uint16_t k_sacn_root_preamble_length = 0x0010;
	const uint16_t k_sacn_root_postamble_length = 0x0010;

	const uint8_t k_sacn_address_data_type = 0xa1;

	enum class sacn_root_vector : uint32_t
	{
		e131_data = 0x00000004,
		e131_extended = 0x00000008
	};

	enum class sacn_e131_vector : uint32_t
	{
		data_packet = 0x00000002
	};

	enum class sacn_dmp_vector : uint8_t
	{
		set_property = 0x02
	};



	inline Poco::Net::IPAddress get_sacn_multicast_address(const universe_address& address)
	{
		uint8_t bytes[4];
		bytes[0] = 239;
		bytes[1] = 255;
		bytes[2] = (address & 0xFF00) >> 8;
		bytes[3] = address & 0x00FF;

		return Poco::Net::IPAddress(bytes, 4);
	}

	#pragma pack(push, 1)
    struct sacn_dmx_header
    {
		// root layer
        uint16_t_be root_preamble_length;
        uint16_t_be root_postamble_length;
        uint8_t id[12];
		uint16_t_be root_flags_length;
		uint32_t_be root_vector;
		uint8_t cid[16];

		// framing layer
		uint16_t_be framing_flags_length;
		uint32_t_be framing_vector;
		char source_name[64];
		uint8_t priority;
		uint16_t_be sync_address;
		uint8_t sequence;
		uint8_t options;
		uint16_t_be universe;

		// dmp layer
		uint16_t_be dmp_flags_length;
		uint8_t dmp_vector;
		uint8_t address_data_type;
		uint16_t_be first_property_address;
		uint16_t_be address_increment;
		uint16_t_be property_value_count;

		sacn_dmx_header()
			: root_preamble_length(k_sacn_root_preamble_length),
			root_postamble_length(k_sacn_root_postamble_length),
			root_vector(static_cast<uint32_t>(sacn_root_vector::e131_data)),
			framing_vector(static_cast<uint32_t>(sacn_e131_vector::data_packet)),
			dmp_vector(static_cast<uint8_t>(sacn_dmp_vector::set_property)),
			address_data_type(k_sacn_address_data_type),
			first_property_address(0x0000),
			address_increment(0x0001)
		{
			memcpy(id, k_sacn_id,sizeof(id));
		}
    };
	#pragma pack(pop)

	const size_t k_min_sacn_dmx_packet_length = sizeof(sacn_dmx_header);

	struct dmx_packet_sacn
	{
		sacn_dmx_header header;
		uint8_t start_code;
		std::vector<uint8_t> dmx_channels;

		dmx_packet_sacn(const Poco::UUID& system_id, const std::string& source_name, uint8_t priority, universe_address sync_address, universe_address address, uint8_t sequence, 
			const universe_buffer& data)
			: dmx_channels(k_universe_length)
		{
			system_id.copyTo(reinterpret_cast<char*>(header.cid));

			header.dmp_flags_length = k_universe_length + 1;

			strncpy(header.source_name, source_name.c_str(), sizeof(header.source_name));

			header.priority = priority;
			header.sync_address = sync_address;
			header.sequence = sequence;
			header.universe = address;
			
			start_code = 0;
			memcpy(dmx_channels.data(), data.data(),k_universe_length);
		}

		static bool deserialize(char* data, size_t length, dmx_packet_sacn& packet)
		{
			if (length < k_min_sacn_dmx_packet_length)
				return false;

			memcpy(&packet.header, data, sizeof(sacn_dmx_header));

			for (size_t i = 0; i < sizeof(k_sacn_id); ++i)
			{
				if (packet.header.id[i] != k_sacn_id[i])
					return false;
			}

			const uint16_t dmx_length = packet.header.dmp_flags_length;

			if (dmx_length < 1 || dmx_length > 513)
				return false;

			if (length < sizeof(sacn_dmx_header) + dmx_length)
				return false;

			packet.dmx_channels.resize(dmx_length - 1);

			packet.start_code = *(data + sizeof(sacn_dmx_header));

			memcpy(packet.dmx_channels.data(), data + sizeof(sacn_dmx_header) + 1, dmx_length - 1);

			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(sacn_dmx_header) + dmx_channels.size());

			memcpy(buffer.data(), &header, sizeof(sacn_dmx_header));
			memcpy(buffer.data() + sizeof(sacn_dmx_header), dmx_channels.data(), dmx_channels.size());

			return buffer;
		}
	};

	#pragma pack(push, 1)
	struct sync_packet_sacn
    {
		// root layer
        uint16_t_be preamble_length;
        uint16_t_be postamble_length;
        uint8_t id[12];
		uint16_t_be root_flags_length;
		uint32_t_be root_vector;
		uint8_t cid[16];

		// framing layer
		uint16_t_be framing_flags_length;
		uint32_t_be framing_vector;
		uint8_t sequence;
		uint16_t_be sync_address;
		uint16_t_be reserved;

		// dmp layer
		uint16_t_be dmp_flags_length;
		uint8_t dmp_vector;
		uint8_t address_data_type;
		uint16_t_be first_property_address;
		uint16_t_be data_length;

		static bool deserialize(char* data, size_t length, sync_packet_sacn& packet)
		{
			if (length < sizeof(sync_packet_sacn))
				return false;

			memcpy(&packet, data, sizeof(sync_packet_sacn));

			for (size_t i = 0; i < sizeof(k_sacn_id); ++i)
			{
				if (packet.id[i] != k_sacn_id[i])
					return false;
			}

			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(sync_packet_sacn));

			memcpy(buffer.data(), this, sizeof(sync_packet_sacn));

			return buffer;
		}
    };
	#pragma pack(pop)
}
