#ifndef DAC8532_DRIVER_H
#define DAC8532_DRIVER_H

// Simple C++ wrapper around Waveshare DAC8532 C driver.
namespace DAC8532Driver {
    // Initialize if needed (DEV_ModuleInit already done by ADS1256Driver::init).
    // Provided for symmetry; returns true.
    bool init();

    // Output to channel A/B in volts (0.0 .. 3.3V typical)
    bool writeA(float volts);
    bool writeB(float volts);
}

#endif // DAC8532_DRIVER_H
