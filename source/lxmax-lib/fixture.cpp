/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2020 David Butler. All rights reserved.
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

	void fixture::set_manager(std::shared_ptr<fixture_manager> manager)
	{
		if (_manager && !_patch_info.channel_range.is_empty())
			unregister_fixture();
		
		_manager = std::move(manager);

		if (_manager && !_patch_info.channel_range.is_empty())
			register_fixture(_patch_info);
	}

	void fixture::register_fixture(const fixture_patch_info& patch_info)
	{
		if (_manager && !patch_info.channel_range.is_empty())
		{
			_patch_info = patch_info;
			_manager->register_fixture(this, _patch_info);
		}
	}

	void fixture::unregister_fixture()
	{
		if (_manager && !_patch_info.channel_range.is_empty())
		{
			_patch_info = fixture_patch_info();
			_manager->unregister_fixture(this);
		}
	}

	void fixture::set_updated()
	{
		_is_updated = true;
		_last_updated = clock::now();
	}
}
