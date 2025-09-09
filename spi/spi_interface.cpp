#include "spi_interface.h"
#include <iostream>

bool SpiInterface::open(int channel, int speed, int flags) {
    spi_handle = spiOpen(channel, speed, flags);
    if (spi_handle < 0) {
        std::cerr << "SPI open failed on channel " << channel << "\n";
        return false;
    }
    return true;
}

void SpiInterface::close() {
    if (spi_handle >= 0) {
        spiClose(spi_handle);
        spi_handle = -1;
    }
}

SpiInterface::~SpiInterface() {
    close();
}