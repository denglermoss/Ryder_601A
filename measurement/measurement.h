#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "config/config.h"
#include <cstdint>

namespace Measurement {
    void init();
    uint16_t readADC();
    data_point get_data_point();
}

#endif // MEASUREMENT_H
