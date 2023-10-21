#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "util.h"

class Channel;

class Client {
	public :
		Client(int socket_fd, std::string username, std::string hostname, \
			std::string realname, std::string user_ip);
		~Client();

		// Getter
		int					getSocketFd() const;
		const std::string&	getNickname() const;
		const std::string&	getUsername() const;
		const std::string&	getHostname() const;
		const std::string&	getRealname() const;
		const std::string&	getUserIp() const;
		const std::map<std::string, Channel *>&	getChannelList() const;

		// Setter
		void	setSocketFd(int socket_fd);
		void	setNickname(std::string nickname);
		void	setUsername(std::string username);
		void	setHostname(std::string hostname);
		void	setRealname(std::string realname);
		void	setUserIp(std::string user_ip);

		std::string	getPrefix() const;

	private :
		int _socket_fd;
		std::string _nickname;
		std::string _username;
		std::string _hostname;
		std::string _realname;
		std::string _user_ip; // 유저의 IP주소
		std::map<std::string, Channel *> _channel_list; // 유저가 가입한 채널 목록

		/*
		* 생성자, 복사생성자, 할당연산자는 private으로,
		* 구현이 필요하다면 cpp파일에서 주석을 해제하여 사용
		*/
		Client();
		Client(const Client& copy);
		Client& operator=(const Client& obj);

		/*
		* Server에서는 이 함수를 통해 유저가 인증되었는지 확인할 계획.
		* Server class로 이관 예정
		*/

		/*
		* int	getAuth(); //pass 처리 -1불가 0가능
		*/

		/*
		* 유저가 가입할 수 있는 최대 채널 수가 아니고 채널의 최대 유저 수.
		* 다시 말해 채팅방 인원 수 제한임.
		* Channel class로 이관
		*/

		/*
		* int _channel_limit;
		*
		* int	getChannelLimit() const;
		* void	setChannelLimit(int new_limit);
		* int	checkChannelLimit() const; //-1불가 0가능 : 유저가 가입할 수 있는 최대 채널 갯수 초과 체크
		*/
};

#endif
