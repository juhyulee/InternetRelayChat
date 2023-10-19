#include "server.hpp"

//-------------------------------------------------------------------------------->>
//oxodox
//-------------------------------------------------------------------------------->>
void 	Channel::initChannel(){
	this->_name = "default";
	this->_password = "";
	this->_operator = std::map<std::string, Client> ();
	this->_topic = "";
	this->_mode = std::set<char>();
	this->_user_cnt = 0;
	this->_user_limit = 3;
	this->_user_list = std::map<int, Client>();
	this->_user_invite_list = std::map<int, Client>();
}

Channel::Channel() {
	this->initChannel();
}

Channel::Channel(std::string name) {
	this->initChannel();
	this->_name = name;
};

Channel::Channel(std::string name, Client & new_user){
	this->initChannel();
	this->_name = name;
	// this->_operator.insert(make_pair(new_user.getFd(), new_user.getNickname())); 
	//getFd getNickname 아직 없음
};

Channel::~Channel(){};



//-------------------------------------------------------------------------------->>
//getter
//-------------------------------------------------------------------------------->>
const std::string&			Channel::getChannelName() const{
	return (this->_name);
};
const std::string&			Channel::getChannelPassword() const{
	return (this->_password);
};
const std::string&			Channel::getChannelTopic() const{
	return (this->_topic);
};
const std::set<char>&		Channel::getChannelMode() const{
	return (this->_mode);
};
int 						Channel::getUserCnt() const{
	return (this->_user_cnt);
};
int 						Channel::getUserLimit() const{
	return (this->_user_limit);
};



//-------------------------------------------------------------------------------->>
//setter
//-------------------------------------------------------------------------------->>
void 						Channel::setChannelName(std::string new_name){
	this->_name = new_name;
};
void 						Channel::setChannelPassword(std::string new_password){
	this->_password = new_password;
};
void 						Channel::deleteChannelPassword(){
	this->_password = "";
	// mode 변경해야 하는지?
	// 서버에서 권한 확인할 것인지?
};
void  						Channel::setChannelTopic(std::string new_topic){
	this->_topic = new_topic;
};
std::vector<std::string>	Channel::*setChannelMode(Server &server, std::vector<std::string> token){
	//??
};
void 						Channel::setUserLimit(int new_limits){
	this->_user_limit = new_limits;
};



//-------------------------------------------------------------------------------->>
//Msg
//-------------------------------------------------------------------------------->>
void 						Channel::broadcastChannelMessage(int send_fd, std::string message){
	for (std::map<int, Client>::iterator iter = this->_user_list.begin(); \
		iter != this->_user_list.end(); iter++){
			if (iter->first == send_fd)
				continue ;
			// sendmsg --> 서버 필요한지?
		}
};



//-------------------------------------------------------------------------------->>
//operator
//-------------------------------------------------------------------------------->>
Client *		 				Channel::searchChannelOperator(std::string nickname) const{
	for (std::map<std::string, Client *>::iterator iter = this->_operator.begin(); \
		iter != this->_operator.end(); iter++){
			if (iter->first == nickname)
				return (iter->second);
		}
	return (NULL);
};
void 						Channel::addChannelOperator(Client new_operator){

};
void  						Channel::deleteChannelOperator(Client operator){

};