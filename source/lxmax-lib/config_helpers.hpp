/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <vector>
#include <Poco/Util/AbstractConfiguration.h>
#include <Poco/Net/IPAddress.h>

namespace lxmax::config_helpers
{
	inline Poco::Net::IPAddress getIpAddress(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key)
	{
		Poco::Net::IPAddress value;
		const std::string raw_value = config->getString(key);

		if (!Poco::Net::IPAddress::tryParse(raw_value, value))
			throw Poco::SyntaxException("Cannot convert to IPAddress", raw_value);

		return value;
	}

	inline void setIpAddress(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                         const Poco::Net::IPAddress& value)
	{
		config->setString(key, value.toString());
	};

	inline std::vector<Poco::Net::IPAddress> getIpAddressVector(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                                            const std::string& key)
	{
		std::vector<Poco::Net::IPAddress> values;
		std::vector<std::string> keys;
		config->keys(key, keys);

		for (const auto& k : keys)
			values.push_back(getIpAddress(config, k));

		return values;
	}

	inline void setIpAddressVector(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                               const std::vector<Poco::Net::IPAddress>& values)
	{
		for (size_t i = 0; i < values.size(); ++i)
			setIpAddress(config, key + "[" + std::to_string(i) + "]", values[i]);
	}
}
