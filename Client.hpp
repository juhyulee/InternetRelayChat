#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "util.h"

class Client {
	private :
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _servername; //ip주소
		std::string _realname;
		int _socket_fd;

	public :
		Client();
		Client(std::string username, std::string hostname, std::string servername,
		std::string realname, int fd);


		//getter
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getHostname() const;
		const std::string&	getServername() const;
		const std::string&	getRealname() const;
		int			getSocketFd() const;
		int			getAuth();

		int			checkChannelLimit(); //-1불가 0가능 : 유저가 가입할 수 있는 최대 채널 갯수 초과 체크 

		std::string getPrefix() const;
};

#endif