#pragma once

#include <memory>

namespace spn::core {
class Exception;

struct ExceptionHandler {
    virtual ~ExceptionHandler(){};
    virtual void handle_exception(const Exception& exception){};
};

/// Set a machine global exception handler which will be called when an exception is thrown
void set_machine_exception_handler(std::unique_ptr<ExceptionHandler> handler);

/// Returns global exception handler
ExceptionHandler* machine_exception_handler();

class Exception {
public:
    Exception(const char* error_msg) : m_error_msg(error_msg){};
    Exception(const Exception& other) : m_error_msg(other.m_error_msg) {}
    virtual ~Exception() = default;

    void throw_error() const;
    virtual const char* error_type() const { return "Exception"; };

protected:
    const char* m_error_msg;
};
} // namespace spn::core

namespace spn {

/// Exception for problems that can only be discovered during runtime
struct runtime_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Runtime exception"; }
};

/// Exception for problems that should have been discovered at compile time
struct logic_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Logic exception"; }
};

/// Exception that will be thrown by assertions
struct assertion_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Assertion exception"; }
};

/// Throws an exception by calling the global exception handler
inline void throw_exception(const spn::core::Exception& exception) { exception.throw_error(); }
} // namespace spn
