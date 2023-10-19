#include "client.hpp"

Client::Client() {
	//
}

Client::Client(std::string username, std::string hostname, std::string servername,
std::string realname, int fd) {
	//
}

std::string Client::getPrefix() const {
	std::string username = "!" + this->username;
	std::string hostname = "@" + this->hostname;

	return this->nickname + username + hostname;
}

const std::string&	getNickname() const{
	return (this->_nickname);
};
const std::string&	getUsername() const{
	return (this->_username);
};
const std::string&	getHostname() const{
	return (this->_hostname);
};
const std::string&	getServername() const{
	return (this->_servername);
};
const std::string&	getRealname() const{
	return (this->_realname);
};
int			getSocketFd() const{
	return (this->_socket_fd);
};

int getAuth() {
	//원래 pass??
	return (0);
}

int checkChannelLimit() const{
	if (this->_channel_limit > this->_channel_list.size())
		return (0);
	else
		return (-1);
}
