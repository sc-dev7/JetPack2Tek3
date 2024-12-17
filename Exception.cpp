/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#include "Exception.hpp"

HandleException::Except::Except(const std::string &msg) : err_(msg) {
    std::cout << "Error: " << what() << std::endl;
}
