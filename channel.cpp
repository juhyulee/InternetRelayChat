#include "channel.hpp"

Channel::Channel() {
	usrcnt = 0;
	usrlimits = 0;
}

void Channel::adduser(int fd, Client client) {
	usrcnt += 1;
	if (usrcnt > usrlimits) {
		return ; //유저수 초과 메세지 전송
	}
	if (usrcnt == 1) {
		setchanneloperator(client.nickname);
	}
	usrlist[fd] = client;
}

void Channel::deleteuser(int fd) {//유저 usrlist에서 지우는 함수

	if (usrlist.find(fd) != usrlist.end())
		usrlist.erase(usrlist.find(fd));
	else {
		//에러메세지 전송
	}
}

void Channel::inviteuser(int fd, Client client) {//채널에 유저 초대하는 함수
	adduser(fd,client);
}

std::string Channel::getchannelname() {
    return this->channelname;
}

int	Channel::getusrcnt() {
	return this->usrcnt;
}

void Channel::setchannelname(std::string chanellname) {
	this->channelname = chanellname;
}
void Channel::setchannelpassword(std::string password) {
	this->channelpassword = password;
}
void Channel::setchanneloperator(std::string newoperator) {
	this->channeloperator = newoperator;
}
void Channel::setchanneltopic(std::string newtopic) {
	this->channeltopic = newtopic;
}
void Channel::setchannelmode(std::string mod) {
	if (mod == "i") { //초대만 가능한 채널로 설정
		if (channelmod[0] == "i")
			channelmod.erase(channelmod.begin() + 1);
		else
			channelmod[0] = "i";
	}
	else if (mod == "t") { //채널 오퍼레이터만 토픽을 설정할 수 있음
		if (channelmod[0] == "t")
			channelmod.erase(channelmod.begin() + 2);
		else
			channelmod[0] = "t";
	}
	else if (mod == "k") { //채널 패스워드 설정
		if (channelmod[0] == "k")
			channelmod.erase(channelmod.begin() + 3);
		else
			channelmod[0] = "k";
	}
	else if (mod == "o") { //채널 오퍼레이터 권한줌
		if (channelmod[0] == "o")
			channelmod.erase(channelmod.begin() + 4);
		else
			channelmod[0] = "o";
	}
	else if (mod == "l") { //채널 유저 수 제한
		if (channelmod[0] == "l")
			channelmod.erase(channelmod.begin() + 5);
		else
			channelmod[0] = "l";
	}
}
//특정 유저한테 보낼 메세지 서버로 보냄
//전체 유저한테 보낼 메세지 서버로 보냄
