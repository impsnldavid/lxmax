/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "color_personality.hpp"

#include <Poco/NumberParser.h>
#include <sstream>
#include <Poco/StringTokenizer.h>

namespace lxmax
{
	Poco::RegularExpression color_personality_element::pattern  { "([A-Z][a-z]?)(?:(8|16|24|32|!16|!24|!32)(\{\d+\})?)?"};

	color_personality_element::color_personality_element(color_component component, value_precision precision, int64_t data):
		component(component),
		precision(precision),
		data(data)
	{
	}

	bool color_personality_element::from_string(const std::string& string_value, color_personality_element& value)
	{
		Poco::RegularExpression::MatchVec matches;

		if (pattern.match(string_value, 0, matches) < 2)
			return false;

		assert(color_component_helper::from_string(string_value.substr(matches[1].offset, matches[1].length), value.
				component));

		if (matches.size() >= 4)
		{
			int64_t data_value;
			assert(Poco::NumberParser::tryParse64(string_value.substr(matches[3].offset, matches[3].length), data_value));
			value.data = data_value;
		}

		if (matches.size() >= 3)
			assert(
			precision_helper::from_string(string_value.substr(matches[2].offset, matches[2].length), value.precision));

		if (matches.size() == 2)
			value.precision = value_precision::_8bit;

		return true;
	}

	std::string color_personality_element::to_string() const
	{
		if (!data.has_value())
		{
			return precision == value_precision::_8bit
				? color_component_helper::to_string(component)
				: color_component_helper::to_string(component) + precision_helper::to_string(precision);
		}
		else
		{
			return precision == value_precision::_8bit
				? color_component_helper::to_string(component) + "{" + std::to_string(data.value()) + "}"
				: color_component_helper::to_string(component) + precision_helper::to_string(precision) + "{" + std::to_string(data.value()) + "}";
		}
		
	}

	std::string to_string(const color_personality& value)
	{
		std::stringstream ss;

		bool is_first = true;

		for (const auto c : value)
		{
			if (!is_first)
				ss << " ";

			ss << c.to_string();

			is_first = false;
		}

		return ss.str();
	}

	bool from_string(const std::string& string_value, color_personality& value)
	{
		value.clear();
		
		Poco::StringTokenizer tokenizer(string_value, " ");
		value.reserve(tokenizer.count());
		
		for(const auto& t : tokenizer)
		{
			color_personality_element el;
			if (!color_personality_element::from_string(t, el))
				return false;
			
			value.push_back(el);
		}

		return true;
	}
}
