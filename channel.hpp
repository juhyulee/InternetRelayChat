#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "util.h"

class Server;

class Channel {
	private :
		std::string channelname; // 채널이름
		std::string channelpassword; //채널비밀번호
		std::string channeloperator; //채널오퍼레이터
		std::string channeltopic; //채널토픽
		std::vector<std::string> channelmod; //채널모드
		int usrcnt; //유저 인원수
		int usrlimits; //최대 유저 인원수
	public :
		Channel();
		std::map<int, Client> usrlist; //유저목록
		void setchannelname(std::string chanellname);
		void setchannelpassword(std::string password);
		void setchanneloperator(std::string newoperator);
		void setchanneltopic(std::string newtopic);
		void setchannelmode(std::string mod);
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄

		void adduser(int fd, Client client); //유저 채널에 추가하는 함수
		void deleteuser(int fd);//유저 usrlist에서 지우는 함수
		void inviteuser(int fd, Client client);//채널에 유저 초대하는 함수
};

#endif
