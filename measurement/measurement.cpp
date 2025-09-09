#include "measurement.h"
#include "spi/spi_interface.h"
#include <chrono>
#include <iostream>

namespace {
    SpiInterface spi;
    std::chrono::steady_clock::time_point start_time;
}

uint16_t Measurement::readADC() {
    uint8_t buf[2] = {0x00, 0x00};
    if (!spi.transfer(buf, 2)) {
        std::cerr << "SPI read failed!" << std::endl;
        return static_cast<uint16_t>(-1);
    }
    return ((static_cast<uint16_t>(buf[0]) << 8) | buf[1]) & 0x0FFF;
}

void Measurement::init() {
    if (!spi.open(ADC_CHANNEL, SPI_SPEED, 1)) {
        std::cerr << "ADC SPI Setup failed!" << std::endl;
        return;
    }
    start_time = std::chrono::steady_clock::now();
}

static float calc_passed_time() {
    std::chrono::duration<float> elapsed = std::chrono::steady_clock::now() - start_time;
    return elapsed.count();
}

data_point Measurement::get_data_point() {
    float adc_value = readADC();
    return data_point(calc_passed_time(), ADC_TO_VOLTS * adc_value);
}
