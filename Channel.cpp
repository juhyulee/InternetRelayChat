#include "Channel.hpp"
#include "Client.hpp"

// -------------------------------------------------------------------------------->>
// Orthodox Canonical Form
// -------------------------------------------------------------------------------->>

// Channel::Channel() {
// 	this->initialize();
// }

// Channel::Channel(const Channel& copy) {}

// Channel& Channel::operator=(const Channel& assign) {
// 	if (this != &assign) {
// 		this->_name = assign.getChannelName();
// 		this->_password = assign.getChannelPassword();
// 		this->_topic = assign.getChannelTopic();
// 		this->_mode = assign.getChannelMode();
// 		this->_user_limit = assign.getUserLimit();
// 		this->_user_list = assign.getUserList();
// 		this->_invite_list = assign.getInviteList();
// 		this->_operator = assign.getChannelOperator();
// 	}
// 	return *this;
// }

Channel::Channel(const std::string& name, Client *client) :_name(name) {
	this->initialize();
	this->_operator.insert(std::make_pair(client->getSocketFd(), client));
};

Channel::~Channel() {};

//-------------------------------------------------------------------------------->>
//initialize
//-------------------------------------------------------------------------------->>

void	Channel::initialize(){
	this->_password = "";
	this->_operator = std::map<int, Client *>();
	this->_topic = "";
	this->_mode = std::set<char>();
	this->_user_limit = 3;
	this->_user_list = std::map<int, Client *>();
	this->_invite_list = std::map<int, Client *>();
};


//-------------------------------------------------------------------------------->>
//getter
//-------------------------------------------------------------------------------->>

const std::string&	Channel::getChannelName() const{
	return _name;
}

const std::string&	Channel::getChannelPassword() const{
	return _password;
}

const std::string&	Channel::getChannelTopic() const{
	return _topic;
}

const std::set<char>&	Channel::getChannelMode() const{
	return _mode;
}

int Channel::getUserLimit() const{
	return _user_limit;
}

int Channel::getUserCount() const{
	return _user_list.size();
}

const std::map<int, Client *>&	Channel::getChannelOperator() const{
	return _operator;
}

// std::string	Channel::getSymbol(){
	// return this->_symbol;
// }



//-------------------------------------------------------------------------------->>
//setter
//-------------------------------------------------------------------------------->>
void 	Channel::setChannelName(std::string new_name){
	_name = new_name;
}

void 	Channel::setChannelPassword(std::string new_password){
	_password = new_password;
}

void 	Channel::deleteChannelPassword(){
	_password.erase();
}

void	Channel::setChannelTopic(std::string new_topic){
	_topic = new_topic;
}

std::vector<std::string>*	Channel::setChannelMode(Server &server, std::vector<std::string> token){
	int switch_mode = 0;
	(void)server;///???
	std::set<char> mode = this->getChannelMode();
	std::vector<std::string> *mode_params = new std::vector<std::string>;
	if (token[2][0] == '+') {
		switch_mode = 0;
	}
	else if (token[2][0] == '-') {
		switch_mode = 1;
	}
	/*
	* 초대 전용 채널로 설정 (i)
	* 채널 오퍼레이터만 토픽을 설정할 수 있음 (t)
	*/
	else if (token[2][1] == 'i' || token[2][1] == 't') {
		if (switch_mode == 0) {
			if (mode.find(token[2][1]) == mode.end()) {
				mode.insert(token[2][1]);
				mode_params->push_back(token[2]);
			}
			else {
				return NULL;
			}
		}
		else if (switch_mode == 1) {
			if (mode.find(token[2][1]) != mode.end()) {
				mode.erase(token[2][1]);
				mode_params->push_back(token[2]);
			}
			else {
				return NULL;
			}
		}
	}
	/*
	* 채널 패스워드 설정 (k)
	* 채널 오퍼레이터 권한줌 (o)
	* 채널 유저 수 제한 (l)
	*/
	else if (token[2][1] == 'k' || token[2][1] == 'o' || token[2][1] == 'l') {
		if (token.size() < 4) {
			return NULL;
		}
		std::map<int, Client *> channel_operator = this->getChannelOperator();
		if (switch_mode == 0) {
			if (token[2][1] == 'o') {
				Client *new_operator = findChannelUser(token[3]);
				if (new_operator == NULL) {
					// 오퍼레이터 추가 시 채널 유저 목록에 없을 경우 에러 처리
					return NULL;
				}
				if (addChannelOperator(new_operator) == true) {
					mode_params->push_back(token[2]);
					mode_params->push_back(token[3]);
				}
				else {
					// 이미 오퍼레이터인 경우
					return NULL;
				}
			}
			else if (mode.find(token[2][1]) == mode.end()) {
				mode.insert(token[2][1]);
				// 해당 멤버에 저장
				if (token[2][1] == 'k') {
					this->setChannelPassword(token[3]);
				}
				else if (token[2][1] == 'l') {
					this->setUserLimit(std::atoi(token[3].c_str()));
				}
				// reply params(command, parameter) 저장
				mode_params->push_back(token[2]);
				mode_params->push_back(token[3]);
			}
			else {
				return NULL;
			}
		}
		else if (switch_mode == 1) {
			if (token[2][1] == 'o') {
				Client *old_operator = findChannelOperator(token[3]);
				if (old_operator == NULL) {
					// 오퍼레이터 삭제 시 채널 오퍼레이터 목록에 없을 경우 에러 처리
					return NULL;
				}
				if (deleteChannelOperator(old_operator) == true) {
					mode_params->push_back(token[2]);
					mode_params->push_back(token[3]);
				}
				else {
					return NULL;
				}
			}
			else if (mode.find(token[2][1]) != mode.end()) {
				mode.erase(token[2][1]);
				// parameter 초기화
				if (token[2][1] == 'k') {
					this->deleteChannelPassword();
				}
				else if (token[2][1] == 'l') {
					this->setUserLimit(100);
				}
				// reply 목록(command) 저장
				mode_params->push_back(token[2]);
			}
			else {
				return NULL;
			}
		}
	}
	else {
		return NULL;
	}
	return mode_params;
	// RPL__mode
	// for (std::map<int,Client>::iterator iter = this->usrlist.begin();
	// iter != this->usrlist.end(); iter++) {
		// send_msg(RPL_MODE(this->usrlist[fd].getPrefix(), token[1], mode_params.at(0), mode_params.at(1)), iter->first);
	// }
}

