#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "util.h"

class Client {
	private :
	public :
		std::string nickname;
		std::string username;
		std::string hostname;
		std::string servername; //ip주소
		std::string realname;
		int fd;
		int pass;
		Client();
		Client(std::string username, std::string hostname, std::string servername,
		std::string realname, int fd);

};

#endif
