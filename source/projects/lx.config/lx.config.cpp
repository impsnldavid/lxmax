;/// @file
///	@ingroup 	lxmax
///	@copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include <Poco/Net/NetworkInterface.h>

#include "version_info.hpp"
#include "c74_min.h"
#include "common.hpp"

using namespace c74;
using namespace min;


class lx_config : public object<lx_config>
{
	max::t_object* _lxmax_service;

public:
	MIN_DESCRIPTION { "Configure the LXMax service." };
	MIN_TAGS { "lxmax" };
	MIN_AUTHOR { "David Butler / The Impersonal Stereo" };
	MIN_RELATED { "lx.dimmer, lx.colorfixture" };

	lx_config(const atoms& args = { })
	{
		if (!maxobj())
			return;
		
		_lxmax_service = get_lxmax_service_and_check_version(maxobj(), lxmax::GIT_VERSION_STR);
	}

	inlet<> input { this, "lx.config" };

	outlet<> dump_outlet { this, "dump" };

	message<> add_universe {
		this, "add_universe", "Adds a universe to the global configuration",
		MIN_FUNCTION
		{
			max::t_atom rv { };
			object_method_typed(_lxmax_service, symbol("add_universe"), 0, nullptr, &rv);

			return { };
		}
	};

	message<> remove_universe {
		this, "remove_universe", "Removes a universe from the global configuration",
		MIN_FUNCTION
		{
			if (args.empty() || args[0].type() != message_type::int_argument)
				return { };

			const max::t_atom_long index = args[0];
			max::t_atom rv { };
			object_method_long(_lxmax_service, symbol("remove_universe"), index, &rv);
			return { };
		}
	};

	message<> clear_universes {
		this, "clear_universes", "Removes all universes from the configuration",
		MIN_FUNCTION
		{
			max::t_atom rv { };
			object_method_typed(_lxmax_service, symbol("clear_universes"), 0, nullptr, &rv);

			return { };
		}
	};

	message<> quick_patch {
		this, "quick_patch", "Quickly add a number of universes to the configuration",
		MIN_FUNCTION
		{
			if (args.size() != 6)
				return { };

			max::t_atom rv { };
			max::t_atom argv[6];

			for (int i = 0; i < 6; ++i)
				argv[i] = args[i];

			object_method_typed(_lxmax_service, symbol("quick_patch"), 6, argv, &rv);

			return { };
		}
	};

	message<> get_global_preferences {
		this, "get_global_preferences", "Outputs a dictionary containing the current global preferences",
		MIN_FUNCTION
		{
			max::t_atom rv;
			max::object_method_typed(_lxmax_service, symbol("get_global_preferences"), 0, nullptr, &rv);

			max::t_dictionary* d = static_cast<max::t_dictionary*>(max::atom_getobj(&rv));

			max::t_symbol* name = nullptr;
			dictobj_register(d, &name);

			dump_outlet.send({ symbol("global_preferences"), symbol("dictionary"), name });

			return { };
		}
	};

	message<> set_global_preferences {
		this, "set_global_preferences", "Sets the current global preferences to values from a dictionary",
		MIN_FUNCTION
		{
			if (args.size() != 2
				|| args[0].type() != message_type::symbol_argument || args[1].type() != message_type::symbol_argument
				|| args[0] != symbol("dictionary"))
			{
				return { };
			}

			max::t_atom argv;
			max::atom_setsym(&argv, args[1]);

			max::t_atom rv;
			max::object_method_typed(_lxmax_service, symbol("set_global_preferences"), 1, &argv, &rv);

			return { };
		}
	};

	message<> get_universe_preferences {
		this, "get_universe_preferences", "Gets a dictionary containing a specified universe's preferences",
		MIN_FUNCTION
		{
			if (args.empty() || args[0].type() != message_type::int_argument)
				return { };

			max::t_atom index;
			max::atom_setlong(&index, args[0]);

			max::t_atom rv;
			max::object_method_typed(_lxmax_service, symbol("get_universe_preferences"), 1, &index, &rv);

			max::t_dictionary* d = static_cast<max::t_dictionary*>(max::atom_getobj(&rv));

			max::t_symbol* name = nullptr;
			dictobj_register(d, &name);

			dump_outlet.send({ symbol("universe_preferences"), symbol("dictionary"), name });

			return { };
		}
	};

	message<> set_universe_preferences {
		this, "set_universe_preferences", "Sets the preferences for a specified universe to values from a dictionary",
		MIN_FUNCTION
		{
			if (args.size() != 3
				|| args[0].type() != message_type::int_argument || args[1].type() != message_type::symbol_argument
				|| args[2].type() != message_type::symbol_argument
				|| args[1] != symbol("dictionary"))
			{
				return { };
			}

			max::t_atom argv[2];
			max::atom_setlong(argv, args[0]);
			max::atom_setsym(argv + 1, args[2]);

			max::t_atom rv;
			max::object_method_typed(_lxmax_service, symbol("set_universe_preferences"), 2, argv, &rv);

			return { };
		}
	};

	message<> get_network_adapters {
		this, "get_network_adapters", "Outputs a dictionary containing the available network adapters",
		MIN_FUNCTION
		{
			max::t_atom rv;
			max::object_method_typed(_lxmax_service, symbol("get_network_adapters"), 0, nullptr, &rv);

			max::t_dictionary* d = static_cast<max::t_dictionary*>(max::atom_getobj(&rv));

			max::t_symbol* name = nullptr;
			dictobj_register(d, &name);

			dump_outlet.send({ symbol("network_adapters"), symbol("dictionary"), name });

			return { };
		}
	};
};

MIN_EXTERNAL(lx_config);
