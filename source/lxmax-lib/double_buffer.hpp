/// @file
/// @ingroup	lxmax
/// @copyright	Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license	Use of this source code is governed by the MIT License found in the License.md file.

#pragma once

#include <mutex>
#include <vector>

namespace lxmax
{
	template <typename T>
	class double_buffer
	{
		mutable std::mutex _resize_mutex;
		mutable std::mutex _access_mutex;

		std::vector<T> _buffer_a;
		std::vector<T> _buffer_b;

		T* _current;
		T* _next;

	public:
		double_buffer(size_t size)
			: _buffer_a(size, 0),
			  _buffer_b(size, 0),
			  _current(&_buffer_a),
			  _next(&_buffer_b)
		{
		}

		size_t size() const
		{
			return _buffer_a.size();
		}

		void resize(size_t size, T initial_value)
		{
			const size_t old_size = _buffer_a.size();
			
			std::lock_guard<std::mutex> resize_lock(_resize_mutex);
			std::lock_guard<std::mutex> access_lock(_access_mutex);
			_buffer_a.resize(size);
			_buffer_b.resize(size);

			if (size > old_size)
            {
                for (size_t i = old_size; i < size; ++i)
                {
	                _buffer_a[i] = initial_value;
	                _buffer_b[i] = initial_value;
                }
            }
		}

		std::vector<T> get()
		{
			std::lock_guard<std::mutex> lock(_access_mutex);
			return *_current;
		}

		void copy_and_swap(const std::vector<T>& data)
		{
			std::lock_guard<std::mutex> resize_lock(_resize_mutex);
			
			const size_t min_size = std::min(data.size(), _buffer_a.size());
			for(size_t i = 0; i < min_size; ++i)
				_next[i] = data[i];

			{
				std::lock_guard<std::mutex> access_lock(_access_mutex);
				std::swap(_next, _current);
			}
		}

		void set_all_and_swap(T value)
		{
			std::lock_guard<std::mutex> resize_lock(_resize_mutex);
			
			for(size_t i = 0; i < _buffer_a.size(); ++i)
				_next[i] = value;

			{
				std::lock_guard<std::mutex> access_lock(_access_mutex);
				std::swap(_next, _current);
			}
		}
	};
}
