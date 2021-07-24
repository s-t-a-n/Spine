#pragma once

#include "spine/io/stream/stream.hpp"

namespace spn::platform {

template<typename UARTImp>
class UART : public io::Stream {
public:
    enum class Status : uint8_t { OK, DATA_TOO_LONG, ERROR, TIMEOUT, BUSY };

public:
    // void initialize() override { static_cast<UARTImp>(this)->initialize(); };
    //
    // size_t available() const override { return static_cast<UARTImp>(this)->available(); }
    // size_t available_for_write() const override { return static_cast<UARTImp>(this)->available_for_write(); }
    // void flush() override { static_cast<UARTImp>(this)->flush(); }
    //
    // size_t read(uint8_t* const buffer, size_t length) override {
    //     return static_cast<UARTImp>(this)->read(buffer, length);
    // }
    // bool read(uint8_t& value) override { return static_cast<UARTImp>(this)->read(value); }
    //
    // size_t write(const uint8_t* const buffer, size_t length) override {
    //     return static_cast<UARTImp>(this)->write(buffer, length);
    // }
    // bool write(uint8_t value) override { return static_cast<UARTImp>(this)->write(value); }

    UART::Status status() { return _status; }

protected:
    void set_status(UART::Status status) { _status = status; }

private:
    Status _status = Status::OK;
};

} // namespace spn::platform