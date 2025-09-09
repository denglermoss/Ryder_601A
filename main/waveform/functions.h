#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "config.h"

namespace Functions{
	wave_func_ptr get_function(int i);
	float SIN(float t);
	float T(float t);
	float R(float t);
	float P(float t);
	float Q(float t);
	float QRS(float t);
	float PRT(float t);
	float PQRST(float t);
}

#endif
