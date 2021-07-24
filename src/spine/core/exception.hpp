#pragma once

#include <memory>

namespace spn::core {
class Exception;

struct ExceptionHandler {
    virtual ~ExceptionHandler(){};
    virtual void handle_exception(const Exception& exception){};
};

/// set a machine global exception handler, that will be called when an exception is thrown
void set_machine_exception_handler(std::unique_ptr<ExceptionHandler> handler);
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

struct runtime_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Runtime exception"; }
};

struct logic_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Logic exception"; }
};

struct assertion_exception : core::Exception {
    using Exception::Exception;
    const char* error_type() const override { return "Assertion exception"; }
};

inline void throw_exception(const spn::core::Exception& exception) { exception.throw_error(); }
} // namespace spn
