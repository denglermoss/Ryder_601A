#include "measurement.h"
#include "spi_interface.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <pigpio.h>

#define ADS1220_CMD_RESET     0x06
#define ADS1220_CMD_START     0x08
#define ADS1220_CMD_RDATA     0x10
#define ADS1220_CMD_WREG      0x40

#define ADS1220_REG_CONFIG0   0x00
#define ADS1220_REG_CONFIG1   0x01
#define ADS1220_REG_CONFIG2   0x02
#define ADS1220_REG_CONFIG3   0x03

#define DRDY_PIN 23  // GPIO pin for DOUT/DRDY (BCM numbering)

ADS1220::ADS1220() {}

bool ADS1220::init(int channel, int speed, int flags) {
    if (!spi.open(channel, speed, flags)) {
        std::cerr << "Failed to initialize SPI interface for ADS1220\n";
        return false;
    }

    if (gpioSetMode(DRDY_PIN, PI_INPUT) != 0) {
        std::cerr << "Failed to configure DRDY pin\n";
        return false;
    }

    reset();
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Configure CONFIG1–CONFIG3 once for continuous mode
    configureStaticRegisters(0x07, 0x00, 0x00);  // 2000SPS, continuous, internal ref
    return true;
}

void ADS1220::reset() {
    char cmd = ADS1220_CMD_RESET;
    spi.write(&cmd, 1);
}

int32_t ADS1220::readData() {
    char cmd = ADS1220_CMD_RDATA;
    char buf[3] = {0};
    spi.write(&cmd, 1);
    if (!spi.read(buf, 3)) {
        std::cerr << "Failed to read from ADS1220\n";
        return -1;
    }
    return convert24bit(buf);
}

void ADS1220::configureMuxOnly(char conf0) {
    char config[] = {
        static_cast<char>(ADS1220_CMD_WREG | ADS1220_REG_CONFIG0),
        conf0
    };
    spi.write(config, sizeof(config));
}

void ADS1220::configureStaticRegisters(char conf1, char conf2, char conf3) {
    char config[] = {
        static_cast<char>(ADS1220_CMD_WREG | ADS1220_REG_CONFIG1 | 0x02),
        conf1, conf2, conf3
    };
    spi.write(config, sizeof(config));
}

int32_t ADS1220::readAIN0() {
    configureMuxOnly(0x08); // AIN0 vs AVSS
    wait_for_drdy();
    return readData();
}

int32_t ADS1220::readAIN3() {
    configureMuxOnly(0x0B); // AIN3 vs AVSS
    wait_for_drdy();
    return readData();
}

void ADS1220::wait_for_drdy() const {
    while (gpioRead(DRDY_PIN) == 1) {
        std::this_thread::sleep_for(std::chrono::microseconds(5));
    }
}

int32_t ADS1220::convert24bit(const char* data) const {
    int32_t result = (static_cast<int32_t>(data[0]) << 16) |
                     (static_cast<int32_t>(data[1]) << 8)  |
                     (static_cast<int32_t>(data[2]));
    if (result & 0x800000) result |= 0xFF000000;  // Sign extend
    return result;
}
