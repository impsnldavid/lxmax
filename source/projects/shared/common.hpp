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

inline c74::max::t_object* get_lxmax_service()
{
	c74::max::t_object* lxmax_service_obj = static_cast<c74::max::t_object*>(c74::max::object_findregistered(
		c74::min::symbol("nobox"), k_lxmax_service_registration));
    assert(lxmax_service_obj);

	return lxmax_service_obj;
}

inline lxmax::fixture_manager* get_fixture_manager(c74::max::t_object* lxmax_service_obj)
{
	assert(lxmax_service_obj);

	c74::max::t_atom rv;
	c74::max::object_method_typed(lxmax_service_obj, c74::max::gensym("get_fixture_manager"), 0, nullptr, &rv);

	lxmax::fixture_manager* fixture_manager = static_cast<lxmax::fixture_manager*>(c74::max::atom_getobj(&rv));
	assert(fixture_manager);

	return fixture_manager;
}