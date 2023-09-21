#include "util.h"

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
