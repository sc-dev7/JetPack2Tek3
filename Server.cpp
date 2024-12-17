/*
** EPITECH PROJECT, 2024
** Synthese
** File description:
** sc
*/

#include "Server.hpp"

SERVER::Server::Server(Handle_arg::Arg *arg)
    : arg(arg), server_(true), player1_(false), player2_(false),
      cliConnected1_(0), cliConnected2_(0), ready1_(false), ready2_(false),
      FireP1_(false), FireP2_(false), scoreP1_(0), scoreP2_(0), xPos_(0) 
{
    gravityP1_ = arg->getGravity();
    gravityP2_ = gravityP1_;
    std::vector<std::string> mapTmp = arg->getMap();
    yPosP1_ = mapTmp.size() / 2;
    yPosP2_ = yPosP1_;
}

int SERVER::Server::getPort() const
{
    return arg->getPort();
}

std::vector<std::string> SERVER::Server::getMap() const
{
    return arg->getMap();
}

float SERVER::Server::getGravity() const
{
    return arg->getGravity();
}

int SERVER::Server::getSocket() const
{
    return Socket_;
}

bool SERVER::Server::Start()
{
    Socket_ = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress_.sin_family = AF_INET;
    serverAddress_.sin_addr.s_addr = INADDR_ANY;
    serverAddress_.sin_port = htons(arg->getPort());
    if (bind(Socket_, (struct sockaddr*)&serverAddress_, sizeof(serverAddress_)) == -1) {
        throw HandleException::Except("bind error");
        Close();
        return false;
    }
    if (listen(Socket_, 5) == -1) {
        throw HandleException::Except("listen error");
        Close();
        return false;
    }
    server_ = true;
    return true;
}

void SERVER::Server::Run()
{
    FD_ZERO(&readSet_);
    FD_SET(Socket_, &readSet_);
    maxFd_ = Socket_;
    while (server_) {
        fd_set tmpSet = readSet_;
        int numReady = select(maxFd_ + 1, &tmpSet, nullptr, nullptr, nullptr);
        if (numReady == -1) {
            throw HandleException::Except("Select failed");
        }
        if (FD_ISSET(Socket_, &tmpSet)) {
            Accept_Connection();
        }
        Handle_Connections(tmpSet);
        if (ready1_ && ready2_) {
            Player();
            Coin();
            Finish();
        }
    }
    Close_All();
}

void SERVER::Server::Close_All() 
{
    for (int clientSocket : clientSockets_) {
        close(clientSocket);
    }
    clientSockets_.clear();
}

void SERVER::Server::Handle_Connections(fd_set& activeSet)
{
        for (auto it = clientSockets_.begin(); it != clientSockets_.end();) {
            int clientSocket = *it;
            if (FD_ISSET(clientSocket, &activeSet)) {
                char buffer[1024] = "\0";
                ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    if (bytesRead != 0) {
                        throw HandleException::Except("connexion failed");
                    }
                    close(clientSocket);
                    FD_CLR(clientSocket, &readSet_);
                    it = clientSockets_.erase(it);
                    continue;
                }
                std::string tmp;
                char *cmd;
                for (int i = 0; buffer[i]; i += 1) {
                    tmp += buffer[i];
                    if (buffer[i] == '\n') {
                        cmd = strdup(tmp.c_str());
                        Command_client(cmd, clientSocket);
                        tmp = ("");
                    }
                }
            }
            ++it;
        }
}

void SERVER::Server::Quit(int index)
{
    if (!clientSockets_.empty()) {
        close(clientSockets_[index]);
        clientSockets_.erase(clientSockets_.begin() + index + 1);
    }
}

void SERVER::Server::Command_client(char *src, int cliSo)
{
    if (strcmp(src, "ID\n") == 0) {
        ID(cliSo);
    } else if (strcmp(src, "MAP\n") == 0) {
        Map(cliSo);
    } else if (strcmp(src, "READY\n") == 0) {
        Ready(cliSo);
    } else if (strcmp(src, "FIRE 1\n") == 0) {
        FireOn(cliSo);
    } else if (strcmp(src, "FIRE 0\n") == 0) {
        FireOff(cliSo);
    } else {
        std::cout << "command not found" << std::endl;
    }
}

void SERVER::Server::Ready(int soCli)
{
    char *clientSocketStr = strdup("START\n");
    if (soCli == cliConnected1_ && ready1_ == false) {
        ready1_ = true;
    }
    if (soCli == cliConnected2_ && ready2_ == false) {
        ready2_ = true;
    }
    if (ready1_ && ready2_) {
        ssize_t bytesSent = send(cliConnected1_, clientSocketStr, strlen(clientSocketStr), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Ready failed");
        }
        bytesSent = send(cliConnected2_, clientSocketStr, strlen(clientSocketStr), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Ready failed");
        }
    }
}

