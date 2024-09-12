#pragma once

#include "spine/core/debugging.hpp"
#include "spine/platform/hal.hpp"
#include "spine/structure/result.hpp"
#include "spine/structure/vector.hpp"

namespace spn::structure {

template<typename T>
/// Pool of reusable objects using reference counting.
class Pool {
protected:
    using Pointer = std::shared_ptr<T>;

public:
    Pool(size_t size) : _pointers(Vector<Pointer>(size)) {}
    ~Pool() {
        for (size_t i = 0; i < _pointers.size(); ++i)
            depopulate().reset();
        assert(_pointers.empty());
    }
    Pool(const Pool& other) : _pointers(other._pointers), _lookup_index(other._lookup_index) {}
    Pool& operator=(const Pool& other) {
        if (&other == this) return *this;
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

    /// Populate the pool with a raw pointer object.
    void populate(T* obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::shared_ptr<T>(obj));
    }

    /// Populate the pool with an r-value object.
    void populate(T&& obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::make_shared<T>(std::move(obj)));
    }

    /// Populate the pool with a shared_ptr object.
    void populate(std::shared_ptr<T> obj) {
        assert(!_pointers.full());
        _pointers.emplace_back(std::move(obj));
    }

    /// Depopulate a single element from the pool.
    [[nodiscard]] Pointer&& depopulate() {
        assert(!_pointers.empty());
        return _pointers.pop_front();
    }

    /// Returns true if the pool is fully populated.
    bool is_fully_populated() { return _pointers.full(); }

    /// Returns a pointer-object from the pool if available, or a nullptr;
    Pointer acquire() {
        assert(is_fully_populated());
        // Assume that it is most common for memory to be accessed and released in order; look in the back
        const auto skipped = _lookup_index;
        for (; _lookup_index < _pointers.size(); ++_lookup_index) {
            if (_pointers[_lookup_index].use_count() == 1) return _pointers[_lookup_index];
        }
        _lookup_index = 0; // Look in the front
        for (size_t i = 0; i < skipped && i < _pointers.size();) {
            if (_pointers[i].use_count() == 1) return _pointers[i];
            ++i;
        }
        return nullptr;
    };

    /// Return amount of objects in the pool.
    size_t size() const { return _pointers.size(); }

protected:
    Vector<Pointer> _pointers;

private:
    uint16_t _lookup_index = 0;
};

} // namespace spn::structure
