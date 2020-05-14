/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#include "fixture.hpp"

#include <utility>
#include "fixture_manager.hpp"

namespace lxmax
{
	void fixture::set_patch_info(fixture_patch_info info)
	{
		if (!_manager || _patch_info == info)
			return;

		_patch_info = std::move(info);

		if (_patch_info.channel_range.is_empty())
			_manager->unregister_fixture(this);
		else
			_manager->register_fixture(this, _patch_info);
	}

	fixture::~fixture()
	{
		if (_manager)
			_manager->unregister_fixture(this);
	}

	void fixture::set_manager(std::shared_ptr<fixture_manager> manager)
	{
		_manager = std::move(manager);
		
		if (!_patch_info.channel_range.is_empty())
			_manager->register_fixture(this, _patch_info);
	}

	void fixture::set_updated()
	{
		_is_updated = true;
		_last_updated = clock::now();
	}
}
