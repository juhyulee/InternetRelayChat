#include "util.h"
#include "Server.hpp"
#include "Client.hpp"

//int Client::checkLimit == -1  : 유저가 가입할 수 있는 최대 채널 갯수 초과 체크
//int Channel::checkUserLimit == -1  : 채널에 가입할 수 있는 최대 인원 초과 체크

void	Server::commandJoin(std::vector<std::string> token, int paramcnt, Client * user, int fd){
	std:: string password = NULL;
	std:: string & channel_name = token[1];

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
		ch = new Channel(token[0], user);
		this->addChannelList(token[0], ch);
		// 채널에 있는 모든 유저들에게 broadcast RPL_JOIN ok
		this->broadcastChannelMessage(RPL_JOIN(user->getPrefix(), channel_name));
		// 채널에 topic이 설정되어 있는 경우
		// RPL_TOPIC
		// RPL_TOPICWHOTIME
		// 구현해야 할 명령어 정리6RPL_NAMREPLY
		// 현재 채널에 있는 user들의 이름 목록, 공백으로 구분하며 operator 앞에는 @ 붙음
		// RPL_ENDOFNAMES
		
	}
	//------------------------------------------------------exist channel
	//------------------------------------------------------error::client exceed max channel cnt
	else if (user->checkChannelLimit() == -1)
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
		this->broadcastChannelMessage(RPL_JOIN(user->getNickname(), channel_name), fd);
		//위 fd map으로 바꿔야 함...
		if (ch->getChannelTopic() != ""){
			//RPL_TOPIC
			this->broadcastChannelMessage(RPL_TOPIC(user->getNickname(), channel_name, ch->getChannelTopic()), fd);
			// this->sendMessage(RPL_TOPICWHOTIME(user->getNickname(), channel_name, ch->getChannelTopic()), fd);
			//RPL_TOPICWHOTIME 마지막 값이 타임스탬프... https://modern.ircdocs.horse/#rpltopic-333
		}
		this->broadcastChannelMessage(RPL_NAMREPLY(user->getNickname(), "=", channel_name, user->getPrefix()), fd);
		this->broadcastChannelMessage(RPL_ENDOFNAMES(user->getNickname(), channel_name), fd);
	}
	return ;



}


void	Server::commandUser(std::vector<std::string> token, int paramcnt, Client * user, int fd)
{
	//USER "username" "hostname" "servername" :"realname"
	(void)fd;
	std::cout << "commandUser\n";
	std::string realname = D_REALNAME;
	if (paramcnt == 4 && token [4][0] == ':')
		realname = &(token[4][1]);
	if (paramcnt == 4)
	{
		user->setUsername(token[1]);
		user->setHostname(token[2]);
		user->setUserIp(token[3]);
		user->setRealname(realname);
	}
	else
	{
		//error ??
		std::cout << "USERERR:: "<<token[0] <<token[1]<<token[2]<<token[3]<<std::endl;
	}

};

void	Server::commandPass(std::vector<std::string> token, int paramcnt, Client * user, int fd){
	if(this->searchClient(fd) != NULL){
		this->sendMessage(ERR_ALREADYREGISTERED(user->getNickname()), fd);
	}
	else if (paramcnt != 1){
		this->sendMessage(ERR_NEEDMOREPARAMS((std::string)"root", (std::string)"PASS"), fd);
	}
	else if (this->getServerPassword() != token[1]){
		this->sendMessage(ERR_PASSWDMISMATCH((std::string)"root"), fd);
		//disconnect?
	}
	else {
		this->addUserList(fd, user);
	}
	return ;
};



void	Server::commandNick(std::vector<std::string> token, int paramcnt, Client * user, int fd)
{
	if (paramcnt != 1){
		this->sendMessage(ERR_NEEDMOREPARAMS(user->getNickname(), (std::string)"NICK"), fd);
		return ;
	}
	user->setNickname(token[1]);

};


void	Server::commandPing(std::vector<std::string> token, int paramcnt, Client * user, int fd)
{
	std::cout << "check pong" << RPL_PONG(user->getPrefix(), token[1]) << std::endl;
	if (paramcnt != 1){
		this->sendMessage(ERR_NOORIGIN(user->getNickname()), fd);
	}
	else{
		this->sendMessage(RPL_PONG(user->getPrefix(), token[1]), fd);
	}
}