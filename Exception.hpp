/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#pragma once

#include <cstring>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <utility>
#include <iostream>
#include <fstream>
#include <exception>

namespace HandleException {
    class Except : public std::exception {
        public:
            explicit Except(const std::string &msg);
            const char* what() const noexcept override {
                return err_.c_str();
            }

        private:
            std::string err_;
    };
}
