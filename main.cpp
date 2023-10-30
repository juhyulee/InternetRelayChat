#include "util.h"
#include "Server.hpp"

// void exit_with_perror(const std::string& msg) {
// 	std::cerr << msg << std::endl;
// 	exit(EXIT_FAILURE);
// }

// void change_events(std::vector<struct kevent>& change_list, uintptr_t ident, \
// int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata) {
// 	struct kevent temp_event;

// 	EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
// 	change_list.push_back(temp_event);
// }

// void disconnect_client(int client_fd, std::map<int, std::string>& clients) {
// 	std::cout << "client disconnected: " << client_fd << std::endl;
// 	close(client_fd);
// 	clients.erase(client_fd);
// }

int main(int argc, char **argv) {
	Server server;
	if (argc != 3) {
		std::cout << "argument error : port" << "pass" << std::endl;
		return 1;
	}

	if (atoi(argv[1]) == 0) {
		std::cout << "did you really input 0? well.. I'll just look at it this time" << std::endl;
	}
	if (atoi(argv[1]) > 65535) {
		std::cout << "but i can't watch this" << std::endl;
		return 1;
	}
	server.serverInit(argc, argv);

	return 0;
}
