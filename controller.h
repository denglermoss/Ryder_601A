#ifndef CONTROLLER_H
#define CONTROLLER_H

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

    std::thread cmd_thread;
    std::thread wave_thread;
    std::thread measure_thread;

    std::atomic<bool> running{true};
    std::atomic<bool> active{false};
};

#endif // CONTROLLER_H