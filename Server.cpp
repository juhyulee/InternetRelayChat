#include "util.h"
#include "Server.hpp"
//10.11.3.2
//irssi -c 10.28.3.5 -p 8080 -w 1234 -n juhyulee
//irssi -c 10.12.9.2 -p 8080 -w 1234 -n juhyulee
//서버네임 숫자 닉네임 메세지

Server::Server() {}

Server::~Server() {}

const std::string& Server::getServerName() const {
	return this->_server_name;
}

const std::map<std::string, Channel>& Server::getChannelList() const {
	return this->_channel_list;
}

const std::map<int, Client>& Server::getUserList() const {
	return this->_user_list;
}

const std::string& Server::getServerPassword() const {
	return this->_server_password;
}

void Server::changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, \
int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
	struct kevent temp_event;

	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
	change_list.push_back(temp_event);
}

void Server::disconnectClient(int client_fd, std::map<int, std::string>& clients) {
	std::cout << "client disconnected: " << client_fd << std::endl;
	close(client_fd);
	clients.erase(client_fd);
}

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
						std::cout << "accept error" << std::endl;
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

					if (n < 0) {
						if (n < 0)
							std::cerr << "client read error!" << std::endl;
						disconnectClient(_curr_event->ident, _clients);
					}
					else {
						buf[n] = '\0';
						_clients[_curr_event->ident] += buf;
						// std::cout << "received data from " << curr_event->ident << ": " << clients[curr_event->ident] << std::endl;
						parsingData(_clients[_curr_event->ident],_curr_event->ident);
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
						std::cout << "send data from" << _curr_event->ident << ": " << _send_data[_curr_event->ident] << std::endl;
						if ((n = send(_curr_event->ident, _clients[_curr_event->ident].c_str(), _clients[_curr_event->ident].size(), 0) == -1)) {
							std::cerr << "client write error!" << std::endl;
							disconnectClient(_curr_event->ident, _clients);
						}
						else
							_clients[_curr_event->ident].clear();
							changeEvents(_change_list, _curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, _curr_event->udata);
							changeEvents(_change_list, _curr_event->ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, _curr_event->udata);
					}
				}
			}
		}
	}
}

void Server::sendMessage(std::string message, int fd) { //메세지 보내는 함수
	_send_data[fd] = message;
	changeEvents(_change_list, _curr_event->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, _curr_event->udata);
}

void Server::parsingData(std::string message, int fd) { //읽음
	std::vector<std::string> token;
	size_t pos = 0;
	std::string line;

	while (1) {

		if (message.find("\r\n") != std::string::npos) {
			pos = message.find("\r\n");
			line = message.substr(0, pos + 1);
			std::cout << "line : " << line << std::endl;
			break;
		}
		else {
			_clients[fd] + message;
		}
	}
	std::istringstream input_str(line);
	std::string word;

	while(input_str >> word) {
		token.push_back(word);
		std::cout << "words :" << word << std::endl;
	}


}
