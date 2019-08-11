/// @file
/// @ingroup     lxmax
/// @copyright    Copyright 2019 David Butler / The Impersonal Stereo. All rights reserved.
/// @license    Use of this source code is governed by the MIT License found in the License.md file.

#include <mutex>
#include <vector>

namespace lxmax
{
    template<typename T>
    class double_buffer
    {
        std::mutex _mutex;
        
        std::vector<T> _buffer_a;
        std::vector<T> _buffer_b;
        
        T* _current;
        T* _next;
        
    public:
        double_buffer(size_t size)
            : _buffer_a(size, 0),
            _buffer_b(size, 0),
            _current(_buffer_a.data()),
            _next(_buffer_b.data())
        {
            
        }
        
        void write(T* data, size_t length)
        {
            std::lock_guard<std::mutex> lock(_mutex);
        }
    };
}
