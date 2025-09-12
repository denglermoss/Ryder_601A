#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <thread>           // std::thread
#include <atomic>           // std::atomic
#include <chrono>           // std::chrono::steady_clock::time_point

class Controller {
public:
    void run();
    void shutdown();

private:
    void command_loop();
    void waveform_loop();
    void measurement_loop();
    void start_threads();
    void stop_threads();

    // ADC handled via ADS1256 wrapper directly; no member needed.

    std::thread cmd_thread;
    std::thread wave_thread;
    std::thread measure_thread;

    std::atomic<bool> running{true};
    std::atomic<bool> active{false};

    std::chrono::steady_clock::time_point measurement_start;
};

#endif // CONTROLLER_H