void SERVER::Server::Accept_Connection()
{
    sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSocket = accept(Socket_, (struct sockaddr*)&clientAddress, &clientAddressLength);
    if (clientSocket == -1) {
        throw HandleException::Except("Accept failed");
    }
    FD_SET(clientSocket, &readSet_);
    maxFd_ = std::max(maxFd_, clientSocket);
    clientSockets_.push_back(clientSocket);
}

void SERVER::Server::Stop()
{
    server_ = false;
}

void SERVER::Server::Close()
{
    if (Socket_ != -1) {
        close (Socket_);
        Socket_ = -1;
    }
}

void SERVER::Server::ID(int cliSo)
{
    char *clientSocketStr;
    if ((cliSo != cliConnected1_) and (cliSo != cliConnected2_)) {
        if (!player1_) {
            player1_ = true;
            cliConnected1_ = cliSo;
            clientSocketStr = strdup("ID 1\n");
        } else if (!player2_) {
            player2_ = true;
            cliConnected2_ = cliSo;
            clientSocketStr = strdup("ID 2\n");
        }
    } else {
        if (cliSo == cliConnected1_)
            clientSocketStr = strdup("ID 1\n");
        else
            clientSocketStr = strdup("ID 2\n");
    }
    ssize_t bytesSent = send(cliSo, clientSocketStr, strlen(clientSocketStr), 0);
    if (bytesSent == -1) {
        throw HandleException::Except("ID failed");
    }
}

void SERVER::Server::Map(int clientSocket)
{
    std::string tmpResponse = "MAP ";
    tmpResponse += std::to_string(getGravity());
    tmpResponse += ' ';
    std::vector<std::string> mapTmp = getMap();
    tmpResponse += std::to_string(mapTmp[0].length());
    tmpResponse += ' ';
    tmpResponse += std::to_string(mapTmp.size());
    tmpResponse += ' ';

    for (const auto& el : mapTmp) {
        tmpResponse += el;
    }

    for (size_t i = 0; i < tmpResponse.size(); ++i) {
        if (tmpResponse[i] == '\n') {
            tmpResponse.erase(i, 1);
            --i;
        }
    }
    tmpResponse += '\n';

    char* clientSocketStr = strdup(tmpResponse.c_str());
    ssize_t bytesSent = send(clientSocket, clientSocketStr, strlen(clientSocketStr), 0);

    free(clientSocketStr);

    if (bytesSent == -1) {
        throw HandleException::Except("Map error");
    }
}

void SERVER::Server::FireOn(int cliSo)
{
    if (cliSo == cliConnected1_) {
        if (!FireP1_) {
            gravityP1_ *= -1;
        }
        FireP1_ = true;
    }
    if (cliSo == cliConnected2_) {
        if (!FireP2_)
            gravityP2_ *= -1;
        FireP2_ = true;
    }
}

void SERVER::Server::FireOff(int cliSo)
{
    if (cliSo == cliConnected1_) {
        if (FireP1_)
            gravityP1_ *= -1;
        FireP1_ = false;
    }
    if (cliSo == cliConnected2_) {
        if (FireP2_)
            gravityP2_ *= -1;
        FireP2_ = false;
    }
}

void SERVER::Server::Player()
{
    std::string P1 = "PLAYER 1 ";
    std::string P2 = "PLAYER 2 ";
    P1 += std::to_string(xPos_);
    P2 += std::to_string(xPos_);
    P1 += " ";
    P2 += " ";
    P1 += std::to_string(yPosP1_);
    P2 += std::to_string(yPosP2_);
    P1 += " ";
    P2 += " ";
    P1 += std::to_string(scoreP1_);
    P2 += std::to_string(scoreP2_);
    P1 += " ";
    P2 += " ";
    if (!FireP1_)
        P1 += "0\n";
    else
        P1 += "1\n";
    if (!FireP2_)
        P2 += "0\n";
    else
        P2 += "1\n";
    std::cout << "PLAYER 1 --> " << P1 << std::endl;
    std::cout << "PLAYER 2 --> " << P2 << std::endl;
    ssize_t bytesSent = send(cliConnected1_, P1.c_str(), P1.length(), 0);
    if (bytesSent == -1) {
        throw HandleException::Except("Error player");
    }
    ssize_t bytesSent2 = send(cliConnected2_, P2.c_str(), P2.length(), 0);
    if (bytesSent2 == -1) {
        throw HandleException::Except("Error player");
    }
    xPos_ += 0.08;
    std::vector<std::string> mapTmp = getMap();
    if (FireP1_ && yPosP1_ < mapTmp.size() - 1)
        yPosP1_ += 0.08;
    else if (yPosP1_ > 0)
        yPosP1_ -= 0.08;
    if (FireP2_ && yPosP2_ < mapTmp.size() - 1)
        yPosP2_ += 0.08;
    else if (yPosP2_ > 0)
        yPosP2_ -= 0.08;
    if (yPosP1_ < 0)
        yPosP1_ = 0;
    if (yPosP2_ < 0)
        yPosP2_ = 0;
    if (yPosP1_ > mapTmp.size() - 1)
        yPosP1_ = mapTmp.size() - 1;
    if (yPosP2_ > mapTmp.size() - 1)
        yPosP2_ = mapTmp.size() - 1;
}

