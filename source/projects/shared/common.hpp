/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include "c74_min.h"
#include "c74_max.h"

namespace lxmax
{
	class fixture_manager;
}

const c74::min::symbol k_lxmax_service_registration { "___lxmax_service" };

inline c74::min::instance get_lxmax_service(c74::min::object_base& x)
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

inline bool version_check(c74::min::object_base& x, c74::min::instance& lxmax_service, const std::string& this_assembly_version)
{
	const c74::min::atom version = lxmax_service(c74::min::symbol("get_version"), 0);

	assert(version.type() == c74::min::message_type::symbol_argument);

	if(version != c74::min::symbol(this_assembly_version))
	{
		assert(false);
		c74::max::object_error(x, 
			"Object (%s) has mismatching version number with currently running LXMax service (%s). Please delete the LXMax package and reinstall.",
			 std::string(version).c_str(), this_assembly_version.c_str());
		return false;
	}
	else
	{
		return true;
	}
}

inline c74::min::instance get_lxmax_service_and_check_version(c74::min::object_base& x, const std::string& this_assembly_version)
{
	c74::min::instance lxmax_service = get_lxmax_service(x);

	if (version_check(x, lxmax_service, this_assembly_version))
		return lxmax_service;
	else
		return nullptr;
}

inline std::shared_ptr<lxmax::fixture_manager> get_fixture_manager(c74::min::object_base& x, c74::min::instance& lxmax_service)
{
	assert(lxmax_service);

	void* value = lxmax_service(c74::min::symbol("get_fixture_manager"), 0);

	std::shared_ptr<lxmax::fixture_manager>* fixture_manager_ptr = static_cast<std::shared_ptr<lxmax::fixture_manager>*>(value);
	if (fixture_manager_ptr == nullptr)
	{
		assert(false);
		c74::max::object_error(x,"Failed to get reference to LXMax fixture manager. Please delete the LXMax package and reinstall.");
	}

	return *fixture_manager_ptr;
}