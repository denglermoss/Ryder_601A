#ifndef COMM_H
#define COMM_H

#include "config.h"

namespace Comm{
    int init();
    void send_data(const data_point& message);
    command receive_data();
    void end_comm();
    int handle_cmd(command &cmd);
}

#endif