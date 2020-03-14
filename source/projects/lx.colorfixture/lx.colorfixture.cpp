/// @file
///	@ingroup 	lxmax
///	@copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"

using namespace c74::min;
using namespace c74::min::ui;


class lx_colorfixture : public object<lx_colorfixture> {
public:
	MIN_DESCRIPTION	{"Send DMX control data to one or more dimmers."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler"};
	MIN_RELATED		{""};

	inlet<>  input	{ this, "lx.colorfixture" };


    argument<number> channel_arg { this, "channel", "DMX start channel for dimmer(s)",
        MIN_ARGUMENT_FUNCTION {
            channel = arg;
        }
    };
    
    argument<number> universe_arg { this, "universe", "DMX universe number",
        MIN_ARGUMENT_FUNCTION {
            universe = arg;
        }
    };
    
    attribute<int, threadsafe::no, limit::clamp> channel { this, "channel", 1,
        range { 1, 512 },
        title { "DMX Channel" },
        description { "DMX start channel for fixture" },
        category {"lx.colorfixture"}, order { 1 },
        setter { MIN_FUNCTION {
            int value = args[0];
            return { MIN_CLAMP(value, 1, 512) };
        }}
    };
    
    attribute<int, threadsafe::no, limit::clamp> universe { this, "universe", 1,
        range { 1, 65535 },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.colorfixture"}, order { 2 }
    };


	// the actual attribute for the message
	attribute<symbol> personality { this, "personality", "RGB",
        title { "Personality" },
		description {
			"Defines the personality of the fixture from color components (R, G, B, C, M, Y, etc)"
		},
        category {"lx.colorfixture"}, order { 3 }
	};
    
    attribute<color> fixture_color { this, "fixture_color", color::white,
        title { "Fixture Color" },
        description { "Sets the color of the fixture" },
        category {"lx.colorfixture"}, order { 4 }
    };
};


MIN_EXTERNAL(lx_colorfixture);
