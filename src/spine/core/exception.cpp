#include "spine/core/exception.hpp"

#include "spine/core/debugging.hpp"
#include "spine/core/logging.hpp"
#include "spine/platform/hal.hpp"

#include <memory>

namespace spn::core {

namespace {
std::unique_ptr<ExceptionHandler> _g_handler = nullptr;
}

void set_machine_exception_handler(std::unique_ptr<ExceptionHandler> handler) {
    spn_assert(_g_handler == nullptr);
    _g_handler.swap(handler);
}

ExceptionHandler* machine_exception_handler() { return _g_handler.get(); }

void Exception::throw_error() const {
    if (const auto eh = machine_exception_handler(); eh != nullptr) {
        ERR("%s was thrown: %s", error_type(), _error_msg);
        eh->handle_exception(*this);
    }

    ERR("%s was thrown: %s, halting without handling exception", error_type(), _error_msg);
    HAL::halt("Exception was thrown.");
}
} // namespace spn::core
