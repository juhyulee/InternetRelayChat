#include "util.h"
#include "server.hpp"

//int Client::checkLimit == -1  : 유저가 가입할 수 있는 최대 채널 갯수 초과 체크
//int Channel::checkUserLimit == -1  : 채널에 가입할 수 있는 최대 인원 초과 체크

void	Server::commandJoin(std::vector<std::string> token, int paramcnt, int fd){
	std::map<int, Client> userlist = this->getUserList();
	Client user = userlist[fd];
	std:: string password = NULL;
	if (paramcnt >= 2)
		password = token[2];

	//------------------------------------------------------error::no channel name
	if (paramcnt < 1)
	{
		this->sendMessage(ERR_NEEDMOREPARAMS(user.getNickname(), "JOIN"), fd);
		return ;
	}
	//------------------------------------------------------error::client exceed max channel cnt
	if (user.checkChannelLimit() == -1)
	{
		this->sendMessage(ERR_TOOMANYCHANNELS(user.getNickname(), "JOIN"), fd);
		return ;
	}
	//------------------------------------------------------success::new channel
	Channel *ch = this->searchChannel(token[0]);
	if (!ch){
		ch = new Channel(token[0], user);
		this->addChannelList(token[0], *ch);
		// 채널에 있는 모든 유저들에게 broadcast RPL_JOIN
		// 채널에 topic이 설정되어 있는 경우
		// RPL_TOPIC
		// RPL_TOPICWHOTIME
		// 구현해야 할 명령어 정리6RPL_NAMREPLY
		// 현재 채널에 있는 user들의 이름 목록, 공백으로 구분하며 operator 앞에는 @ 붙음
		// RPL_ENDOFNAMES
		return ;
	}
	//------------------------------------------------------exist channel
	//------------------------------------------------------error::
	if (ch->checkUserLimit() == -1){
		this->sendMessage(ERR_CHANNELISFULL(user.getNickname(), token[1]), fd);
	}
	if (ch->getChannelMode().find('i') != ch->getChannelMode().end()){
		this->sendMessage(ERR_INVITEONLYCHAN(user.getNickname(), token[1]), fd);
	}
	if (ch->checkPassword(password) == -1){
		this->sendMessage(ERR_BADCHANNELKEY(user.getNickname(), token[1]), fd);
	}
	if (ch->checkBanned(user) == -1){
		this->sendMessage(ERR_BANNEDFROMCHAN(user.getNickname(), token[1]), fd);
	}
	//------------------------------------------------------Success::
	else {
		ch->addChannelUser(fd, user);
		this->broadcastChannelMessage(RPL_JOIN(user.getNickname(), token[1]), fd);
		//위 fd map으로 바꿔야 함...
		if (ch->getChannelTopic() != NULL){
			//RPL_TOPIC
			this->sendMessage(RPL_TOPIC(user.getNickname(), token[1], ch->getChannelTopic()), fd);
			// this->sendMessage(RPL_TOPICWHOTIME(user.getNickname(), token[1], ch->getChannelTopic()), fd);
			//RPL_TOPICWHOTIME 마지막 값이 타임스탬프... https://modern.ircdocs.horse/#rpltopic-333
		}
		this->sendMessage(RPL_NAMREPLY(user.getNickname(), ch->getSymbol(), token[1], user.getPrefix()), fd);
		//RPL_NAMREPLY :: 어케하누
		this->broadcastChannelMessage(RPL_ENDOFNAMES(user.getNickname(), token[1]), fd);
		//RPL_ENDOFNAMES
	}

}