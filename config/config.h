#ifndef CONFIG_H
#define CONFIG_H

#include <cstdint>
#include <iostream>

// === SPI Configuration ===
#define DAC_CHANNEL 0
#define ADC_CHANNEL 1
#define SPI_SPEED 8000000

// === System Rates ===
#define WAVEFORM_UPDATE_RATE 48000
#define MEASUREMENT_RATE 2000

// === ADC/DAC Scaling ===
#define MAX_DAC_VALUE 4095
inline constexpr float ADC_TO_VOLTS = 5.0f / MAX_DAC_VALUE;
inline constexpr float VOLTS_TO_ADC = MAX_DAC_VALUE / 4.095f;

// === Command + Data Structs ===
#pragma pack(push, 1)
struct data_point {
    float timestamp;
    float voltage;
    data_point(float t = -1.0f, float v = -1.0f) : timestamp(t), voltage(v) {}
    void print() const { std::cout << voltage << " " << timestamp << "\n"; }
    bool operator==(const data_point& other) const {
        return (timestamp == other.timestamp) && (voltage == other.voltage);
    }
};

struct command {
    float amp;
    float pulse_freq;
    float wave_freq;
    float offset;
    int cmd;
    int func;

    command(int c = -1, int s = -1, float a = -1.0f, float pf = -1.0f, float wf = -1.0f, float o = -1.0f)
        : cmd(c), func(s), amp(a), pulse_freq(pf), wave_freq(wf), offset(o) {}

    void print() const {
        std::cout << "amp: " << amp << ", pulse_freq: " << pulse_freq
                  << ", wave_freq: " << wave_freq << ", offset: " << offset
                  << ", cmd: " << cmd << ", func: " << func  << "\n";
    }

    bool operator==(const command& other) const {
        return (cmd == other.cmd) && (func == other.func) &&
               (amp == other.amp) && (pulse_freq == other.pulse_freq) &&
               (wave_freq == other.wave_freq) && (offset == other.offset);
    }
};
#pragma pack(pop)

// === Global Constants ===
inline const data_point BAD_DATA_POINT{};
inline const command BAD_COMMAND{};
inline const command STOP_COMMAND{0};
inline const command START_COMMAND{1};
inline const command CLOSE_COMMAND{10};

// === Waveform Function Type ===
typedef float(*wave_func_ptr)(float);

#endif // CONFIG_H
