// ads1220.h
#ifndef ADS1220_H
#define ADS1220_H

#include <cstdint>
#include <utility>
#include "spi/spi_interface.h"

class ADS1220 {
public:
    ADS1220();
    bool init(int channel, int speed, int flags = 1);
    void reset();
    int32_t readData();
    void configureMuxOnly(char conf0);
    void configureStaticRegisters(char conf1, char conf2, char conf3);
    int32_t readAIN0();
    int32_t readAIN3();

private:
    SpiInterface spi;
    void wait_for_drdy() const;
    int32_t convert24bit(const char* data) const;
};

#endif // ADS1220_H
