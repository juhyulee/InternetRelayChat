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
#include "server.hpp"
#include <vector>
#include <map>

#define FD_ISSET(n,p) __DARWIN_FD_ISSET(n, p)
#define BUFFER_SIZE 1024

int main()
{
	int server_sock, client_sock;
	struct sockaddr_in server_addr, client_addr;
	struct timeval timeout;
	fd_set reads, cpy_reads;
	
	socklen_t addr_size;
	int fd_max, str_len, fd_num;

	int option = 1;

	server_sock = socket(PF_INET, SOCK_STREAM, 0);
	setsockopt(server_sock, SOL_SOCKET,SO_REUSEADDR, &option, sizeof(option));
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(8080);

	if (bind(server_sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) ==  -1)
	{
		std::cout << "bind err" << std::endl;
		close(server_sock);
		exit(1);
	}
	if (listen(server_sock, 5) == -1)
	{
		std::cout << "listen err" << std::endl;
		close(server_sock);
		exit(1);
	}
	FD_ZERO(&reads);
	FD_SET(server_sock, &reads);
	fd_max = server_sock;

	while (1)
	{
		cpy_reads = reads;
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;
		char * buf[BUFFER_SIZE] = {0};
		
		if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1 )
			break;
		if (fd_num == 0)
			continue;
		for (int i  = 0; i < fd_max + 1; i++)
		{
			if (FD_ISSET(i, &cpy_reads))
			{
				std::cout << "new connect : " << i  <<std::endl;
				if (i == server_sock)
				{
					addr_size = sizeof(client_addr);
					client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
					FD_SET(client_sock, &reads);
					if (fd_max < client_sock)
						fd_max = client_sock;
					std::cout << "connected client :" << client_sock << std::endl;
				}
				else
				{
					str_len = recv(i, buf, BUFFER_SIZE, 0);
					buf[str_len] = 0;
					if (str_len == 0){
						FD_CLR(i, &reads);
						close(i);
						std::cout << "closed client : " << i << std::endl;
					}
					std::cout << "client msg :" << buf <<std::endl;
				}
			}
		}
	}
}