bool Check_Coin(std::pair<int, int> val, std::vector<std::pair<int, int>> stock)
{
    if (stock.empty()) {
        return true;
    }
    for (const auto& el : stock) {
        if (val == el) {
            return false;
        }
    }
    return true;
}

void SERVER::Server::Coin()
{
    int x = static_cast<int>(xPos_);
    std::vector<std::string> mapTmp = getMap();
    int p1y = static_cast<int>(yPosP1_);
    int p2y = static_cast<int>(yPosP2_);
    p2y = (mapTmp.size() - p2y - 1);
    p1y = (mapTmp.size() - p1y - 1);
    std::pair<int, int> newCoin_;
    newCoin_.first = x;
    newCoin_.second = yPosP1_;
    if (mapTmp[p1y][x] == 'c' && Check_Coin(newCoin_, coinGettedP1_)) {
        std::string sendp1 = "COIN 1 ";
        sendp1 += std::to_string(x);
        sendp1 += " ";
        sendp1 += std::to_string(p1y);
        sendp1 += "\n";
        char *clientSocketStr = strdup(sendp1.c_str());
        ssize_t bytesSent = send(cliConnected1_, clientSocketStr, strlen(clientSocketStr), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Error player");
        }
        coinGettedP1_.push_back(newCoin_);
        std::cout << "COIN P1" << std::endl;
        scoreP1_ += 1;
    }
    newCoin_.second = p2y;
    if (mapTmp[p2y][x] == 'c' && Check_Coin(newCoin_, coinGettedP2_)) {
        std::string sendp2 = "COIN 2 ";
        sendp2 += std::to_string(x);
        sendp2 += " ";
        sendp2 += std::to_string(p2y);
        sendp2 += "\n";
        char *clientSocketStr2 = strdup(sendp2.c_str());
        ssize_t bytesSent = send(cliConnected2_, clientSocketStr2, strlen(clientSocketStr2), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Error player");
        }
        coinGettedP2_.push_back(newCoin_);
        std::cout << "COIN P2 IN --> X = " << x << "--> Y = " << yPosP2_ << std::endl;
        scoreP2_ += 1;
    }
}

void SERVER::Server::Finish()
{
    int x = static_cast<int>(xPos_);
    std::vector<std::string> mapTmp = getMap();
    int p1y = static_cast<int>(yPosP1_);
    int p2y = static_cast<int>(yPosP2_);
    p2y = mapTmp.size() - p2y - 1;
    p1y = mapTmp.size() - p1y - 1;
    if (mapTmp[p1y][x] == 'e') {
        std::string finish1 = "FINISH 2\n";
        char *clientSocketStr = strdup(finish1.c_str());
        ssize_t bytesSent2 = send(cliConnected2_, clientSocketStr, strlen(clientSocketStr), 0);
        if (bytesSent2 == -1) {
            throw HandleException::Except("Error finish");
        }
        ssize_t bytesSent = send(cliConnected1_, clientSocketStr, strlen(clientSocketStr), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Error finish");
        }
    }
    if (mapTmp[p2y][x] == 'e') {
        std::string finish2 = "FINISH 1\n";
        char *clientSocketStr2 = strdup(finish2.c_str());
        ssize_t bytesSent2 = send(cliConnected1_, clientSocketStr2, strlen(clientSocketStr2), 0);
        if (bytesSent2 == -1) {
            throw HandleException::Except("Error finish");
        }
        ssize_t bytesSent = send(cliConnected2_, clientSocketStr2, strlen(clientSocketStr2), 0);
        if (bytesSent == -1) {
            throw HandleException::Except("Error finish");
        }
    }
}
