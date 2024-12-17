/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#pragma once

#include "Exception.hpp"

namespace Handle_arg {
    class Arg {
        public:
            Arg(char **);
            ~Arg();
            void setPort();
            int getPort();
            void setGravity();
            float getGravity();
            void setMap();
            std::vector<std::string> getMap();

        private:
            std::vector<std::string> stock_;
            int port_;
            float gravity_;
            std::vector<std::string> map_;
    };
}
