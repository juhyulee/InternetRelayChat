#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <set>
#include <cstdlib>
#include <vector>
#include <map>
#include <iostream>

class Server;

class Channel {
	private :
		std::string _name; // 채널이름
		std::string _password; //채널비밀번호
		std::map<std::string, Client> _operator; //채널오퍼레이터
		std::string _topic; //채널토픽
		std::set<char> _mode; //채널모드
		int _user_cnt; //유저 인원수
		int _user_limit; //최대 유저 인원수
		std::map<int, Client> _user_list; //유저목록
		std::map<int, Client> _user_invite_list; //초대된 유저목록

		Channel(const Channel & copy);
		const Channel & operator=(const Channel & assign);
		void	initChannel();

	public :
		Channel();
		Channel(std::string name);
		Channel(std::string name, Client & new_user);
		~Channel();

		//getter
		const std::string&			getChannelName() const;
		const std::string&			getChannelPassword() const;
		// const std::map<std::string, Client>& getChannelOperator() const; >> search로 변경, operator로 내림
		const std::string&			getChannelTopic() const;
		const std::set<char>&		getChannelMode() const;
		int 						getUserCnt() const;
		int 						getUserLimit() const;

		//setter
		void 						setChannelName(std::string new_name);
		void 						setChannelPassword(std::string new_password);
		void 						deleteChannelPassword();
		void  						setChannelTopic(std::string new_topic);
		std::vector<std::string>	*setChannelMode(Server &server, std::vector<std::string> token);
		void 						setUserLimit(int new_limits);
		
		//msg
		void 						broadcastChannelMessage(int send_fd, std::string message);// 채널에 메세지 보내는 함수
		// ^^^ send_fd == -1 자기자신 포함, 유저일 때는 비포함
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄

		//operator
		Client 		 				*searchChannelOperator(std::string nickname) const;
		void 						addChannelOperator(Client new_operator);
		void  						deleteChannelOperator(Client operator);


		//user
		Client  					*searchChannelUser(std::string nickname); // 유저 목록에 있는지 확인
		Client  					*searchChannelInvite(std::string nickname); // 초대되었는지 확인
		void 						addChannelUser(int fd, Client client); //유저 채널에 추가하는 함수
		void 						deleteChannelUser(int fd);//유저 usrlist에서 지우는 함수
		void 						inviteChannelUser(int fd, Client client);//채널에 유저 초대하는 함수
};

#endif
