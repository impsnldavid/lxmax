/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "config_helpers.hpp"
#include <Poco/JSON/Parser.h>

namespace lxmax::config_helpers
{
	std::vector<std::string> getArray(const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config,
	                                  const std::string& key)
	{
		std::vector<std::string> values;
		const std::string raw_values = config->getString(key);

		if (*raw_values.begin() != '[')
		{
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
}
