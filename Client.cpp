#include "Client.hpp"

// Client::Client() {}

// Client::Client(const Client& copy) {
// 	*this = copy;
// }

// Client& Client::operator=(const Client& obj) {
// 	if (this == &obj)
// 		return *this;
// 	_socket_fd = obj._socket_fd;
// 	_nickname = obj._nickname;
// 	_username = obj._username;
// 	_hostname = obj._hostname;
// 	_realname = obj._realname;
// 	_user_ip = obj._user_ip;
// 	return *this;
// }

Client::Client(int socket_fd, std::string username, std::string hostname, \
	std::string realname, std::string user_ip) \
: _socket_fd(socket_fd), _username(username), _hostname(hostname), _realname(realname), _user_ip(user_ip) {}

Client::~Client() {}

int	Client::getSocketFd() const { return _socket_fd; };

const std::string&	Client::getNickname() const { return _nickname; };

const std::string&	Client::getUsername() const { return _username; };

const std::string&	Client::getHostname() const { return _hostname; };

const std::string&	Client::getRealname() const { return _realname; };

const std::string&	Client::getUserIp() const { return _user_ip; };

std::string	Client::getPrefix() const {
	std::string username = "!" + _username;
	std::string hostname = "@" + _hostname;
	return _nickname + username + hostname;
}






// int Client::getAuth() {
	// 원래 pass??
	// return 0;
// }

// int Client::checkChannelLimit() const {
// 	if (this->_channel_limit > this->_channel_list.size())
// 		return (0);
// 	else
// 		return (-1);
// }
