#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/types.h>
#include <sys/time.h>
#include <cstdint>

#include <vector>
#include <map>

#define FD_ISSET(n,p) __DARWIN_FD_ISSET(n, p)

void usage(char *argv){
    std::cout << "Usage : " << argv << " [port]" << " [password]" << std::endl;
    std::cout << "Example) " << argv << " 1234" << " 1234" <<  std::endl;
}

int main(int argc, char **argv) {
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	struct timeval timeout;
	fd_set reads, cpy_reads;

	socklen_t addr_size;
	int fd_max, str_len, fd_num, i;
	char buf[1024];
	if (argc != 3) {
		usage(argv[0]);
		exit(1);
	}

	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		std::cout << "bind error" << std::endl;
		close(server_sock);
		exit(1);
	}

	if (listen(server_sock, 5) == -1) {
		std::cout << "listen error" << std::endl;
		close(server_sock);
		exit(1);
	}

	FD_ZERO(&reads);
	FD_SET(server_sock, &reads);
	fd_max = server_sock;

	while (1) {
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;
		memset(buf, 0, 1024);

		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
			break;
		if (fd_num == 0)
			continue;
		for (i = 0; i < fd_max+1; i++) {
			if (FD_ISSET(i, &cpy_reads)) { //서버가 소켓 감지
				if (i == server_sock) { //신규 소켓이 들어올 경우
					addr_size = sizeof(client_addr);
					client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
					//nonblocking

					FD_SET(client_sock, &reads);
					if (fd_max << client_sock)
						fd_max = client_sock;
					std::cout << "connected client :" << client_sock << std::endl;
					// password_check(client_sock, "1234");
				}
				else{ // 클라이언트로부터 메세지가 들어올 경우
					memset(buf, 0, 1024);
					str_len = recv(i, buf, 1024, 0); //메세지 buf에 입력받음
					buf[str_len] = 0;
					std::cout << "buff : " << buf << std::endl;
					if(str_len == 0) { // 클라이언트 연결이 끊김
						FD_CLR(i, &reads);
						close(i);
						std::cout << "closed client : " << i << std::endl;
					}
					else { // 메세지 전송
						std::string msg = "Welcome to the Internet Relay Network";
						send(i, msg.c_str(), msg.size(), 0);
					}
				}
			}
		}
	}
	close(server_sock);
	return 0;
}
