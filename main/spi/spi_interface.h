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

    inline bool write(char* buf, int len) const {
        return spiWrite(spi_handle, buf, len) == len;
    }

    inline bool read(char* buf, int len) const {
        return spiRead(spi_handle, buf, len) == len;
    }

    inline bool transfer(char* buf, int len) const {
        return spiXfer(spi_handle, buf, buf, len) == len;
    }

    bool is_open() const { return spi_handle >= 0; }

private:
    int spi_handle = -1;
};

#endif // SPI_INTERFACE_H
