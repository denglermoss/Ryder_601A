#include "wave_gen.h"
#include "waveform/functions.h"
#include "wrappers/dac8532_driver.h"
#include "config/config.h"
#include <limits>
#include <iostream>

namespace {
    uint16_t code_table[WAVEFORM_UPDATE_RATE];
    float temp_table[WAVEFORM_UPDATE_RATE];

    volatile int sample_index = 0;
    volatile int num_points_pulse = WAVEFORM_UPDATE_RATE;
    volatile int num_points_wave = WAVEFORM_UPDATE_RATE;
}

int scale_table(float in_min, float in_max, float amp, float offset) {
    if (in_min == in_max) {
        std::cerr << "Error: Function produces constant output; normalization is not possible." << std::endl;
        return -1;
    }

    float volts_min = (offset - (amp / 2));
    float volts_max = (offset + (amp / 2));

    int out_min = std::max(static_cast<int>(volts_min * VOLTS_TO_ADC), 0);
    int out_max = std::min(static_cast<int>(volts_max * VOLTS_TO_ADC), MAX_DAC_VALUE);

    if (out_max < out_min) {
        std::cerr << "Error: Output range is invalid (out_max < out_min)." << std::endl;
        return -1;
    }

	float scale = static_cast<float>(out_max - out_min) / (in_max - in_min);
	float shift = -in_min * scale + out_min;
    	for (int i = 0; i < num_points_pulse; i++) {
    		code_table[i] = static_cast<uint16_t>(temp_table[i] * scale + shift);
    	}

    std::cout << "Table Generated with DAC scaling [" << out_min << ", " << out_max << "]" << std::endl;

    return 1;
}



int WaveGen::setup(command cmd) {
	if(cmd.amp <= 0.0 or cmd.pulse_freq <= 0.0 or cmd.wave_freq <= 0.0) return -1;

	wave_func_ptr f = Functions::get_function(cmd.func);
	if(f == nullptr) return -1;

    num_points_pulse = std::min(static_cast<int>(WAVEFORM_UPDATE_RATE / cmd.pulse_freq), WAVEFORM_UPDATE_RATE);
	num_points_wave= std::min(static_cast<int>(WAVEFORM_UPDATE_RATE / cmd.wave_freq), WAVEFORM_UPDATE_RATE);

    float f_min = std::numeric_limits<float>::max();
    float f_max = std::numeric_limits<float>::lowest();

    // First pass: Determine min and max values of f(t)
	float inv_num_points = 1.0f/static_cast<float>(num_points_pulse);
    for (int i = 0; i < num_points_pulse; i++) {
        float t = i*inv_num_points;
        float val = f(t);
		f_min = std::min(f_min, val);
		f_max = std::max(f_max, val);
		temp_table[i] = val;
    }

    if(scale_table(f_min,f_max,cmd.amp,cmd.offset) < 0) return -1;
	return 1;
}

void WaveGen::reset_sample_index(){
	sample_index = 0;
}


void WaveGen::init(){
    DAC8532Driver::init();
}

void WaveGen::update(int samples_to_skip) {
	sample_index += samples_to_skip;
	    if (sample_index <= num_points_pulse) {
        uint16_t code = (sample_index < num_points_pulse) ? code_table[sample_index] : 0;
        // Convert code (0..MAX_DAC_VALUE) to volts for DAC driver
        float volts = static_cast<float>(code) * (3.3f / static_cast<float>(MAX_DAC_VALUE));
        DAC8532Driver::writeA(volts);
    }
	sample_index = (sample_index + 1) % num_points_wave;
}
