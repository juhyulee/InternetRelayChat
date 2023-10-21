#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <set>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>

class Server;

class Client;

class Channel {
	private :
		std::string				_name;			// 채널 이름
		std::string				_password;		// 채널 비밀번호
		std::string				_topic;			// 채널 토픽
		std::set<char>			_mode;			// 채널 모드
		int						_user_limit;	// 최대 유저 인원수
		int						_user_count;	// 유저 인원수
		std::map<int, Client *>	_user_list;		// 유저 목록
		std::map<int, Client *>	_invite_list;	// 유저 초대 목록
		std::map<int, Client *>	_operator;		// 채널 오퍼레이터

		Channel();
		Channel(const Channel& copy);
		const Channel& operator=(const Channel& assign);
		void	initialize();


	public :
		Channel(const std::string& name, Client *client);
		~Channel();

		// Getter
		const std::string&				getChannelName() const;
		const std::string&				getChannelPassword() const;
		const std::string&				getChannelTopic() const;
		const std::set<char>&			getChannelMode() const;
		int 							getUserLimit() const;
		int 							getUserCount() const;
		const std::map<int, Client *>&	getUserList() const;
		const std::map<int, Client *>&	getInviteList() const;
		const std::map<int, Client *>&	getChannelOperator() const;

		// Setter
		void 						setChannelName(std::string new_name);
		void 						setChannelPassword(std::string new_password);
		void 						deleteChannelPassword();
		void  						setChannelTopic(std::string new_topic);
		std::vector<std::string>	*setChannelMode(Server &server, std::vector<std::string> token);
		void 						setUserLimit(int new_limits);

		//msg		보류: Server에서 처리하게 될 것 같음
		void	broadcastChannelMessage(int send_fd, std::string message);// 채널에 메세지 보내는 함수
		// ^^^ send_fd == -1 자기자신 포함, 유저일 때는 비포함
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄

		// Operator
		Client	*searchChannelOperator(std::string nickname);
		void	addChannelOperator(Client *new_operator);
		void	deleteChannelOperator(Client *new_operator);


		// User
		Client	*searchChannelUser(std::string nickname);	// 유저 목록에 있는지 확인
		Client	*searchChannelInvite(std::string nickname);	// 초대되었는지 확인
		void	addChannelUser(Client *client);				// 유저 채널에 추가하는 함수
		void	deleteChannelUser(Client *client);			// 유저 usrlist에서 지우는 함수
		void	inviteChannelUser(Client *client);			// 채널에 유저 초대하는 함수

		// Check
		int	checkPassword(std::string password);
		int	checkUserLimit() ;
		int	checkInvite(Client *client);
};

#endif
