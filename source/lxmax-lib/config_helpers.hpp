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
	std::vector<std::string> get_array(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                  const std::string& key);

	Poco::Net::IPAddress get_ip_address(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                         const std::string& key);

	void set_ip_address(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                         const Poco::Net::IPAddress& value);;

	std::vector<Poco::Net::IPAddress> get_ip_address_vector(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                                     const std::string& key);

	void set_ip_address_vector(Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config, const std::string& key,
	                        const std::vector<Poco::Net::IPAddress>& values);
}
