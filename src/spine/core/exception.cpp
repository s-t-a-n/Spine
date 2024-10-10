#include "spine/core/exception.hpp"

#include "spine/core/debugging.hpp"
#include "spine/core/logging.hpp"
#include "spine/platform/hal.hpp"

#include <memory>

namespace spn::core {

namespace {
std::unique_ptr<ExceptionHandler>& static_eh() {
    static std::unique_ptr<ExceptionHandler> handler = nullptr;
    return handler;
}
} // namespace

void set_machine_exception_handler(std::unique_ptr<ExceptionHandler> handler) {
    if (static_eh() != nullptr) SPN_DBG("Replacing machine wide exception handler!");
    static_eh().swap(handler);
}

ExceptionHandler* machine_exception_handler() { return static_eh().get(); }

void Exception::throw_error() const {
    if (const auto eh = machine_exception_handler(); eh != nullptr) {
        SPN_DBG("%s was thrown: %s", error_type(), _error_msg);
        eh->handle_exception(*this);
    }

    SPN_WARN("%s was thrown: %s, halting without handling exception", error_type(), _error_msg);
    HAL::halt("Exception was thrown.");
}
} // namespace spn::core
