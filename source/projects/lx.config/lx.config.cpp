/// @file
///	@ingroup 	lxmax
///	@copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include "c74_min.h"
#include "common.hpp"

using namespace c74::min;


class lx_config : public object<lx_config> {
    
    c74::max::t_object* _lxmax_service;
    
public:
	MIN_DESCRIPTION	{"Configure the LXMax service."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler / The Impersonal Stereo"};
	MIN_RELATED		{"lx.dimmer, lx.colorfixture"};

    lx_config(const atoms& args = {})
    {
        _lxmax_service = static_cast<c74::max::t_object*>(c74::max::object_findregistered(k_lxmax_namespace, k_lxmax_service_registration));

        assert(_lxmax_service);
    }
    
	inlet<>  input	{ this, "lx.config" };

	outlet<> dump_outlet { this, "dump" };
    
	message<> add_universe { this, "add_universe", "Adds a universe to the global configuration",
		MIN_FUNCTION {
            
            c74::max::t_atom rv {};
            c74::max::object_method_typed(_lxmax_service, symbol("add_universe"), 0, nullptr, &rv);
            
			return {};
		}
	};
    
    message<> remove_universe { this, "remove_universe", "Removes a universe from the global configuration",
        MIN_FUNCTION {
            
            if (args.empty() || args[0].a_type != c74::max::A_LONG)
                return {};

            const c74::max::t_atom_long index = args[0];
            c74::max::t_atom rv {};
            c74::max::object_method_long(_lxmax_service, symbol("remove_universe"), index, &rv);
            return {};
        }
    };
    
    message<> clear_universes { this, "clear_universes", "Removes all universes from the configuration",
        MIN_FUNCTION {
            
            c74::max::t_atom rv {};
            c74::max::object_method_typed(_lxmax_service, symbol("clear_universes"), 0, nullptr, &rv);
            
            return {};
        }
    };

	message<> quick_patch { this, "quick_patch", "Quickly add a number of universes to the configuration",
        MIN_FUNCTION {

			if (args.size() != 6)
				return {};
        	
        	c74::max::t_atom rv {};
			c74::max::t_atom argv[6];

        	for(int i = 0; i < 6; ++i)
        		argv[i] = args[i];
        	
            c74::max::object_method_typed(_lxmax_service, symbol("quick_patch"), 6, argv, &rv);
            
            return {};
        }
    };

	message<> get_local_settings { this, "get_local_settings", "Outputs a dictionary containing local settings for a universe",
        MIN_FUNCTION {
            
			if (args.empty() || args[0].a_type != c74::max::A_LONG)
                return {};

            const c74::max::t_atom_long index = args[0];
            c74::max::t_atom rv {};
            c74::max::object_method_long(_lxmax_service, symbol("get_local_settings"), index, &rv);

            dump_outlet.send({ symbol("local_settings"), index, rv });
        	
            return {};
        }
    };

	message<> get_global_settings { this, "get_global_settings", "Outputs a dictionary containing the current global settings",
        MIN_FUNCTION {
            
			c74::max::t_atom rv {};
            c74::max::object_method_typed(_lxmax_service, symbol("get_global_settings"), 0, nullptr, &rv);

            dump_outlet.send({ symbol("global_settings"), rv });
        	
            return {};
        }
    };
};

MIN_EXTERNAL(lx_config);
