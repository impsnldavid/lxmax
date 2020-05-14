/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "version_info.hpp"
#include "c74_min.h"
#include "color_personality.hpp"
#include "color_processor.hpp"
#include "fixture.hpp"
#include "precision_helpers.hpp"
#include "common.hpp"

using namespace c74;
using namespace min;

atoms to_atoms(const lxmax::color_personality& value)
{
	atoms a;
	
	for(const auto& c : value)
		a.push_back(c.to_string());

	return a;
}

class lx_colorfixture : public object<lx_colorfixture>, public lxmax::fixture {

	instance _lxmax_service { };
	
	std::mutex _value_mutex;

	lxmax::color_personality _personality;
	lxmax::color_processor _processor;

	int _channel_count { 0 };
	
	void update_patch_info(int universe, int channel)
    {
	    set_patch_info(lxmax::fixture_patch_info("Color Fixture", false, 
            { universe, channel, _channel_count}));
    	set_updated();
    }

public:

	MIN_DESCRIPTION	{"Send DMX control data to a fixture with basic color control"};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler"};
	MIN_RELATED		{"lx.colorfixture"};

	inlet<>  input	{ this, "lx.colorfixture" };

	lx_colorfixture(const atoms& args = {})
	{
        if (dummy())
			return;
		
        _lxmax_service = get_lxmax_service_and_check_version(*this, lxmax::GIT_VERSION_STR);
		
		set_manager(get_fixture_manager(*this, _lxmax_service));
		update_patch_info(attr_universe.get(), attr_channel.get());
    }
    
    attribute<int, threadsafe::no, limit::clamp> attr_channel { this, "channel", 1,
        range { 1, 512 },
        title { "DMX Channel" },
		description { "DMX start channel for fixture" },
        category {"lx.colorfixture"}, order { 2 },
    	setter { MIN_FUNCTION {

            update_patch_info(attr_universe.get(), args[0]);
            return { args[0] };
        }}
	};
    
    attribute<int, threadsafe::no, limit::clamp> attr_universe { this, "universe", 1,
        range { lxmax::k_universe_min, lxmax::k_universe_max },
        title { "DMX Universe" },
        description { "DMX universe number" },
        category {"lx.colorfixture"}, order { 3 },
    	setter { MIN_FUNCTION {

            update_patch_info(args[0], attr_channel.get());
            return { args[0] };
        }}
    };

	attribute<symbol> attr_personality { this, "personality", "R G B",
		title { "Personality"},
		description { "Defines the channel layout of the DMX fixture"},
		category { "lx.colorfixture"}, order { 4 },
		getter { MIN_GETTER_FUNCTION {
			return to_atoms(_personality);
		}},
		setter { MIN_FUNCTION {

			lxmax::color_personality personality;

			for(const auto& a : args)
			{
				lxmax::color_personality_element e;
				if (!lxmax::color_personality_element::from_string(a, e))
					return to_atoms(_personality);

				personality.push_back(e);
			}

			_personality = personality;
			
			return to_atoms(_personality);
		}}
	};

	attribute<ui::color> attr_fixture_color { this, "fixture_color", ui::color(1, 1, 1, 1),
		title { "Fixture Color"},
		description { "Sets the color of the fixture"},
		category { "lx.colorfixture"}, order { 5 }
	};
    
	argument<number> arg_channel { this, "channel", "DMX start channel for fixture",
		MIN_ARGUMENT_FUNCTION {
			attr_channel = arg;
		}
	};
    
    argument<number> arg_universe { this, "universe", "DMX universe number",
        MIN_ARGUMENT_FUNCTION {
            attr_universe = arg;
        }
    };

	

	bool write_to_buffer(const lxmax::fixture_patch_info& patch_info, lxmax::universe_buffer_map& buffer_map,
	                     lxmax::universe_updated_list& updated_universes, bool is_force) override
    {
	    if (!is_force && !is_updated())
			return false;
		
	    const lxmax::universe_address universe = patch_info.channel_range.start_universe();
    	
    	updated_universes.push_back(universe);

	    const auto it = buffer_map.find(universe);

    	if (it == std::end(buffer_map))
			return false;

    	lxmax::universe_buffer& buffer = it->second;
		
		{
    		std::lock_guard<std::mutex> lock(_value_mutex);

	    	for (const auto& e : _personality)
	    	{
	    		
	    	}

			int channel = patch_info.channel_range.start_local();

			

	    	clear_updated(); 
		}

    	return true;
    }
};


MIN_EXTERNAL(lx_colorfixture);
