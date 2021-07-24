#pragma once

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/result.hpp"
#include "spine/structure/vector.hpp"

namespace spn::structure {

// sliced memory pool
// useful for safely passing stuff around without reallocation
template<typename T>
class Pool {
protected:
    using Pointer = std::shared_ptr<T>;

public:
    Pool(size_t size) : _pointers(Vector<Pointer>(size)) {}

    ~Pool() {
        DBG("Destroying pool");
        for (size_t i = 0; i < _pointers.size(); ++i) {
            depopulate().reset();
        }
        assert(_pointers.empty());
    }

    Pool(const Pool& other) : _pointers(other._pointers), _lookup_index(other._lookup_index) {}
    Pool& operator=(const Pool& other) {
        if (&other == this) {
            return *this;
        }
        _pointers = other._pointers;
        _lookup_index = other._lookup_index;
        return *this;
    }
    Pool& operator=(Pool&& other) noexcept {
        if (&other == this) return *this;
        _pointers = std::move(other._pointers);
        _lookup_index = other._lookup_index;
        return *this;
    }

    void populate(T* obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::shared_ptr<T>(obj));
    }

    void populate(T&& obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::make_shared<T>(std::move(obj)));
    }

    void populate(std::shared_ptr<T> obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::move(obj));
    }

    [[nodiscard]] Pointer&& depopulate() {
        assert(!_pointers.empty());
        return std::move(_pointers.pop_front());
    }

    bool is_fully_populated() { return _pointers.full(); }

    Pointer acquire() {
        assert(is_fully_populated());
        // try first to follow previous access order
        // we assume that it is most common for memory to be accessed and released in order

        // paranoia
        // Serial.print("Free memory: ");
        // Serial.println(HAL::free_memory());
        // HAL::print(F("Acquire ("));
        // HAL::print(_pointers.size());
        // HAL::print(F("): "));
        for (ArrayBase::Size i = 0; i < _pointers.size(); ++i) {
            const auto use_count = _pointers[i].use_count();
            // HAL::print(use_count);
            if (use_count == 0) {
                assert(_pointers[i]);
                assert(_pointers[i].get());
            }
        }
        // HAL::println("");

        const auto skipped = _lookup_index;
        for (; _lookup_index < _pointers.size(); ++_lookup_index) {
            if (_pointers[_lookup_index].use_count() == 1) {
                return _pointers[_lookup_index];
            }
        }
        _lookup_index = 0;

        // miss, loop until where previous loop started
        for (size_t i = 0; i < skipped && i < _pointers.size(); ++i) {
            if (_pointers[i].use_count() == 1) {
                return _pointers[i];
            }
        }
        return nullptr;
    };

    // todo: integrate these two into caching function
    bool can_acquire() {
        const auto skipped = _lookup_index;
        for (auto i = _lookup_index; i < _pointers.size(); i++) {
            if (_pointers[i].use_count() == 1) {
                return true;
            }
        }
        for (size_t i = 0; i < skipped && i < _pointers.size(); i++) {
            if (_pointers[i].use_count() == 1) {
                return true;
            }
        }
        return false;
    }

    size_t size() const { return _pointers.size(); }

protected:
    Vector<Pointer> _pointers;

private:
    uint16_t _lookup_index = 0;
};

} // namespace spn::structure
