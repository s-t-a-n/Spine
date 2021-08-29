#pragma once

#include <core/standard.hpp>
#include <core/system.hpp>

namespace spn::core {

class Exception {
public:
    Exception(const char* error_msg) : m_error_msg(error_msg){};
    Exception(const Exception& other) : m_error_msg(other.m_error_msg) {}

    void throw_error() const {
        char msg[128];
        snprintf(msg, sizeof(msg), "Exception was throw: %s, %s", error_type(), m_error_msg);
        debug_print(__FILE__, __LINE__, __func__, msg);
    }
    virtual const char* error_type() const { return "Exception"; };

protected:
    const char* m_error_msg;
};

struct runtime_error : public Exception {
    using Exception::Exception;
    virtual const char* error_type() const override { return "Runtime error"; }
};

struct logic_error : public Exception {
    using Exception::Exception;
    virtual const char* error_type() const override { return "Logic error"; }
};

} // namespace spn::core

#ifdef DEBUG
inline void spn_throw(const spn::core::Exception& exception) { exception.throw_error(); }
#else
inline void spn_throw(const spn::core::Exception&) {}
#endif