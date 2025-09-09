#include "controller.h"
#include "waveform/wave_gen.h"
#include "measurement/measurement.h"
#include "comm/comm.h"
#include "config/config.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <atomic>
#include <pigpio.h>

void Controller::run() {
    measurement_start = std::chrono::steady_clock::now();
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed!" << std::endl;
        return;
    }
    WaveGen::init();
    adc.init(ADC_CHANNEL,SPI_SPEED);
    if (Comm::init() == -1) return;

    start_threads();

    cmd_thread.join();
    wave_thread.join();
    measure_thread.join();

    shutdown();
}

void Controller::shutdown() {
    gpioTerminate();
    Comm::end_comm();
    running = false;
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
        // cmd.print();/
        if ( cmd == CLOSE_COMMAND) {
            stop_threads();
            break;
        } else if (cmd == START_COMMAND) {
            WaveGen::reset_sample_index();
            measurement_start = std::chrono::steady_clock::now();
            active = true;
        } else if (cmd == STOP_COMMAND) {
            active = false;
        } else if (cmd.cmd == 2) {
            WaveGen::setup(cmd);
        }
    }
}

void Controller::waveform_loop() {
    using namespace std::chrono;
    const auto interval = microseconds(1000000 / WAVEFORM_UPDATE_RATE);
    auto next_time = steady_clock::now() + interval;
    int intervals_missed = 0;

    while (running) {
        if (active) {
            auto start = steady_clock::now();
            WaveGen::update(intervals_missed);
            intervals_missed = 0;
            auto exec_time = duration_cast<microseconds>(steady_clock::now() - start);
            if (exec_time > interval) {
                intervals_missed = (exec_time / interval)+1;
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

    while (running) {
        if (active) {
            auto timestamp = duration<float>(steady_clock::now() - measurement_start).count();

            int32_t raw0 = adc.readAIN0();
            int32_t raw3 = adc.readAIN3();
            if (raw0 != -1 && raw3 != -1) {
                float voltage0 = raw0 * ADC_TO_VOLTS;
                float voltage3 = raw3 * ADC_TO_VOLTS;

                Comm::send_data(data_point(timestamp, voltage0,voltage3)); // AIN0
            }
        } else {
            std::this_thread::sleep_for(milliseconds(10));
        }
    }
}

