#include "Client.hpp"

Client::Client() {
    pass = 0;
}

Client::Client(std::string username, std::string hostname, std::string servername,
std::string realname, int fd) {
    this->username = username;
    this->hostname = hostname;
    this->servername = servername;
    this->realname = realname;
    this->fd = fd;
}

std::string Client::getPrefix() const {
    std::string username = "!" + this->username;
    std::string hostname = "@" + this->hostname;

    return this->nickname + username + hostname;
}

std::string Client::getnickname() const {
    return this->nickname;
}
