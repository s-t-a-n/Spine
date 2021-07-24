// #pragma once
//
// #include "spine/core/debugging.hpp"
// #include "spine/structure/vector.hpp"
//
// namespace spn::structure {
//
//
//
//
///// container for storing and retreiving blocks of data
// template<typename T>
// class BlockBuffer {
// public:
//     using Size = unsigned long long;
//
//     BlockBuffer(Size capacity, T init_value = {}) : buffer(capacity, init_value), head(0), tail(0) {}
//
//     bool fits(Size size) const { return size <= this->capacity(); }
//
//     bool has(Size size) const { return size <= this->size(); }
//
//     T* block(Size size) {
//         assert(fits(size));
//         return block_at_head(size);
//     }
//
//     T* get(Size count) {
//         assert(has(count));
//         if (head > tail) {
//             auto data = buffer.begin() + tail;
//             tail += count;
//             assert(tail <= head);
//             return data;
//         } else {
//             // not supported
//             spn_throw("not supported");
//         }
//     }
//
//     void retract(Size count) {
//         assert(head >= count);
//         head -= count;
//     }
//
//     Size size() const {
//         assert(head - tail >= 0);
//         return head - tail;
//     }
//     Size capacity() const {
//         assert(buffer.size() - this->size() >= 0);
//         return buffer.size() - this->size();
//     }
//
// private:
//     bool fits_at_head(Size size) const { return buffer.size() - head >= size; }
//
//     T* block_at_head(Size size) {
//         assert(fits(size));
//
//         if (!fits_at_head(size)) {
//             align_front();
//         }
//
//         const auto head_of_block = head;
//         head += size;
//
//         return buffer.data() + head_of_block;
//     }
//
//     void align_front() {
//         if (head >= tail) {
//             memmove(buffer.data(), buffer.data() + tail, this->size() * sizeof(T));
//             head = this->size();
//             tail = 0;
//         } else {
//             // not supported
//             spn_throw("not supported");
//         }
//     }
//
// private:
//     Array<T> buffer;
//     Size head;
//     Size tail;
// };
//
// } // namespace spn::structure
