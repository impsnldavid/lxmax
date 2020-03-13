/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "config_helpers.hpp"
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Array.h>

namespace lxmax::config_helpers
{
	std::vector<std::string> get_array(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                  const std::string& key)
	{
		std::vector<std::string> values;
		const std::string raw_values = config->getString(key);

		if (*raw_values.begin() != '[')
		{
			// This isn't a JSON array so must be just a single value
			values.push_back(raw_values);
			return values;
		}
		
		Poco::JSON::Parser parser;
		const Poco::Dynamic::Var result = parser.parse(raw_values);
		const auto& json_array = result.extract<Poco::JSON::Array::Ptr>();

		for (auto it = json_array->begin(); it != json_array->end(); ++it)
		{
			const auto& value = it->convert<std::string>();

			if (!value.empty())
				values.push_back(value);
		}

		return values;
	}

	Poco::Net::IPAddress get_ip_address(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                  const std::string& key)
	{
		Poco::Net::IPAddress value;
		const std::string raw_value = config->getString(key);

		if (!Poco::Net::IPAddress::tryParse(raw_value, value))
			throw Poco::SyntaxException("Cannot convert to IPAddress", raw_value);

		return value;
	}

	void set_ip_address(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                  const Poco::Net::IPAddress& value)
	{
		config->setString(key, value.toString());
	}

	std::vector<Poco::Net::IPAddress> get_ip_address_vector(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                                     const std::string& key)
	{
		std::vector<Poco::Net::IPAddress> values;
		std::vector<std::string> keys;

		for (const auto& raw_value : get_array(config, key))
		{
			Poco::Net::IPAddress value;
			if (!Poco::Net::IPAddress::tryParse(raw_value, value))
				throw Poco::SyntaxException("Cannot convert to IPAddress", raw_value);

			values.push_back(value);
		}

		return values;
	}

	void set_ip_address_vector(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                        const std::vector<Poco::Net::IPAddress>& values)
	{
		if (values.empty())
		{
			config->setString(key + "[0]", "");
		}
		else
		{
			for (size_t i = 0; i < values.size(); ++i)
				set_ip_address(config, key + "[" + std::to_string(i) + "]", values[i]);
		}
	}
}
