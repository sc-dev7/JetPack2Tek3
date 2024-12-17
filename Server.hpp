/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#pragma once

#include "Handle_arg.hpp"

namespace SERVER {
    class Server {
        public:
            Server(Handle_arg::Arg *);
            ~Server() {};
            int getPort() const ;
            std::vector<std::string> getMap() const;
            float getGravity() const;
            int getSocket() const;
            bool Start();
            void Run();
            void Stop();
            void Close();
            void Accept_Connection();
            void Handle_Connections(fd_set&);
            void Command_client(char *, int);
            void Quit(int);
            void Close_All();
            void ID(int);
            void Map(int);
            void Ready(int);
            void FireOn(int);
            void FireOff(int);
            void Player();
            void Coin();
            void Finish();

        private:
            Handle_arg::Arg *arg;
            std::chrono::milliseconds interval_;
            std::chrono::steady_clock::time_point nextTime_;
            float gravityP1_;
            float gravityP2_;
            int Socket_;
            int maxFd_;
            fd_set readSet_;
            bool server_;
            sockaddr_in serverAddress_;
            std::vector<int> clientSockets_;
            bool player1_;
            bool player2_;
            int cliConnected1_;
            int cliConnected2_;
            bool ready1_;
            bool ready2_;
            bool FireP1_;
            bool FireP2_;
            float yPosP1_;
            float yPosP2_;
            int scoreP1_;
            int scoreP2_;
            float xPos_;
            std::vector<std::pair<int, int>> coinGettedP1_;
            std::vector<std::pair<int, int>> coinGettedP2_;
    };
}
