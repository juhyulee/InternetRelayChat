#ifndef SERVER_HPP
#define SERVER_HPP

class Channel;

#include <vector>
#include <map>
#include <string>
#include <iostream>

class Server {
	private :
		std::string servername;
		std::vector<Channel* > clist;
		std::map<std::string, int> usrlist;
		std::string acceptmsg;
		std::string sendmsg;
		std::string serverpassword;
	public :
		//메세지 보내는 함수
		//메세지 받는 함수
		//채널 만들어주는 함수

};

#endif
