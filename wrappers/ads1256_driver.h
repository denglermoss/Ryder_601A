#ifndef ADS1256_DRIVER_H
#define ADS1256_DRIVER_H

#include <cstdint>

// Simple C++ wrapper around Waveshare ADS1256 C driver.
namespace ADS1256Driver {
    // Initialize bcm2835, SPI, GPIO and ADS1256 itself.
    // Returns true on success.
    bool init();

    // Read a single-ended channel [0..7]. Returns raw 24-bit signed value
    // sign-extended to int32_t. On error, returns -1.
    int32_t read_channel(int channel);

    // Read all 8 single-ended channels into the provided array of size >= 8.
    // Returns true on success.
    bool read_all(int32_t* out8);

    // Shutdown SPI/GPIO.
    void shutdown();
}

#endif // ADS1256_DRIVER_H
