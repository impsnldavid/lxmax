/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <array>
#include "color_component.hpp"

namespace lxmax
{
	using color_values = std::array<double, 3>;

	class color_processor
	{
		color_values _values { 0, 0, 0 };
		colorspace _colorspace { colorspace::rgb };
		
	public:
		void set_rgb(double r, double g, double b)
		{
			_colorspace = colorspace::rgb;
			_values[0] = r;
			_values[1] = g;
			_values[2] = b;
		}

		void set_cmy(double c, double m, double y)
		{
			_colorspace = colorspace::cmy;
			_values[0] = c;
			_values[1] = m;
			_values[2] = y;
		}

		void set_hsb(double h, double s, double b)
		{
			_colorspace = colorspace::hsb;
			_values[0] = h;
			_values[1] = s;
			_values[2] = b;
		}

		color_values get_rgb() const;

		color_values get_cmy() const;

		color_values get_hsb() const;

		double get_white() const
		{
			
		}

		double get_amber() const
		{
			
		}

	private:
		static color_values rgb_to_hsb(color_values rgb);

		static color_values hsb_to_rgb(color_values hsb);

		static color_values invert(color_values values);
	};
}
