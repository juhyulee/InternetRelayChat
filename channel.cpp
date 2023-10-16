#include "channel.hpp"
#include <cstdlib>

Channel::Channel() {
	usrcnt = 0;
	usrlimits = 3;
	channeltopic = "";
	channeloperator = "";
	channelpassword = "";
	channelname = "";
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

const std::string& Channel::getchannelname() const {
	return this->channelname;
}

const std::string& Channel::getchannelpassword() const {
	return this->channelpassword;
}

const std::string& Channel::getchanneloperator() const {
	return this->channeloperator;
}

const std::string& Channel::getchanneltopic() const {
	return this->channeltopic;
}

const std::set<char>& Channel::getchannelmode() const {
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
	this->channeloperator.push_back(newoperator);
}
void Channel::setchanneltopic(std::string newtopic) {
	this->channeltopic = newtopic;
}
void Channel::setchannelmode(std::vector<std::string> token) {
	int switch_mode = 0;
	std::set<char> *channelmode = &this->channelmode;
	std::string set_mode;
	std::string params;
	if (token[2][0] == '+') {
		switch_mode = 0;
	}
	else if (token[2][0] == '-') {
		switch_mode = 1;
	}
	//초대만 가능한 채널로 설정 (i)
	//채널 오퍼레이터만 토픽을 설정할 수 있음 (t)
	else if (token[2][1] == 'i' || token[2][1] == 't') {
		if (switch_mode == 0) {
			if (channelmode->find(token[2][1]) == channelmode->end()) {
				channelmode->insert(token[2][1]);
				set_mode = token[2];
			}
		}
		else if (switch_mode == 1) {
			if (channelmode->find(token[2][1]) != channelmode->end()) {
				channelmode->erase(token[2][1]);
				set_mode = token[2];
			}
		}
	}
	// 채널 패스워드 설정 (k)
	// 채널 오퍼레이터 권한줌 (o)
	// 채널 유저 수 제한 (l)
	else if (token[2][1] == 'k' || token[2][1] == 'o' || token[2][1] == 'l') {
		if (switch_mode == 0) {
			if (channelmode->find(token[2][1]) == channelmode->end()) {
				channelmode->insert(token[2][1]);
				// parameter 저장
				if (token[2][1] == 'k') {
					this->setchannelpassword(token[3]);
				}
				else if (token[2][1] == 'o') {
					;
				}
				else if (token[2][1] == 'l') {
					this->setusrlimits(std::atoi(token[3].c_str()));
				}
				// reply 목록(command, parameter) 저장
				set_mode = token[2];
				params = token[3];
			}
		}
		else if (switch_mode == 1) {
			if (channelmode->find(token[2][1]) != channelmode->end()) {
				channelmode->erase(token[2][1]);
				// parameter 제거
				if (token[2][1] == 'k') {
					setchannelpassword("");
				}
				else if (token[2][1] == 'o') {
					;
				}
				else if (token[2][1] == 'l') {
					setusrlimits(100);
				}
				// reply 목록(command) 저장
				set_mode = token[2];
			}
		}
	}
	else {
		// Invalid MODE
	}
	// RPL_CHANNELMODE
	for (std::map<int,Client>::iterator iter = this->usrlist.begin();
	iter != this->usrlist.end(); iter++) {
		send_msg(RPL_MODE(this->usrlist[fd].getPrefix(), token[1], disp_mode, params), iter->first);
	}
}
//특정 유저한테 보낼 메세지 서버로 보냄
//전체 유저한테 보낼 메세지 서버로 보냄

void Channel::setusrlimits(int new_limits) {
	this->usrlimits = new_limits;
}
