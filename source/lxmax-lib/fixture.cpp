/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "fixture.hpp"
#include "fixture_manager.hpp"

namespace lxmax
{
	fixture::~fixture()
	{
		if (_manager)
			_manager->unregister_fixture(this);
	}

	void fixture::set_manager(std::shared_ptr<fixture_manager> manager, const fixture_patch_info& patch_info)
	{
		if (_manager)
			_manager->unregister_fixture(this);

		_manager = std::move(manager);

		if (_manager)
			_manager->register_fixture(this, patch_info);
	}

	void fixture::set_updated()
	{
		_is_updated = true;
		_last_updated = clock::now();
	}
}
