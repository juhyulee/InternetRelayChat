#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "server.hpp"

class Server;

class Channel {
	private :
		std::map<std::string, int> usrlist;
		std::string usernick;
		std::vector<std::string> banlist;
		std::string channelpassword;
		int usrlimits;
		std::string channeloperator;
		std::string channeltopic;
		std::vector<std::string> channelmod;
	public :
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄
		//유저 usrlist에서 지우는 함수
		//유저 오퍼레이터 변경하는 함수
		//토픽 설정하는 함수
		//채널 모드 변경하는함수
		//채널에 유저 초대하는 함수
		//
};

#endif
