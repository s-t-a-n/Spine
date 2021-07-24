// #pragma once
//
// #include "spine/core/debugging.hpp"
// #include "spine/io/stream/stream.hpp"
// #include "spine/platform/hal.hpp"
//
// namespace spn::io {
// class UARTStream final : public Stream {
// public:
//     UARTStream(HAL::UART&& uart) : Stream(), _uart(std::move(uart)){};
//     ~UARTStream() override = default;
//
// public:
//     void initialize() override {}
//
//     size_t available() override { return _uart.available(); }
//     size_t available_for_write() override { return _uart.available_for_write(); }
//     bool write(const uint8_t value) override { return _uart.write(value); }
//     size_t write(const uint8_t* const buffer, size_t length) override { return _uart.write(buffer, length); }
//
//     bool read(uint8_t& value) override {
//         value = _uart.read();
//         return true;
//     }
//     size_t read(uint8_t* buffer, size_t length) override { return _uart.read(buffer, length); }
//
// private:
//     HAL::UART _uart;
// };
//
// } // namespace spn::io