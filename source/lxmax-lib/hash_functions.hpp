/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <Poco/Net/IPAddress.h>

namespace std
{
	template<>
	struct hash<Poco::Net::IPAddress>
	{
		// TODO: Replace this with something more efficient
		size_t operator()(const Poco::Net::IPAddress& obj) const noexcept
		{
			return hash<std::string>()(obj.toString());
		}
	};
}