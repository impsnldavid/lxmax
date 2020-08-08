/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "lxmax.hpp"
#include "lx.config.hpp"
#include "lx.colorfixture.hpp"
#include "lx.dimmer.hpp"
#include "lx.raw.write.hpp"

#include "c74_min.h"

void ext_main(void* r)
{
	wrap_lxmax_service(r);
	wrap_lx_colorfixture(r);
	wrap_lx_config(r);
	wrap_lx_dimmer(r);
	wrap_lx_raw_write(r);
}

