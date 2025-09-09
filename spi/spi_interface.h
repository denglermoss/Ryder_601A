#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

#include <cstdint>
#include <pigpio.h>

class SpiInterface {
public:
    SpiInterface() = default;
    ~SpiInterface();

    bool open(int channel, int speed, int flags = 0);
    void close();

    inline bool write(const uint8_t* buf, int len) const {
        return spiWrite(spi_handle, reinterpret_cast<const char*>(buf), len) == len;
    }

    inline bool read(uint8_t* buf, int len) const {
        return spiRead(spi_handle, reinterpret_cast<char*>(buf), len) == len;
    }

    inline bool transfer(uint8_t* buf, int len) const {
        return spiXfer(spi_handle, reinterpret_cast<char*>(buf), reinterpret_cast<char*>(buf), len) == len;
    }

    bool is_open() const { return spi_handle >= 0; }

private:
    int spi_handle = -1;
};

#endif // SPI_INTERFACE_H
