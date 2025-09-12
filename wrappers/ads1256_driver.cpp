#include "ads1256_driver.h"
#include <cstdio>

extern "C" {
#include "drivers/DEV_Config.h"
#include "drivers/ADS1256.h"
}

namespace ADS1256Driver {

bool init() {
    if (DEV_ModuleInit() != 0) {
        std::fprintf(stderr, "DEV_ModuleInit failed\n");
        return false;
    }
    if (ADS1256_init() != 0) {
        std::fprintf(stderr, "ADS1256_init failed\n");
        return false;
    }
    return true;
}

int32_t read_channel(int channel) {
    if (channel < 0 || channel > 7) return -1;
    // ADS1256_GetChannalValue returns UDOUBLE (uint32_t) representing signed 24-bit.
    UDOUBLE v = ADS1256_GetChannalValue((UBYTE)channel);
    // Interpret as signed 24-bit value
    int32_t s = (int32_t)v;
    if (s & 0x800000) s |= 0xFF000000;
    return s;
}

bool read_all(int32_t* out8) {
    if (!out8) return false;
    UDOUBLE vals[8] = {0};
    ADS1256_GetAll(vals);
    for (int i = 0; i < 8; ++i) {
        int32_t s = (int32_t)vals[i];
        if (s & 0x800000) s |= 0xFF000000;
        out8[i] = s;
    }
    return true;
}

void shutdown() {
    DEV_ModuleExit();
}

} // namespace ADS1256Driver