void 	Channel::setUserLimit(int new_limits){
	_user_limit = new_limits;
}



//-------------------------------------------------------------------------------->>
//Msg
//-------------------------------------------------------------------------------->>

void 	Channel::broadcastChannelMessage(int send_fd, std::string message) {
	for (std::map<int, Client *>::iterator iter = _user_list.begin(); \
		iter != _user_list.end(); iter++) {
			if (iter->second->getSocketFd() == send_fd)
				continue ;
			// sendmsg --> 서버 필요한지?
		}
}



//-------------------------------------------------------------------------------->>
//operator
//-------------------------------------------------------------------------------->>

bool	Channel::isChannelOperator(Client *client) { // 오퍼레이터 목록에 있는지 확인
	if (_operator.find(client->getSocketFd()) != _operator.end()) {
		return true;
	}
	return false;
}

Client	*Channel::findChannelOperator(std::string nickname) {
	for (std::map<int, Client *>::iterator iter = _operator.begin(); \
		iter != _operator.end(); iter++) {
			if (iter->second->getNickname() == nickname)
				return iter->second;
		}
	return NULL;
}

bool	Channel::addChannelOperator(Client *new_operator) {
	if (isChannelOperator(new_operator) == false) {
		_operator.insert(std::make_pair(new_operator->getSocketFd(), new_operator));
		return true;
	}
	return false;
}

bool	Channel::deleteChannelOperator(Client *old_operator) {
	if (isChannelOperator(old_operator) == true) {
		_operator.erase(old_operator->getSocketFd());
		return true;
	}
	return false;
}



//-------------------------------------------------------------------------------->>
//user
//-------------------------------------------------------------------------------->>

bool	Channel::isChannelUser(Client *client) { // 유저 목록에 있는지 확인
	if (_user_list.find(client->getSocketFd()) != _user_list.end()) {
		return true;
	}
	return false;
}

Client	*Channel::findChannelUser(std::string nickname) {
	for (std::map<int, Client *>::iterator iter = _user_list.begin(); \
	iter != _user_list.end(); iter++) {
		if (iter->second->getNickname() == nickname)
			return iter->second;
	}
	return NULL;
}

bool	Channel::addChannelUser(Client *client){  //유저 채널에 추가하는 함수
	if (isChannelUser(client) == false) {
		_user_list.insert(std::make_pair(client->getSocketFd(), client));
		return true;
	}
	return false;
}

bool	Channel::deleteChannelUser(Client *client) { //유저 목록에서 지우는 함수
	if (isChannelUser(client) == true) {
		_user_list.erase(client->getSocketFd());
		return true;
	}
	return false;
}

bool	Channel::isInvitedUser(Client *client) {  // 초대 목록에 있는지 확인
	if (_invite_list.find(client->getSocketFd()) != _invite_list.end()) {
		return true;
	}
	return false;
}

Client	*Channel::findInviteUser(std::string nickname) {
	for (std::map<int, Client *>::iterator iter = _invite_list.begin(); \
	iter != _invite_list.end(); iter++) {
		if (iter->second->getNickname() == nickname)
			return iter->second;
	}
	return NULL;
}

bool	Channel::addInviteUser(Client *client){ //채널에 유저 초대
	if (isChannelUser(client) == false && isInvitedUser(client) == false) {
		_invite_list.insert(std::make_pair(client->getSocketFd(), client));
		return true;
	}
	return false;
}

bool	Channel::deleteInviteUser(Client *client){ //채널에 초대된 유저 지우기
	if (isInvitedUser(client) == true) {
		_invite_list.erase(client->getSocketFd());
		return true;
	}
	return false;
}

//-------------------------------------------------------------------------------->>
//check
//-------------------------------------------------------------------------------->>

int	Channel::checkUserLimit() {
	if (_user_list.size() < this->_user_limit) {
		return 0;
	}
	return -1;
}

int	Channel::checkInvite(Client *client) {
	if (isInvitedUser(client) == true) {
		return 0;
	}
	return -1;
}

int	Channel::checkPassword(std::string password) {
	if (_password != "" && _password != password) {
		return -1;
	}
	return 0;
}
