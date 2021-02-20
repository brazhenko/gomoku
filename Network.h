//
// Created by 17641238 on 19.01.2021.
//

#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <assert.h>
#include <string.h>
#include <ctime>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

namespace Network
{
    const int port = 5555;

    std::vector<std::string> find_active_users(int port);
    std::string send_message(std::string ip, int port, std::string message);
    void run_server(int port);
}

#endif