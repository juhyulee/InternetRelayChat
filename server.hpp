#ifndef SERVER_HPP
# define SERVER_HPP

# include "util.h"

class Server {
	private :
		int _server_socket;
		struct sockaddr_in _server_addr;
		std::map<int, std::string> _clients;
		std::vector<struct kevent> _change_list;
		struct kevent _event_list[8];
		int _new_events;
		struct kevent* _curr_event;
		std::map<int, std::string> _send_data; //전송할 데이터
		std::map<int, std::string> _recv_data; //읽은 데이터
	public :
		void serverInit(int argc, char **argv);
		void changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, \
		int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void disconnectClient(int client_fd, std::map<int, std::string>& clients);
		void parsingData(std::string message);
		void sendMessage(std::string message, int fd); //메세지 보내는 함수
};
//메세지 보내는 함수
//메세지 받는 함수

#endif
