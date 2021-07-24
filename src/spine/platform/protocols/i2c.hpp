#pragma once

namespace spn::platform {

template<typename I2CImp>
class I2C {
public:
    enum class Status { //
        OK = 0,
        DATA_TOO_LONG = 1,
        NACK_ADDR = 2,
        NACK_DATA = 3,
        ERROR = 4,
        TIMEOUT = 5,
        BUSY = 6
    };

public:
    void initialize() { static_cast<I2CImp>(this)->initialize(); };
    uint16_t read(uint8_t* buffer, uint16_t length) { return static_cast<I2CImp>(this)->read(buffer, length); }
    uint8_t read() { return static_cast<I2CImp>(this)->read(); }
    uint16_t write(uint8_t* buffer, uint16_t length, uint8_t address) {
        return static_cast<I2CImp>(this)->write(buffer, length, address);
    }
    I2C::Status status() { return _status; }

protected:
    void set_status(I2C::Status status) { _status = status; }

private:
    Status _status;
};

} // namespace spn::platform