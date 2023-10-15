#include "channel.hpp"

Channel::Channel() {
	usrcnt = 0;
	usrlimits = 3;
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

Client *Channel::search_user(std::string nickname) {
	for (std::map<int, Client>::iterator iter = this->usrlist.begin(); iter != this->usrlist.end(); ++iter) {
		if (iter->second.nickname == nickname)
		return &iter->second;
	}
	return NULL;
}

void Channel::deleteuser(int fd) {//유저 usrlist에서 지우는 함수

	if (usrlist.find(fd) != usrlist.end()) {
		usrlist.erase(usrlist.find(fd));
		usrcnt--;
	}
	else {
		//에러메세지 전송
	}
}

void Channel::inviteuser(int fd, Client client) {//채널에 유저 초대하는 함수
	adduser(fd,client);
}

std::string Channel::getchannelname() const {
	return this->channelname;
}

std::string Channel::getchannelpassword() const {
	return this->channelpassword;
}

std::string Channel::getchanneloperator() const {
	return this->channeloperator;
}

std::string Channel::getchanneltopic() const {
	return this->channeltopic;
}

std::set<char> Channel::getchannelmode() const {
	return this->channelmode;
}

int	Channel::getusrcnt() const {
	return this->usrcnt;
}

int	Channel::getusrlimits() const {
	return this->usrlimits;
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
void Channel::setchannelmode(std::string mode) {
	int switch_mode = 0;
	std::set<char> *channelmode = &this->channelmode;
	std::set<char> setmode;
	for (int i = 0; i < mode.size(); i++) {
		if (mode[i] == '+') {
			switch_mode = 0;
		}
		else if (mode[i] == '-') {
			switch_mode = 1;
		}
		else if (mode[i] == 'i') { //초대만 가능한 채널로 설정
			if (switch_mode == 0) {
				if (channelmode->find('i') == channelmode->end()) {
					channelmode->insert(mode[i]);
				}
			}
			else if (switch_mode == 1) {
				if (channelmode->find('i') != channelmode->end()) {
					channelmode->erase(mode[i]);
				}
			}
		}
		else if (mode[i] == 't') { //채널 오퍼레이터만 토픽을 설정할 수 있음
		}
		else if (mode[i] == 'k') { //채널 패스워드 설정
		}
		else if (mode[i] == 'o') { //채널 오퍼레이터 권한줌
		}
		else if (mode[i] == 'l') { //채널 유저 수 제한
		}
	}
}
//특정 유저한테 보낼 메세지 서버로 보냄
//전체 유저한테 보낼 메세지 서버로 보냄
