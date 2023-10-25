#include "util.h"
#include "Server.hpp"
#include "Client.hpp"

//int Client::checkLimit == -1  : 유저가 가입할 수 있는 최대 채널 갯수 초과 체크
//int Channel::checkUserLimit == -1  : 채널에 가입할 수 있는 최대 인원 초과 체크

void	Server::commandJoin(std::vector<std::string> token, Client * user, int fd){
	std:: string password = "";
	std:: string & channel_name = token[1];
	const int paramcnt = token.size();

	if (paramcnt >= 2)
		password = token[2];
	//------------------------------------------------------error::no channel name
	if (paramcnt < 1)
	{
		this->sendMessage(ERR_NEEDMOREPARAMS(user->getNickname(), "JOIN"), fd);
		return ;
	}
	Channel *ch = this->searchChannel(channel_name);
	//------------------------------------------------------success::new channel
	if (!ch){
		std::cout << "new channel made : " << token[1] << std::endl;
		ch = makeChannel(token[1], user);
		// 채널에 있는 모든 유저들에게 broadcast RPL_JOIN ok
		this->sendMessage(RPL_JOIN(user->getNickname(), channel_name),fd);
		if (ch->getChannelTopic() != ""){
			this->sendMessage(RPL_TOPIC(user->getNickname(), channel_name, ch->getChannelTopic()), fd);
		}
		this->sendMessage(RPL_ENDOFNAMES(user->getNickname(), channel_name), fd);
		// 채널에 topic이 설정되어 있는 경우
		// RPL_TOPIC
		// RPL_TOPICWHOTIME
		// 구현해야 할 명령어 정리6RPL_NAMREPLY
		// 현재 채널에 있는 user들의 이름 목록, 공백으로 구분하며 operator 앞에는 @ 붙음
		// RPL_ENDOFNAMES
		return ;

	}
	//------------------------------------------------------exist channel
	//------------------------------------------------------error::client exceed max channel cnt
	if (user->checkChannelLimit() == -1)
	{
		this->sendMessage(ERR_TOOMANYCHANNELS(user->getNickname(), "JOIN"), fd);
		return ;
	}
	//------------------------------------------------------error::
	else if (ch->checkUserLimit() == -1){
		this->sendMessage(ERR_CHANNELISFULL(user->getNickname(), channel_name), fd);
	}
	else if (ch->getChannelMode().find('i') != ch->getChannelMode().end()){
		this->sendMessage(ERR_INVITEONLYCHAN(user->getNickname(), channel_name), fd);
	}
	else if (ch->checkInvite(fd) == -1 && ch->checkPassword(password) == -1){
		this->sendMessage(ERR_BADCHANNELKEY(user->getNickname(), channel_name), fd);
	}
	//------------------------------------------------------Success:: exist channel
	else {
		ch->addChannelUser(user);
		std::cout << "exist channel enter : " << token[1] <<"\nthis channel now has " << ch->getUserCount() << std::endl;
		this->sendMessage(RPL_JOIN(user->getNickname(), channel_name),fd);
		this->broadcastChannelMessage(RPL_JOIN(user->getNickname(), channel_name), ch, fd);
		if (ch->getChannelTopic() != ""){
			this->sendMessage(RPL_TOPIC(user->getNickname(), channel_name, ch->getChannelTopic()), fd);
		}
		this->sendMessage(RPL_ENDOFNAMES(user->getNickname(), channel_name), fd);
		this->broadcastChannelMessage(RPL_NAMREPLY(user->getNickname(), "=", channel_name, user->getPrefix()), ch, fd);
	}
	return ;



}


void	Server::commandUser(std::vector<std::string> token, int fd)
{
	//USER "username" "hostname" "servername" :"realname"
	std::string realname = D_REALNAME;
	Client *user = this->searchClient(fd);
	const int paramcnt = token.size();

	if(user != NULL) // already in user_list
	{
		this->sendMessage(ERR_ALREADYREGISTERED(user->getNickname()), fd);
		return ;
	}
	if (paramcnt == 5 && token [4][0] == ':')
		realname = &(token[4][1]);
	else //param err
	{
		this->sendMessage(ERR_NEEDMOREPARAMS((std::string)"root", (std::string)"USER"), fd);
		return ;
	}

	user = this->searchTemp(fd);
	if (!user)
	{
		user = new Client(fd);
		this->_temp_list.insert(std::make_pair(fd, user));
	}
	user->setUsername(token[1]);
	user->setHostname(token[2]);
	user->setUserIp(token[3]);
	user->setRealname(realname);
};

void	Server::commandPass(std::vector<std::string> token, int fd){
	Client * user = NULL;
	user = this->_temp_list.find(fd)->second;
	if(this->searchClient(fd) != NULL){
		this->sendMessage(ERR_ALREADYREGISTERED(user->getNickname()), fd);
	}
	else if (token.size() != 2){
		this->sendMessage(ERR_NEEDMOREPARAMS((std::string)"root", (std::string)"PASS"), fd);
	}
	else if (this->getServerPassword() != token[1]){
		this->sendMessage(ERR_PASSWDMISMATCH((std::string)"root"), fd);
		//disconnect?
	}
	else if (user != this->_temp_list.end()->second) // pass correct && temp user exist
	{
		user->setPass();
	}
	else { // pass correct && temp user none
		user = new Client(fd);
		user->setPass();
		this->_temp_list.insert(std::make_pair(fd, user));
	}
	return ;
};



