/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#include "Handle_arg.hpp"

Handle_arg::Arg::Arg(char **arg)
{
    for (int i = 0; arg[i]; i++) {
        stock_.push_back(arg[i]);
    }
}

Handle_arg::Arg::~Arg()
{
    stock_.clear();
}

void Handle_arg::Arg::setPort()
{
    std::string tmp;

    for (size_t i = 0; i < stock_.size(); ++i) {
        if (stock_[i] == "-p") {
            if (i + 1 < stock_.size()) {
                tmp = stock_[i + 1];
                if (std::all_of(tmp.begin(), tmp.end(), ::isdigit)) {
                    try {
                        port_ = std::stoi(tmp);
                        if (port_ < 1024 || port_ > 65535) {
                            throw HandleException::Except("Port number must be between 1024 and 65535.");
                        }
                    } catch (const std::invalid_argument& e) {
                        throw HandleException::Except("Port number is not valid.");
                    } catch (const std::out_of_range& e) {
                        throw HandleException::Except("Port number is out of range.");
                    }
                } else {
                    throw HandleException::Except("Port argument must be a valid number.");
                }
            } else {
                throw HandleException::Except("Missing argument for -p.");
            }
            return;
        }
    }
    throw HandleException::Except("Port argument -p is required.");
}

int Handle_arg::Arg::getPort()
{
    return port_;
}

void Handle_arg::Arg::setMap()
{
    size_t i = 0;
    std::string tmp;
    for (const auto& el : stock_) {
        if (el == "-m") {
            if (stock_.size() == i) {
                throw HandleException::Except("Map not found");
            } else {
                i ++;
                tmp = stock_[i];
                break;
            }
        }
        i += 1;
    }
    std::ifstream file;
    std::string input;
    file.open(tmp);
    if (!file)
        throw HandleException::Except("Map not found.");
    for (int i = 0; std::getline(file, input); i ++) {
        for (int j = 0; input[j]; j += 1) {
            if (input[j] != '_' && input[j] != 'o' && input[j] != 'c' && input[j] != 'e' && input[j] != '\n' && input[j] != ' ')
                throw HandleException::Except("Error load map.");
        }
        map_.push_back(input);
    }
}

std::vector<std::string> Handle_arg::Arg::getMap()
{
    return map_;
}

void Handle_arg::Arg::setGravity()
{
    size_t i = 0;
    bool err = false;
    std::string tmp;
    size_t size = stock_.size() - 1;
    for (const auto& el : stock_) {
        if (el == "-g") {
            if (size > i) {
                err = true;
                i += 1;
                tmp = stock_[i];
                break;
            } else if (size == i) {
                break;
            }
        }
        i ++;
    }
    if (!err) {
        gravity_ = 1;
        return;
    }
    int chk = 0;
    for (int j = 0; tmp[j]; j += 1) {
        if ((tmp[j] < '0' || tmp[j] > '9') && tmp[j] != '.') {
            gravity_ = 1;
            return;
        }
        if (tmp[j] == '.')
            chk += 1;
    }
    if (chk > 1)
        throw HandleException::Except("error gravity");
    gravity_ = std::stof(tmp);
}

float Handle_arg::Arg::getGravity()
{
    return gravity_;
}
