#include "Channel.hpp"
#include "Client.hpp"
#include "message.h"
#include <sstream>
#include <stdlib.h>

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
	this->addChannelUser(client);
	this->addChannelOperator(client);
}

Channel::~Channel() {}

//-------------------------------------------------------------------------------->>
// Initialize
//-------------------------------------------------------------------------------->>

void	Channel::initialize(){
	this->_password = "";
	this->_operator = std::map<int, Client *>();
	this->_topic = "";
	this->_mode = std::set<char>();
	this->_user_limit = 3;
	this->_user_list = std::map<int, Client *>();
	this->_invite_list = std::map<int, Client *>();
}


//-------------------------------------------------------------------------------->>
// Exception
//-------------------------------------------------------------------------------->>

Channel::ChannelModeException::ChannelModeException(const std::string& message){
	_message = message;
}

Channel::ChannelModeException::~ChannelModeException() throw() {}

const char	*Channel::ChannelModeException::what() const throw() {
	return _message.c_str();
}

//-------------------------------------------------------------------------------->>
// Getter
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

size_t	Channel::getUserLimit() const{
	return _user_limit;
}

std::map<int, Client *>::size_type	Channel::getUserCount() const{
	return _user_list.size();
}

const std::map<int, Client *>&	Channel::getUserList() const{
	return _user_list;
}


const std::map<int, Client *>&	Channel::getInviteList() const{
	return _invite_list;
}

const std::map<int, Client *>&	Channel::getChannelOperator() const{
	return _operator;
}

const std::string Channel::getUserNameList() {
	std::string res;
	for (std::map <int, Client *> ::iterator iter = _user_list.begin();\
	iter != _user_list.end(); iter++){
		if (isChannelOperator(iter->second) == true){
			res += "@";
		}
		res += iter->second->getNickname();
		res += " ";
	}
	return (res);
}

//-------------------------------------------------------------------------------->>
// Setter
//-------------------------------------------------------------------------------->>

void	Channel::setChannelName(std::string new_name){
	_name = new_name;
}

void	Channel::setChannelPassword(std::string new_password){
	_password = new_password;
}

void	Channel::removeChannelPassword(){
	_password.erase();
}

void	Channel::setChannelTopic(std::string new_topic){
	_topic = new_topic;
}

std::vector<std::string>	*Channel::setChannelMode(std::vector<std::string> token, Client *client){
	if (token[2][0] != '+' && token[2][0] != '-') {
		throw ChannelModeException(ERR_NOSUCHNICK(client->getNickname(), token[2]));
	}
	// 설정하려는 모드가 없음
	else if (token[2].length() == 1) {
		throw ChannelModeException("hi");
	}
	// 설정하려는 모드가 여러개임
	else if (token[2].length() > 2) {
		throw ChannelModeException("hi");
	}
	int switch_mode = 0;
	if (token[2][0] == '-') {
		switch_mode = 1;
	}
	/*
	* 초대 전용 채널로 설정 (i)
	* 채널 오퍼레이터만 토픽을 설정할 수 있음 (t)
	*/
	std::vector<std::string> *mode_params = new std::vector<std::string>;
	if (token[2][1] == 'i' || token[2][1] == 't') {
		if (switch_mode == 0) {
			if (_mode.find(token[2][1]) != _mode.end()) {
				return NULL;
			}
			else {
				_mode.insert(token[2][1]);
				mode_params->push_back(token[2]);
			}
		}
		else if (switch_mode == 1) {
			if (_mode.find(token[2][1]) == _mode.end()) {
				return NULL;
			}
			else {
				_mode.erase(token[2][1]);
				mode_params->push_back(token[2]);
			}
		}
	}
	/*
	* 채널 패스워드 설정 (k)
	* 채널 오퍼레이터 권한줌 (o)
	* 채널 유저 수 제한 (l)
	*/
	else if (token[2][1] == 'o' || token[2][1] == 'k' || token[2][1] == 'l') {
		if (token.size() < 4) {
			if (token[2][1] == 'o') {
				throw ChannelModeException(ERR_NOOPPARAM(client->getNickname(), _name, token[2][1], "op", "nick"));
			}
			else if (token[2][1] == 'k') {
				throw ChannelModeException(ERR_NOOPPARAM(client->getNickname(), _name, token[2][1], "key", "key"));
			}
			else {
				throw ChannelModeException(ERR_NOOPPARAM(client->getNickname(), _name, token[2][1], "limit", "limit"));
			}
		}
		if (switch_mode == 0) {
			if (token[2][1] == 'o') {
				Client *new_operator = findChannelUser(token[3]);
				// 오퍼레이터 추가 시 채널 유저 목록에 없을 경우 에러 처리
				if (new_operator == NULL) {
					throw ChannelModeException(ERR_USERNOTINCHANNEL(client->getNickname(), token[3], _name));
				}
				if (addChannelOperator(new_operator) == false) {
					// 이미 오퍼레이터인 경우 (변경 없음, 응답 또한 없음)
					return NULL;
				}
				else {
					mode_params->push_back(token[2]);
					mode_params->push_back(token[3]);
				}
			}
			else if (_mode.find(token[2][1]) == _mode.end()) {
				_mode.insert(token[2][1]);
				// 해당 멤버에 저장
				if (token[2][1] == 'k') {
					// 채널 비밀번호 20자 제한 있다고 함. 동작 확인 필요
					if (token[3].length() > 20) {
						throw ChannelModeException(ERR_LONGPWD(client->getNickname(), _name));
					}
					setChannelPassword(token[3]);
				}
				else if (token[2][1] == 'l') {
					setUserLimit(std::atoi(token[3].c_str()));
				}
				// reply params(command, parameter) 저장
				mode_params->push_back(token[2]);
				mode_params->push_back(" " + token[3]);
			}
			else {
				// 변경하려는 모드(k, l)가 이미 적용된 경우 동작 확인 필요
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
				if (removeChannelOperator(old_operator) == true) {
					mode_params->push_back(token[2]);
					mode_params->push_back(" " + token[3]);
				}
				else {
					return NULL;
				}
			}
			else if (_mode.find(token[2][1]) != _mode.end()) {
				_mode.erase(token[2][1]);
				// parameter 초기화
				if (token[2][1] == 'k') {
					removeChannelPassword();
				}
				else if (token[2][1] == 'l') {
					setUserLimit(100);
				}
				// reply 목록(command) 저장
				mode_params->push_back(token[2]);
			}
			else {
			// 변경하려는 모드(k, l)가 이미 적용되지 않은 경우 동작 확인 필요
				return NULL;
			}
		}
	}
	else {
		throw ChannelModeException(ERR_UNKNOWNMODE(client->getNickname(), token[2][1]));
	}
	return mode_params;
}

