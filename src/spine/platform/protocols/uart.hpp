#pragma once

#include "spine/io/stream/stream.hpp"

#if not defined(SERIAL_TX_BUFFER_SIZE)
#    define SERIAL_TX_BUFFER_SIZE 256 // Per standard, use an RX buffer of 256 bytes
#endif

#if not defined(SERIAL_RX_BUFFER_SIZE)
#    define SERIAL_RX_BUFFER_SIZE 256 // Per standard, use an TX buffer of 256 bytes
#endif

namespace spn::platform {

template<typename UARTImp>
/// Abstract protocol for UART.
class UART : public io::Stream {
    // todo: is this pointless? https://github.com/s-t-a-n/Spine/issues/22
public:
    enum class Status : uint8_t { OK, DATA_TOO_LONG, ERROR, TIMEOUT, BUSY };

    /// Returns the UART's communication status
    UART::Status status() { return _status; }

protected:
    void set_status(UART::Status status) { _status = status; }

private:
    Status _status = Status::OK;
};

} // namespace spn::platform