void	Server::commandNick(std::vector<std::string> token, int fd)
{
	Client *user = NULL;
	if (token.size() != 2){
		this->sendMessage(ERR_NEEDMOREPARAMS(user->getNickname(), (std::string)"NICK"), fd);
		return ;
	}
	if (this->searchClient(token[1])){
		this->sendMessage(ERR_NICKNAMEINUSE(token[1]), fd);
		return;
	};
	user = this->searchClient(fd);
	if (user != NULL)
	{
		this->sendMessage(RPL_NICK(user->getPrefix(), token[1]), fd);
		user->setNickname(token[1]);
		return;
	}
	user = this->searchTemp(fd);
	if (user != NULL)
	{
		user->setNickname(token[1]);
	}
	else
	{
		user = new Client(fd);
		user->setNickname(token[1]);
		this->_temp_list.insert(std::make_pair(fd, user));
	}
};


void	Server::commandPing(std::vector<std::string> token, Client * user,  int fd)
{
	std::cout << "check pong" << RPL_PONG(user->getPrefix(), token[1]) << std::endl;
	if (token.size() != 2){
		this->sendMessage(ERR_NOORIGIN(user->getNickname()), fd);
	}
	else{
		this->sendMessage(RPL_PONG(user->getPrefix(), token[1]), fd);
	}
}

void Server::commandMode(std::vector<std::string> token, Client *user, int fd) {
	if (token.size() == 1) {
		this->sendMessage(ERR_NEEDMOREPARAMS(user->getPrefix(), token[0]), fd);
		return ;
	}
	Channel *channel = searchChannel(token[1]);
	if (channel == NULL) {
		this->sendMessage(ERR_NOSUCHCHANNEL(user->getPrefix(), token[1]), fd);
	}
	// 요청한 클라이언트가 해당 채널에 존재하는지 확인
	// 클라이언트에 메소드 추가
	// else if () {
	// }
	else if (token.size() == 2) {
		std::vector<std::string> *mode_params = channel->getChannelModeParams();
		this->sendMessage(RPL_CHANNELMODEIS(user->getPrefix(), channel->getChannelName(), (*mode_params)[0], (*mode_params)[1]), fd);
	}
	else {
		try {
			std::vector<std::string> *mode_params = channel->setChannelMode(token, user);
			if (mode_params) {
				this->broadcastChannelMessage(RPL_MODE(user->getPrefix(), channel->getChannelName(), \
				(*mode_params)[0], (*mode_params)[1]), channel);
			}
		} catch (std::exception &e) {
			this->sendMessage(e.what(), fd);
		}
	}
}


void	Server::commandPart(std::vector<std::string> token, Client * user, int fd) {
	// std::string b;
	// std::cin >> b;
	if (token.size() != 2)
		return ;
	Channel * ch = searchChannel(token[1]);
	if (!ch){
		sendMessage(ERR_NOSUCHCHANNEL(user->getNickname(), token[1]), fd);
		return ;
	}
	if (ch->isChannelUser(user) == false){
		sendMessage(ERR_NOTONCHANNEL(user->getNickname(), token[1]), fd);
		return ;
	}
	ch->removeChannelUser(user);
	broadcastChannelMessage(RPL_PART(user->getPrefix(), token[1]), ch);
}


void	Server::commandPrivmsg(std::vector<std::string> token, Client * user, int fd){
	if (token.size() != 3 ){
		sendMessage(ERR_NEEDMOREPARAMS(user->getNickname(), token[0]), fd);
		std::cout << "privmsg error from " << user->getNickname() << std::endl;
		return;
	}
	Channel *ch = searchChannel(token[1]);
	if (!ch)
		sendMessage(ERR_NOSUCHCHANNEL(user->getNickname(), token[1]), fd);
	if (ch->isChannelUser(user) == false){
		sendMessage(ERR_USERNOTINCHANNEL(user->getPrefix(), user->getNickname(), token[1]), fd);
	}
	broadcastChannelMessage(RPL_PRIVMSG(user->getNickname(), token[1], token[2]), ch, fd);
}

// void	Server::commandInvite(std::vector<std::string> token, Client * user, int fd){
	//invite nickname #channel - 파라미터
	//정상 실행 시
		//해당 유저 인바이트 리스트에 추가
		//RPL_INVITE(user, nick, channel)
	//닉네임이 없는 경우
		//ERR_NOSUCHNICK(user, nick)
	//없는 채널일 경우
		//ERR_NOSUCHCHANNEL(user, channel)
	//오퍼레이터가 아닌 경우
		//ERR_CHANOPRIVSNEEDED(user, channel)
	//채널에 없는 유저가 보낸 경우
		//ERR_NOTONCHANNEL(user, channel)
	//이미 채널에 있는 유저일경우
		//ERR_USERONCHANNEL(user, nick, channel)
// }
// void	Server::commandKick(std::vector<std::string> token, Client * user, int fd){
	//kick #channel nickname - 파라미터
	//정살 실행시 해당 채널에 broadcast
	//닉네임이 없는 경우
		//ERR_NOSUCHNICK(user, nick)
	//없는 채널일 경우
		//ERR_NOSUCHCHANNEL(user, channel)
	//채널에 없는 유저가 실행했을 경우
		//ERR_NOTONCHANNEL(user, channel)
	//채널에 없는 유저를 kick한 경우
		//ERR_USERNOTINCHANNEL(user, nick, channel)
	//오퍼레이터가 아닌데 오퍼레이터를 강퇴시킨경우
		//ERR_CHANOPRIVSNEEDED(user, channel)
	//오퍼레이터가 방에 없는 경우 -? 이건 뭔소리고
		//ERR_CHANOPRIVSNEEDED2(user, channel)\

// }