void	Channel::setUserLimit(int new_limits){
	_user_limit = new_limits;
}

//-------------------------------------------------------------------------------->>
// Mode
//-------------------------------------------------------------------------------->>

void	Channel::clearChannelMode(){
	_mode.clear();
}

bool Channel::checkChannelMode(char c)
{
	if (_mode.find(c) == _mode.end())
		return false;
	return true;
}

std::vector<std::string>	*Channel::getChannelModeParams() const{
	std::vector<std::string>	*mode_params = new std::vector<std::string>;
	std::string 				modes;
	for (std::set<char>::iterator iter = _mode.begin(); \
	iter != _mode.end(); iter++) {
		modes.push_back(*iter);
	}
	mode_params->push_back(modes);
	std::string	params;
	for (std::string::size_type i = 0; i < modes.length(); i++) {
		if (modes[i] == 'k') {
			params += " " + _password;
		}
		else if (modes[i] == 'l') {
			std::stringstream ss;
			ss << _user_limit;
			params += " " + ss.str();
		}
	}
	mode_params->push_back(params);
	return mode_params;
}

//-------------------------------------------------------------------------------->>
// Operator
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

bool	Channel::removeChannelOperator(Client *old_operator) {
	if (isChannelOperator(old_operator) == true) {
		_operator.erase(old_operator->getSocketFd());
		return true;
	}
	return false;
}

void	Channel::clearChannelOperator(){
	_operator.clear();
}

//-------------------------------------------------------------------------------->>
// User
//-------------------------------------------------------------------------------->>

bool	Channel::isChannelUser(Client *client) { // 유저 목록에 있는지 확인
	if (_user_list.find(client->getSocketFd()) != _user_list.end())
			return true ;
	return false;
}

bool	Channel::isChannelUser(std::string nickname) { // 유저 목록에 있는지 확인
	for (std::map<int, Client *>::iterator iter = _user_list.begin(); \
	iter != _user_list.end(); iter++) {
		if (iter->second->getNickname() == nickname)
			return true;
	}
	return false;
}


Client	*Channel::findChannelUser(std::string nickname) { // 유저 목록에서 찾아서 유저 리턴 / 없을경우 NULL
	for (std::map<int, Client *>::iterator iter = _user_list.begin(); \
	iter != _user_list.end(); iter++) {
		if (iter->second->getNickname() == nickname)
			return iter->second;
	}
	return NULL;
}

Client	*Channel::findChannelUser(Client *client) { // 유저 목록에서 찾아서 유저 리턴 / 없을경우 NULL
	for (std::map<int, Client *>::iterator iter = _user_list.begin(); \
	iter != _user_list.end(); iter++) {
		if (iter->second == client)
			return iter->second;
	}
	return NULL;
}


bool	Channel::addChannelUser(Client *client){  //유저 채널에 추가하는 함수
	Client *user = findChannelUser(client);
	if (!user) {
		_user_list.insert(std::make_pair(client->getSocketFd(), client));
		return true;
	}
	return false;
}

bool	Channel::removeChannelUser(Client *client) { //유저 목록에서 지우는 함수
	Client *user = findChannelUser(client);
	if (user) {
		_user_list.erase(client->getSocketFd());
		std::cout << client->getNickname() << "is parted from channel "<< _name << std::endl;
		return true;
	}
	return false;
}

void	Channel::clearUserList(){
	_user_list.clear();
}

//-------------------------------------------------------------------------------->>
// Invite
//-------------------------------------------------------------------------------->>

bool	Channel::isInvitedUser(Client *client) {  // 초대 목록에 있는지 확인
	if (_invite_list.find(client->getSocketFd()) != _invite_list.end()) {
		return true;
	}
	return false;
}

Client	*Channel::findInvitedUser(std::string nickname) {
	for (std::map<int, Client *>::iterator iter = _invite_list.begin(); \
	iter != _invite_list.end(); iter++) {
		if (iter->second->getNickname() == nickname)
			return iter->second;
	}
	return NULL;
}

bool	Channel::addInvitedUser(Client *client){ //채널에 유저 초대
	if (isChannelUser(client) == false && isInvitedUser(client) == false) {
		_invite_list.insert(std::make_pair(client->getSocketFd(), client));
		return true;
	}
	return false;
}

bool	Channel::removeInvitedUser(Client *client){ //채널에 초대된 유저 지우기
	if (isInvitedUser(client) == true) {
		_invite_list.erase(client->getSocketFd());
		return true;
	}
	return false;
}

void	Channel::clearInviteList(){
	_invite_list.clear();
}

//-------------------------------------------------------------------------------->>
// Check
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

int	Channel::checkInvite(int fd) {
	if (_invite_list.find(fd) != _invite_list.end()) {
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
