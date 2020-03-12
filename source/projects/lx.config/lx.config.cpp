/// @file
///	@ingroup 	lxmax
///	@copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include <Poco/Net/NetworkInterface.h>

#include "c74_min.h"
#include "common.hpp"

using namespace c74;
using namespace min;


class lx_config : public object<lx_config> {
    
    max::t_object* _lxmax_service;
    
public:
	MIN_DESCRIPTION	{"Configure the LXMax service."};
	MIN_TAGS		{"lxmax"};
	MIN_AUTHOR		{"David Butler / The Impersonal Stereo"};
	MIN_RELATED		{"lx.dimmer, lx.colorfixture"};

    lx_config(const atoms& args = {})
    {
		_lxmax_service = get_lxmax_service();
    }
    
	inlet<>  input	{ this, "lx.config" };

	outlet<> dump_outlet { this, "dump" };
    
	message<> add_universe { this, "add_universe", "Adds a universe to the global configuration", message_type::no_argument,
		MIN_FUNCTION {
            
            max::t_atom rv {};
            object_method_typed(_lxmax_service, symbol("add_universe"), 0, nullptr, &rv);
            
			return {};
		}
	};
    
    message<> remove_universe { this, "remove_universe", "Removes a universe from the global configuration", message_type::int_argument,
        MIN_FUNCTION {
            
            const max::t_atom_long index = args[0];
            max::t_atom rv {};
            object_method_long(_lxmax_service, symbol("remove_universe"), index, &rv);
            return {};
        }
    };
    
    message<> clear_universes { this, "clear_universes", "Removes all universes from the configuration", message_type::no_argument,
        MIN_FUNCTION {
            
            max::t_atom rv {};
            object_method_typed(_lxmax_service, symbol("clear_universes"), 0, nullptr, &rv);
            
            return {};
        }
    };

	message<> quick_patch { this, "quick_patch", "Quickly add a number of universes to the configuration", 
        MIN_FUNCTION {

			if (args.size() != 6)
				return {};
        	
        	max::t_atom rv {};
			max::t_atom argv[6];

        	for(int i = 0; i < 6; ++i)
        		argv[i] = args[i];
        	
            object_method_typed(_lxmax_service, symbol("quick_patch"), 6, argv, &rv);
            
            return {};
        }
    };

	message<> get_global_preferences { this, "get_global_preferences", "Outputs a dictionary containing the current global preferences",
        MIN_FUNCTION {
            
            max::t_atom rv; 
            max::object_method_typed(_lxmax_service, symbol("get_global_preferences"), 0, nullptr, &rv);

			max::t_dictionary* d = static_cast<max::t_dictionary*>(max::atom_getobj(&rv));

        	max::t_symbol* name = nullptr;
        	dictobj_register(d, &name);
        	
            dump_outlet.send({ symbol("global_preferences"), symbol("dictionary"), name });
        	
            return {};
        }
    };

	message<> get_local_settings { this, "get_universe_preferences", "Gets a dictionary containing a specified universe's preferences",
		MIN_FUNCTION {
            
			if (args.empty() || args[0].a_type != max::A_LONG)
                return {};

            max::t_atom index;
			max::atom_setlong(&index, args[0]);

			max::t_atom rv; 
            max::object_method_typed(_lxmax_service, symbol("get_universe_preferences"), 1, &index, &rv);

			max::t_dictionary* d = static_cast<max::t_dictionary*>(max::atom_getobj(&rv));

        	max::t_symbol* name = nullptr;
        	dictobj_register(d, &name);
        	
            dump_outlet.send({ symbol("global_preferences"), symbol("dictionary"), name });
        	
            return {};
        }
    };

	message<> get_network_adapters { this, "get_network_adapters", "Outputs a dictionary containing the currently available network adapters", message_type::no_argument,
        MIN_FUNCTION {
            
			const auto list = Poco::Net::NetworkInterface::list();

        	dict network_adapters(symbol(true));

        	for(const auto& n : list)
            {
                Poco::Net::IPAddress address;
                n.firstAddress(address);
        		
                dict adapter;

        		adapter["name"] = n.displayName();
        		adapter["address"] = address.toString();

                max::dictionary_appenddictionary(network_adapters, symbol(static_cast<int>(n.index())), adapter);
            }

        	dump_outlet.send( { symbol("network_adapters"), symbol("dictionary"), network_adapters.name() });
        	
            return { };
        }
    };
};

MIN_EXTERNAL(lx_config);
