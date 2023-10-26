#include "Server.hpp"
#include "message.h"
#include <vector>

//10.11.3.2
//irssi -c 10.12.1.7 -p 8080 -w 1234 -n juhyulee
//docker run -d --name ubuntu -p 80:80 -it --privileged ubuntu:20.04
//서버네임 숫자 닉네임 메세지

Server::Server() {}

Server::~Server() {}

const std::string& Server::getServerName() const { return _server_name; }

const std::string& Server::getServerPassword() const { return _server_password; }

const std::map<int, Client *>& Server::getUserList() const { return _user_list; }

const std::map<std::string, Channel *>& Server::getChannelList() const { return _channel_list; }

void Server::setServerName(const std::string& server_name) { _server_name = server_name; }

void Server::setServerPassword(const std::string& server_password) { _server_password = server_password; }

void Server::addUserList(int client_fd, Client *client) {
	_user_list.insert(std::make_pair(client_fd, client));
}

void Server::removeUserList(int client_fd) { _user_list.erase(client_fd); }

void Server::addChannelList(const std::string& channel_name, Channel *channel) {
	_channel_list.insert(std::make_pair(channel_name, channel));
}

void Server::removeChannelList(const std::string& channel_name) { _channel_list.erase(channel_name); }

void Server::serverInit(int argc, char **argv) {

	(void)argc;
	_server_password = argv[2];
	if ((_server_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		std::cout << "socket error" << std::endl;
		exit(0);
	}
	int option = 1;
	setsockopt(_server_socket, SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option));

	memset(&_server_addr, 0, sizeof(_server_addr));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(_server_socket, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) == -1) {
		std::cout << "bind error" << std::endl;
		exit(0);
	}

	if (listen(_server_socket, 5) == -1) {
		std::cout << "listen error" << std::endl;
		exit(0);
	}
	fcntl(_server_socket, F_SETFL, O_NONBLOCK);
	int kq;
	if ((kq = kqueue()) == -1) {
		std::cout << "kqueue() error" << std::endl;
		exit(0);
	}

	changeEvents(_change_list, _server_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	std::cout << "echo server started" <<std::endl;

	while (1) {
		_new_events = kevent(kq, &_change_list[0], _change_list.size(), _event_list, 8, NULL);
		if (_new_events == -1) {
			std::cout << "kqueue() error" << std::endl;
			exit(0);
		}

		_change_list.clear();

		for (int i = 0; i < _new_events; ++i) {
			_curr_event = &_event_list[i];

			if (_curr_event->flags & EV_ERROR) {
				if (int(_curr_event->ident) == _server_socket) {
					std::cout << "server socket error" << std::endl;
					exit(0);
				}
				else {
					std::cerr << "client socket error" << std::endl;
					disconnectClient(_curr_event->ident, _clients);
				}
			}
			else if (_curr_event->filter == EVFILT_READ) {
				if (int(_curr_event->ident) == _server_socket) {
					int client_socket;
					if ((client_socket = accept(_server_socket, NULL, NULL)) == -1) {
						std::cout << " error" << std::endl;
						exit(0);
					}
					std::cout << "accept new client: " << client_socket << std::endl;
					fcntl(client_socket, F_SETFL, O_NONBLOCK);

					changeEvents(_change_list, client_socket, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
					changeEvents(_change_list, client_socket, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
					_clients[client_socket] = "";
				}
				else if (_clients.find(_curr_event->ident) != _clients.end()) {
					char buf[1024];
					int n = recv(_curr_event->ident, buf, sizeof(buf), 0);

					if (n <= 0) {
						if (n < 0)
							std::cerr << "client read error!" << std::endl;
						disconnectClient(_curr_event->ident, _clients);
					}
					else {
						buf[n] = '\0';
						_clients[_curr_event->ident] += buf;
						std::cout << "msg from " << _curr_event->ident << ":\n" << _clients[_curr_event->ident] << std::endl;
						parsingData(_curr_event->ident);
						//===================parsing==============================
						if (!_send_data[_curr_event->ident].empty()) {
							changeEvents(_change_list, _curr_event->ident, EVFILT_READ, EV_DISABLE, 0, 0, _curr_event->udata);
							changeEvents(_change_list, _curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, _curr_event->udata);
						}
					}
				}
			}
			else if (_curr_event->filter == EVFILT_WRITE) {
				std::map<int, std::string>::iterator it = _clients.find(_curr_event->ident);
				if (it != _clients.end()) {
					if (!_send_data[_curr_event->ident].empty()) {//ㄴㅐ가 보보내내는는거
						int n;
						// std::cout << "send data from" << _curr_event->ident << ": " << _send_data[_curr_event->ident] << std::endl;
						if ((n = send(_curr_event->ident, _send_data[_curr_event->ident].c_str(), _send_data[_curr_event->ident].size(), 0) == -1)) {
							std::cerr << "client write error!" << std::endl;
							disconnectClient(_curr_event->ident, _clients);
						}
						else { // 스코프 범위 다시 확실하게 변경
							_send_data[_curr_event->ident].clear();
							changeEvents(_change_list, _curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, _curr_event->udata);
							changeEvents(_change_list, _curr_event->ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, _curr_event->udata);
						}
					}
				}
			}
		}
	}
}

void Server::changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, \
	int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd, std::map<int, std::string>	clients) {
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	(void)clients;
	this->_clients.erase(client_fd);
	std::map<int, Client *>::iterator it;
	it = this->_temp_list.find(client_fd);
	if (it != this->_temp_list.end()) //temp list에 있는 fd
	{
		delete it->second;
		this->_temp_list.erase(client_fd);
		return ;
	}
	it = this->_user_list.find(client_fd);
	if (it != this->_user_list.end()) //user list에 있는 fd
	{
		delete it->second;
		this->_user_list.erase(client_fd);
	}
}

void Server::parsingData(int fd) { //읽음
	std::vector<std::string> token;
	size_t pos;
	std::string line;
	Client *user = NULL;
	while (!_clients[fd].empty()) {
		if (_clients[fd].find("\r\n") != std::string::npos) {
			pos = _clients[fd].find("\r\n");
			line = _clients[fd].substr(0, pos + 2);
			token.push_back(line);
			_clients[fd].erase(0, pos + 2);
			std::cout << "token : " << line << std::endl;
		}
		else {
			std::cout << "parsing ERR" << std::endl;
			break;
		}
	}
			// std::cout << "parsing....." << std::endl;
	for (size_t i = 0; i < token.size();i++) {
		std::istringstream input_str(token[i]);
		std::string word;
		std::vector<std::string>	tokenizer;

		while(input_str >> word) {
			tokenizer.push_back(word);
			// std::cout << "words : " << word << std::endl;
		}
		if (tokenizer[0] == "PASS" || tokenizer[0] == "NICK" || tokenizer[0] == "USER")
		{
			if (tokenizer[0] == "PASS") {commandPass(tokenizer, fd);}
			else if (tokenizer[0] == "NICK") {commandNick(tokenizer, fd);}
			else if (tokenizer[0] == "USER") {commandUser(tokenizer, fd);}
			user = this->searchTemp(fd);
			if (user && this->getAuth(user) == true)
			{
				this->_temp_list.erase(fd);
				this->addUserList(fd, user);
				std::cout<< RPL_WELCOME(user->getNickname()) << std::endl;
				this->sendMessage(RPL_WELCOME(user->getNickname()), fd);
			}
		}
		else
		{
			user = this->searchClient(fd);
			if (!user)
			{
				std::cout <<"unregistered user fd: "<<fd << std::endl;
				return;
			}
			if (tokenizer[0] == "JOIN") {commandJoin(tokenizer, user, fd);}
			else if (tokenizer[0] == "PING") {commandPing(tokenizer, user, fd);}
			else if (tokenizer[0] == "PART") {commandPart(tokenizer, user, fd);}
			else if (tokenizer[0] == "PRIVMSG") {commandPrivmsg(tokenizer, user, fd);}
			// else if (tokenizer[0] == "QUIT") {}
			// else if (tokenizer[0] == "TOPIC") {}
			// else if (tokenizer[0] == "INVITE") {}
			// else if (tokenizer[0] == "KICK") {}
			else if (tokenizer[0] == "MODE") {commandMode(tokenizer, user, fd);}
		}

	}
}

bool	Server::getAuth(Client const *user) {
	if (user->getPass() == false)
		return false ;
	if (user->getUsername() == "" || user->getRealname() == "")
		return false ;
	if (user->getNickname() == "")
		return false ;
	return true ;
}

Channel	*Server::makeChannel(std::string channel_name, Client *client) {
	Channel *channel = new Channel(channel_name, client);
	addChannelList(channel_name, channel);
	return channel;
}

void	Server::deleteChannel(Channel **channel) {
	(*channel)->clearChannelMode();
	(*channel)->clearUserList();
	(*channel)->clearInviteList();
	(*channel)->clearChannelOperator();
	delete (*channel);
	(*channel) = NULL;
}

Channel	*Server::searchChannel(std::string channel_name) {
	std::map<std::string, Channel *>::iterator iter = _channel_list.find(channel_name);
	if (iter != _channel_list.end()) {
		return iter->second;
	}
	return (NULL);
}

void Server::sendMessage(std::string message, int fd) { //메세지 보내는 함수
	_send_data[fd] += message;
	std::cout << "msg to " << fd << ":\n" << message << std::endl; 
	changeEvents(_change_list, _curr_event->ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, _curr_event->udata);
}

void Server::broadcastChannelMessage(std::string message, Channel *ch) {
	std::cout <<"broadcasting " << std::endl;
	for (std::map<int, Client *>::const_iterator iter = ch->getUserList().begin(); \
		iter != ch->getUserList().end(); ++iter) {
		sendMessage(message, iter->second->getSocketFd());
	}
}

void Server::broadcastChannelMessage(std::string message, Channel *ch, int socket_fd) {
	std::cout <<"broadcasting except " << socket_fd << std::endl;
	for (std::map<int, Client *>::const_iterator iter = ch->getUserList().begin(); \
		iter != ch->getUserList().end(); ++iter) {
		if (iter->first != socket_fd) {
			std::cout <<  iter->first << "except " << socket_fd << std::endl;
			sendMessage(message, iter->second->getSocketFd());
		}
	}
}


Client	*Server::searchClient(std::string nickname){
	for (std::map<int, Client *>::iterator iter = this->_user_list.begin();\
	iter != _user_list.end(); iter++)
	{
		if (iter->second->getNickname() == nickname)
			return (iter->second);
	}
	return (NULL);
}

Client	*Server::searchClient(int fd){
	std::map<int, Client *>::iterator iter= _user_list.find(fd);
	if (iter != _user_list.end()) {
		return iter->second;
	}
	return (NULL);
}

Client	*Server::searchTemp(int fd){
	std::map<int, Client *>::iterator iter= _temp_list.find(fd);
	if (iter != _temp_list.end()) {
		return iter->second;
	}
	return (NULL);
}
