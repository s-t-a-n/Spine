#pragma once

#include "spine/io/stream/stream.hpp"

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