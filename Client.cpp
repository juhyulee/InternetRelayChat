#include "Client.hpp"
#include "Channel.hpp"

// Client::Client() {}

// Client::Client(const Client& copy) {
// 	*this = copy;
// }

Client& Client::operator=(const Client& obj) {
	if (this == &obj)
		return *this;
	_socket_fd = obj.getSocketFd();
	_nickname = obj.getNickname();
	_username = obj.getUsername();
	_hostname = obj.getHostname();
	_realname = obj.getRealname();
	_user_ip = obj.getUserIp();
	return *this;
}

Client::Client(int socket_fd, std::string username, std::string hostname, \
	std::string realname, std::string user_ip) \
: _socket_fd(socket_fd), _username(username), _hostname(hostname), \
_realname(realname), _user_ip(user_ip) {}

Client::~Client() {}

int	Client::getSocketFd() const { return _socket_fd; };

const std::string&	Client::getNickname() const { return _nickname; };

const std::string&	Client::getUsername() const { return _username; };

const std::string&	Client::getHostname() const { return _hostname; };

const std::string&	Client::getRealname() const { return _realname; };

const std::string&	Client::getUserIp() const { return _user_ip; };

const std::map<std::string, Channel *>&	Client::getChannelList() const { return _channel_list; };

void	Client::setSocketFd(int socket_fd) { _socket_fd = socket_fd; };

void	Client::setNickname(const std::string& nickname) { _nickname = nickname; };

void	Client::setUsername(const std::string& username) { _username = username; };

void	Client::setHostname(const std::string& hostname) { _hostname = hostname; };

void	Client::setRealname(const std::string& realname) { _realname = realname; };

void	Client::setUserIp(const std::string& user_ip) { _user_ip = user_ip; };

std::string	Client::getPrefix() const {
	std::string username = "!" + _username;
	std::string hostname = "@" + _hostname;
	return _nickname + username + hostname;
}
