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
	const uint16_t k_sacn_port = 5568;

	const uint8_t k_sacn_id[] { 0x41, 0x53, 0x43, 0x2d, 0x45, 0x31, 0x2e, 0x31, 0x37, 0x00, 0x00, 0x00 };

	const uint16_t k_sacn_root_preamble_length = 0x0010;
	const uint16_t k_sacn_root_postamble_length = 0x0000;
	const uint16_t k_sacn_root_flags = 0x7000;
	
	const uint16_t k_sacn_framing_flags = 0x7000;

	const uint16_t k_sacn_dmp_flags = 0x7000;

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

	enum class sacn_e131_extended_vector : uint32_t
	{
		sync_packet = 0x00000001
	};

	enum class sacn_dmp_vector : uint8_t
	{
		set_property = 0x02
	};

	enum class sacn_options_flags : uint8_t
	{
		none = 0,
		force_sync = 1 << 5,
		stream_terminated = 1 << 6,
		preview_data = 1 << 7
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
	struct sacn_root_layer
	{
		uint16_t_be root_preamble_length;
        uint16_t_be root_postamble_length;
        uint8_t id[12];
		uint16_t_be root_flags_length;
		uint32_t_be root_vector;
		uint8_t cid[16];

		sacn_root_layer(const Poco::UUID& system_id, sacn_root_vector vector)
			: root_preamble_length(k_sacn_root_preamble_length),
			root_postamble_length(k_sacn_root_postamble_length),
			id(),
			root_flags_length(k_sacn_root_flags + ((sizeof(sacn_root_layer) - 16) & 0xFFF)),
			root_vector(static_cast<uint32_t>(vector)),
			cid()
		{
			memcpy(id, k_sacn_id,sizeof(id));
			system_id.copyTo(reinterpret_cast<char*>(cid));
		}

		void set_length(uint16_t length)
		{
			root_flags_length = k_sacn_root_flags + ((length + (sizeof(sacn_root_layer) - 16)) & 0xFFF);
		}
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct sacn_framing_layer_data
	{
		uint16_t_be framing_flags_length;
		uint32_t_be framing_vector;
		char source_name[64];
		uint8_t priority;
		uint16_t_be sync_address;
		uint8_t sequence;
		uint8_t options;
		uint16_t_be universe;

		sacn_framing_layer_data(const std::string& source_name_value, uint8_t priority_value, uint16_t sync_address_value, uint8_t sequence_value,
			sacn_options_flags options_value, uint16_t universe_value)
			: framing_flags_length(k_sacn_root_flags + (sizeof(sacn_framing_layer_data) & 0xFFF)),
			framing_vector(static_cast<uint32_t>(sacn_e131_vector::data_packet)),
			source_name(),
			priority(priority_value),
			sync_address(sync_address_value),
			sequence(sequence_value),
			options(static_cast<uint8_t>(options_value)),
			universe(universe_value)
		{
			strncpy(source_name, source_name_value.c_str(), sizeof(source_name));
		}
		
		void set_length(uint16_t length)
		{
			framing_flags_length = k_sacn_root_flags + ((length + sizeof(sacn_framing_layer_data)) & 0xFFF);
		}
	};
	#pragma pack(pop)

	#pragma pack(push, 1)
	struct sacn_framing_layer_sync
	{
		uint16_t_be framing_flags_length;
		uint32_t_be framing_vector;
		uint8_t sequence;
		uint16_t_be sync_address;
		uint16_t_be reserved;

		sacn_framing_layer_sync(uint8_t sequence_value, uint16_t sync_address_value)
			: framing_flags_length(k_sacn_root_flags + (sizeof(sacn_framing_layer_sync) & 0xFFF)),
			framing_vector(static_cast<uint32_t>(sacn_e131_extended_vector::sync_packet)),
			sequence(sequence_value),
			sync_address(sync_address_value),
			reserved(0)
		{
			
		}
	};
	#pragma pack(pop)
	
	#pragma pack(push, 1)
    struct sacn_dmp_layer
    {
		uint16_t_be dmp_flags_length;
		uint8_t dmp_vector;
		uint8_t address_data_type;
		uint16_t_be first_property_address;
		uint16_t_be address_increment;
		uint16_t_be property_value_count;
    	uint8_t dmx_start_code;

		sacn_dmp_layer()
			: dmp_flags_length(k_sacn_dmp_flags),
			dmp_vector(static_cast<uint8_t>(sacn_dmp_vector::set_property)),
			address_data_type(k_sacn_address_data_type),
			first_property_address(0x0000),
			address_increment(0x0001),
    		property_value_count(0x0001),
    		dmx_start_code(0x00)
		{
			
		}

    	void set_length(uint16_t length)
		{
			dmp_flags_length = k_sacn_root_flags + ((length + sizeof(sacn_dmp_layer)) & 0xFFF);
		}

    	void set_channel_count(uint16_t count)
		{
			property_value_count = count + 1;
		}
    };
	#pragma pack(pop)

	const size_t k_min_sacn_dmx_packet_length = sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer);
	
	struct dmx_packet_sacn
	{
		sacn_root_layer root_layer;
		sacn_framing_layer_data framing_layer;
		sacn_dmp_layer dmp_layer;
		std::vector<uint8_t> dmx_channels;

		dmx_packet_sacn(const Poco::UUID& system_id, const std::string& source_name, uint8_t priority, universe_address sync_address, 
			uint8_t sequence, sacn_options_flags options, universe_address universe, const universe_buffer& data)
			: root_layer(system_id, sacn_root_vector::e131_data),
			framing_layer(source_name, priority, sync_address, sequence, options, universe),
			dmx_channels(data.size())
		{
			root_layer.set_length(sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer) + dmx_channels.size());
			framing_layer.set_length(sizeof(sacn_dmp_layer) + dmx_channels.size());
			dmp_layer.set_length(data.size());
			dmp_layer.set_channel_count(dmx_channels.size());

			memcpy(dmx_channels.data(), data.data(), std::min(data.size(), dmx_channels.size()));
		}

		static bool deserialize(char* data, size_t length, dmx_packet_sacn& packet)
		{
			if (length < k_min_sacn_dmx_packet_length)
				return false;

			memcpy(&packet.root_layer, data, sizeof(sacn_root_layer));

			for (size_t i = 0; i < sizeof(k_sacn_id); ++i)
			{
				if (packet.root_layer.id[i] != k_sacn_id[i])
					return false;
			}

			memcpy(&packet.framing_layer, data + sizeof(sacn_root_layer), sizeof(sacn_framing_layer_data));
			memcpy(&packet.dmp_layer, data + sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data), sizeof(sacn_dmp_layer));

			const uint16_t dmx_length = (packet.dmp_layer.dmp_flags_length & 0x1000) - sizeof(sacn_dmp_layer);

			if (dmx_length < 1 || dmx_length > 513)
				return false;

			// TODO: More error checking

			if (length < sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer) + dmx_length)
				return false;

			packet.dmx_channels.resize(dmx_length);

			memcpy(packet.dmx_channels.data(), data + sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer),
				dmx_length);

			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer) + dmx_channels.size());

			memcpy(buffer.data(), &root_layer, sizeof(sacn_root_layer));
			memcpy(buffer.data() + sizeof(sacn_root_layer), &framing_layer, sizeof(sacn_framing_layer_data));
			memcpy(buffer.data() + sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data), &dmp_layer, sizeof(sacn_dmp_layer));
			memcpy(buffer.data() + sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_data) + sizeof(sacn_dmp_layer),
				dmx_channels.data(), dmx_channels.size());

			return buffer;
		}
	};

	struct sync_packet_sacn
    {
		sacn_root_layer root_layer;
		sacn_framing_layer_sync framing_layer;

		sync_packet_sacn(const Poco::UUID& system_id, uint8_t sequence, universe_address sync_address)
			: root_layer(system_id, sacn_root_vector::e131_extended),
			framing_layer(sequence, sync_address)
		{
			root_layer.set_length(sizeof(sacn_framing_layer_sync));
		}
		
		static bool deserialize(char* data, size_t length, sync_packet_sacn& packet)
		{
			if (length < sizeof(sync_packet_sacn))
				return false;

			memcpy(&packet, data, sizeof(sync_packet_sacn));

			for (size_t i = 0; i < sizeof(k_sacn_id); ++i)
			{
				if (packet.root_layer.id[i] != k_sacn_id[i])
					return false;
			}

			return true;
		}

		std::vector<char> serialize() const noexcept
		{
			std::vector<char> buffer(sizeof(sacn_root_layer) + sizeof(sacn_framing_layer_sync));

			memcpy(buffer.data(), &root_layer, sizeof(sacn_root_layer));
			memcpy(buffer.data() + sizeof(sacn_root_layer), &framing_layer, sizeof(sacn_framing_layer_sync));

			return buffer;
		}
    };
}
