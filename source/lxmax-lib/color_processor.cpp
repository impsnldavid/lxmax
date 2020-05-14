/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "color_processor.hpp"

namespace lxmax
{
	// Adapted from https://www.cs.rit.edu/~ncs/color/t_convert.html

	color_values color_processor::get_rgb() const
	{
		switch (_colorspace)
		{
		default:
		case colorspace::none:
			assert(false);
			return color_values{0, 0, 0};

		case colorspace::rgb:
			return _values;
			break;

		case colorspace::cmy:
			return invert(_values);
			break;

		case colorspace::hsb:
			return hsb_to_rgb(_values);
		}
	}

	color_values color_processor::get_cmy() const
	{
		switch (_colorspace)
		{
		default:
		case colorspace::none:
			assert(false);
			return color_values{0, 0, 0};

		case colorspace::rgb:
			return invert(_values);
			break;

		case colorspace::cmy:
			return _values;
			break;

		case colorspace::hsb:
			return rgb_to_hsb(invert(_values));
		}
	}

	color_values color_processor::get_hsb() const
	{
		switch (_colorspace)
		{
		default:
		case colorspace::none:
			assert(false);
			return color_values{0, 0, 0};

		case colorspace::rgb:
			return rgb_to_hsb(_values);

		case colorspace::cmy:
			return rgb_to_hsb(invert(_values));

		case colorspace::hsb:
			return _values;
		}
	}

	color_values color_processor::rgb_to_hsb(color_values rgb)
	{
		color_values hsb;
		
		const double min = std::min({rgb[0], rgb[1], rgb[2]});
		const double max = std::max({rgb[0], rgb[1], rgb[2]});
		hsb[2] = max;
		
		const double delta = max - min;

		if (max != 0)
		{
			hsb[1] = delta / max;
		}
		else
		{
			hsb[1] = 0;
			hsb[0] = 0;
			return hsb;
		}

		if (rgb[0] == max)
			hsb[0] = (rgb[1] - rgb[2]) / delta;
		else if (rgb[1] == max)
			hsb[0] = 2 + (rgb[2] - rgb[0]) / delta;
		else
			hsb[0] = 4 + (rgb[0] - rgb[1]) / delta;
		
		hsb[0] *= 60; // degrees
		if (hsb[0] < 0)
			hsb[0] += 360;

		hsb[0] /= 360.; // normalize

		return hsb;
	}

	color_values color_processor::hsb_to_rgb(color_values hsb)
	{
		color_values rgb;
		
		if (hsb[1] == 0)
		{
			// achromatic (grey)
			rgb[0] = hsb[2];
			rgb[1] = hsb[2];
			rgb[2] = hsb[2];
			return rgb;
		}

		hsb[0] *= 360; // de-normalize
		hsb[0] /= 60; // sector 0 to 5

		const int i = floor(hsb[0]);
		const double f = hsb[0] - i; // factorial part of h
		const double p = hsb[2] * (1 - hsb[1]);
		const double q = hsb[2] * (1 - hsb[1] * f);
		const double t = hsb[2] * (1 - hsb[1] * (1 - f));

		switch (i)
		{
		case 0:
			rgb[0] = hsb[2];
			rgb[1] = t;
			rgb[2] = p;
			break;
		case 1:
			rgb[0] = q;
			rgb[1] = hsb[2];
			rgb[2] = p;
			break;
		case 2:
			rgb[0] = p;
			rgb[1] = hsb[2];
			rgb[2] = t;
			break;
		case 3:
			rgb[0] = p;
			rgb[1] = q;
			rgb[2] = hsb[2];
			break;
		case 4:
			rgb[0] = t;
			rgb[1] = p;
			rgb[2] = hsb[2];
			break;
		default: // case 5:
			rgb[0] = hsb[2];
			rgb[1] = p;
			rgb[2] = q;
			break;
		}
	}

	color_values color_processor::invert(color_values values)
	{
		return color_values { 1. - values[0], 1. - values[1], 1. - values[2] };
	}
}
