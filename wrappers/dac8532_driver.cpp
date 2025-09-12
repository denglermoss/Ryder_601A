#include "dac8532_driver.h"

extern "C" {
#include "drivers/DEV_Config.h"
#include "drivers/DAC8532.h"
}

namespace DAC8532Driver {

bool init() {
    // DEV already initialized by ADS1256Driver::init; nothing to do here.
    return true;
}

bool writeA(float volts) {
    DAC8532_Out_Voltage(channel_A, volts);
    return true;
}

bool writeB(float volts) {
    DAC8532_Out_Voltage(channel_B, volts);
    return true;
}

} // namespace DAC8532Driver
