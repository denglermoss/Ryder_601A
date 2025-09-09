#ifndef WAVE_GEN_H
#define WAVE_GEN_H
#include "config/config.h"
#include <cstdint>

namespace WaveGen{
	void init();
	void update(int samples_to_skip = 0);
	void start();
	void stop();
	int setup(command cmd);
}

#endif
