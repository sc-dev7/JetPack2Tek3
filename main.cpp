/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#include "Server.hpp"

int main(int ac, char **av)
{
    try {
        if (ac > 7) {
            throw HandleException::Except("Incorrect arguments.");
        }
        Handle_arg::Arg *arg = new Handle_arg::Arg(av);
        arg->setPort();
        arg->setMap();
        arg->setGravity();
        SERVER::Server S(arg);
        S.Start();
        S.Run();
        S.Stop();
    } catch (const HandleException::Except& e) {
        return 84;
    }
    return 0;
}
