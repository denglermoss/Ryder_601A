#include <iostream>
#include <pigpio.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <chrono>
#include <pthread.h>
#include <sched.h>
#include "wave_gen.h"
#include "measurement.h"
#include "functions.h"
#include "comm.h"
#include "config.h"


static volatile std::atomic<bool> running(true);

void set_realtime_priority(int prio = 80) {
    sched_param sch_params;
    sch_params.sched_priority = prio;
    if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sch_params) != 0) {
        std::cerr << "Failed to set real-time priority\n";
    }
}

void waveform_loop() {
    using namespace std::chrono;

    const auto interval = microseconds(1000000 / WAVEFORM_UPDATE_RATE);
    auto next_time = steady_clock::now() + interval;

    set_realtime_priority(); // Optional, but helps
	int samples_to_skip = 0;

    while (running) {
        auto start = steady_clock::now();

        WaveGen::update(samples_to_skip);
		samples_to_skip = 0; // reset

        auto end = steady_clock::now();
        auto exec_time = duration_cast<microseconds>(end - start);
        if (exec_time > interval) {
            std::cerr << "[Overrun] WaveGen::update took " << exec_time.count() << " µs (limit: " << interval.count() << " µs)\n";
            samples_to_skip = 1;
			next_time = end + interval; // resync cleanly
        } else {
            std::this_thread::sleep_until(next_time);
            next_time += interval;
        }
    }
}

void measurement_loop() {
    using namespace std::chrono;

    const auto interval = microseconds(1000000 / MEASUREMENT_RATE); // ~250 µs for 4 kHz
    auto next_time = steady_clock::now() + interval;

    set_realtime_priority();  // optional

    while (running) {
        auto start = steady_clock::now();

        data_point output = Measurement::get_data_point();
        if (!(output == BAD_DATA_POINT)) {
            Comm::send_data(output);
        }

        auto end = steady_clock::now();
        auto exec_time = duration_cast<microseconds>(end - start);

        if (exec_time > interval) {
            std::cerr << "[Overrun] Measurement took "
                      << exec_time.count() << " µs (limit: " << interval.count() << " µs)\n";
            next_time = end + interval;
        } else {
            std::this_thread::sleep_until(next_time);
            next_time += interval;
        }
    }
}

void command_listener() {
    command cmd;
    while (running) {
        cmd = Comm::receive_data();  // This blocks until data is received
        if (Comm::handle_cmd(cmd) == -1){
            running = false;  // Shut everything down
            break;
        }
    }
}

int setup(){
	if (gpioInitialise() < 0) {
		std::cerr << "pigpio initialization failed!" << std::endl;
		return 1;
	}
	WaveGen::init();
	Measurement::init();
	if (Comm::init() == -1) {
        return 1;
    }

	return 0;
}

void close(){
	WaveGen::stop();
	Measurement::stop_reading();
	gpioTerminate();
	Comm::end_comm();
}

int main() {
	atexit(close);
	if (setup() != 0){
		return -1;
	}	
	std::thread waveform_thread(waveform_loop);
	std::thread measure_thread(measurement_loop);
	std::thread cmd_thread(command_listener);
	
	measure_thread.join();
	waveform_thread.join();
	cmd_thread.join();
	return 0;
}
