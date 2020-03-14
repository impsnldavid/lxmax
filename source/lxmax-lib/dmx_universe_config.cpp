/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "dmx_universe_config.hpp"

namespace lxmax
{
	std::unique_ptr<dmx_universe_config> dmx_universe_config::create_from_configuration(
		const Poco::AutoPtr<Poco::Util::AbstractConfiguration>& config)
	{
		std::unique_ptr<dmx_universe_config> universe_config;
		
		const dmx_universe_type universe_type = dmx_universe_type_from_string(config->getString(key_universe_type));

		switch(universe_type)
		{
			case dmx_universe_type::input:
				universe_config = std::make_unique<dmx_input_universe_config>();
				break;
			case dmx_universe_type::output:
				universe_config = std::make_unique<dmx_output_universe_config>();
				break;
			case dmx_universe_type::none:
			default:
				throw Poco::SyntaxException("Invalid universe type");
		}
		
		universe_config->read_from_configuration(config);
		
		return universe_config;
	}
}
