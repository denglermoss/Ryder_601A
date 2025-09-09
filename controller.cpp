#include "controller.h"
#include "waveform/wave_gen.h"
#include "measurement/measurement.h"
#include "comm/comm.h"
#include "config/config.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>

void Controller::run() {
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed!" << std::endl;
        return;
    }
    WaveGen::init();
    Measurement::init();
    if (Comm::init() == -1) return;

    start_threads();

    cmd_thread.join();
    wave_thread.join();
    measure_thread.join();

    shutdown();
}

void Controller::shutdown() {
    WaveGen::stop();
    Measurement::stop_reading();
    gpioTerminate();
    Comm::end_comm();
}

void Controller::start_threads() {
    cmd_thread = std::thread(&Controller::command_loop, this);
    wave_thread = std::thread(&Controller::waveform_loop, this);
    measure_thread = std::thread(&Controller::measurement_loop, this);
}

void Controller::stop_threads() {
    running = false;
}

void Controller::command_loop() {
    while (running) {
        command cmd = Comm::receive_data();
        if (cmd == BAD_COMMAND || Comm::handle_cmd(cmd) == -1) {
            stop_threads();
            break;
        } else if (cmd == START_COMMAND) {
            active = true;
        } else if (cmd == STOP_COMMAND) {
            active = false;
        }
    }
}

void Controller::waveform_loop() {
    using namespace std::chrono;
    const auto interval = microseconds(1000000 / WAVEFORM_UPDATE_RATE);
    auto next_time = steady_clock::now() + interval;

    while (running) {
        if (active) {
            auto start = steady_clock::now();
            WaveGen::update();
            auto exec_time = duration_cast<microseconds>(steady_clock::now() - start);
            if (exec_time > interval) {
                std::cerr << "[Overrun] WaveGen::update took " << exec_time.count()
                          << " µs (limit: " << interval.count() << " µs)\n";
                next_time = steady_clock::now() + interval;
            } else {
                std::this_thread::sleep_until(next_time);
                next_time += interval;
            }
        } else {
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
}

void Controller::measurement_loop() {
    using namespace std::chrono;
    const auto interval = microseconds(1000000 / MEASUREMENT_RATE);
    auto next_time = steady_clock::now() + interval;

    while (running) {
        if (active) {
            auto start = steady_clock::now();
            data_point output = Measurement::get_data_point();
            if (!(output == BAD_DATA_POINT)) {
                Comm::send_data(output);
            }
            auto exec_time = duration_cast<microseconds>(steady_clock::now() - start);
            if (exec_time > interval) {
                std::cerr << "[Overrun] Measurement took " << exec_time.count()
                          << " µs (limit: " << interval.count() << " µs)\n";
                next_time = steady_clock::now() + interval;
            } else {
                std::this_thread::sleep_until(next_time);
                next_time += interval;
            }
        } else {
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
}
