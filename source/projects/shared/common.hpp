/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "c74_min.h"
#include "c74_max.h"

namespace lxmax
{
	class fixture_manager;
}

const c74::min::symbol k_lxmax_service_registration { "___lxmax_service" };

inline c74::max::t_object* get_lxmax_service(c74::max::t_object* x)
{
	c74::max::t_object* lxmax_service_obj = static_cast<c74::max::t_object*>(c74::max::object_findregistered(
		c74::min::symbol("nobox"), k_lxmax_service_registration));

	if (lxmax_service_obj == nullptr)
	{
		assert(false);
		c74::max::object_error(x,"Failed to get reference to LXMax service. Please delete the LXMax package and reinstall.");
	}
   

	return lxmax_service_obj;
}

inline bool version_check(c74::max::t_object* x, c74::max::t_object* lxmax_service_obj, const std::string& this_assembly_version)
{
	c74::max::t_atom rv;
	c74::max::object_method_typed(lxmax_service_obj, c74::max::gensym("get_version"), 0, nullptr, &rv);

	assert(c74::max::atom_gettype(&rv) == c74::max::A_SYM);

	if(c74::max::atom_getsym(&rv) != c74::max::gensym(this_assembly_version.c_str()))
	{
		assert(false);
		c74::max::object_error(x, 
			"Object (%s) has mismatching version number with currently running LXMax service (%s). Please delete the LXMax package and reinstall.",
			 c74::max::atom_getsym(&rv)->s_name, this_assembly_version.c_str());
		return false;
	}
	else
	{
		return true;
	}
}

inline c74::max::t_object* get_lxmax_service_and_check_version(c74::max::t_object* x, const std::string& this_assembly_version)
{
	c74::max::t_object* lxmax_service_obj = get_lxmax_service(x);

	if (version_check(x, lxmax_service_obj, this_assembly_version))
		return lxmax_service_obj;
	else
		return nullptr;
}

inline lxmax::fixture_manager* get_fixture_manager(c74::max::t_object* x, c74::max::t_object* lxmax_service_obj)
{
	assert(lxmax_service_obj);

	c74::max::t_atom rv;
	c74::max::object_method_typed(lxmax_service_obj, c74::max::gensym("get_fixture_manager"), 0, nullptr, &rv);

	lxmax::fixture_manager* fixture_manager = static_cast<lxmax::fixture_manager*>(c74::max::atom_getobj(&rv));
	if (fixture_manager == nullptr)
	{
		assert(false);
		c74::max::object_error(x,"Failed to get reference to LXMax fixture manager. Please delete the LXMax package and reinstall.");
	}

	return fixture_manager;
}