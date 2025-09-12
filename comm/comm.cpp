#include "comm.h"
#include "waveform/wave_gen.h"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
namespace {
    int server_fd, client_fd;
}

int Comm::init(){
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed!" << std::endl;
        return -1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;  // Listen on any network interface
    address.sin_port = htons(PORT);


    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Binding failed!" << std::endl;
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed!" << std::endl;
        return -1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    client_fd = accept(server_fd, nullptr, nullptr);
    if (client_fd < 0) {
        std::cerr << "Accept failed!" << std::endl;
        return -1;
    }

    struct timeval timeout = {1, 0}; // 1 second timeout
    setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    std::cout << "Connected to Client" << std::endl;
    return 1;
}

command Comm::receive_data() {
    command received_data;
    ssize_t bytes = recv(client_fd, reinterpret_cast<char*>(&received_data), sizeof(received_data), 0);
    if (bytes <= 0) {
        if (errno != EWOULDBLOCK && errno != EAGAIN) {
            std::cerr << "Receive failed: " << strerror(errno) << std::endl;
        }
        return BAD_COMMAND;
    }
    received_data.print();
    return received_data;
}

void Comm::send_data(const data_point& message) {
    if(message == BAD_DATA_POINT) return;
    send(client_fd, reinterpret_cast<const char*>(&message), sizeof(message), 0); 
}

void Comm::end_comm(){
    Comm::send_data(BAD_DATA_POINT);
    close(client_fd);
    close(server_fd);
}

