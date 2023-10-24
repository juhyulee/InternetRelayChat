#include "util.h"
#include "Server.hpp"

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

void	Server::commandPart(std::vector<std::string> token, Client * user, int fd) {
	if (token.size() != 2)
		return ;
	broadcastChannelMessage(RPL_QUIT(user->getNickname(), ": from this channel"), fd);
	Channel * temp = searchChannel(token[1]);
	temp->removeChannelUser(user);
}

void	Server::commandInvite(std::vector<std::string> token, int paramcnt, Client * user, int fd) {
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
}

void	Server::commandKick(std::vector<std::string> token, int paramcnt, Client * user, int fd) {
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
		//ERR_CHANOPRIVSNEEDED2(user, channel)
}

void Server::commandMode(std::vector<std::string> token, Client *user, int fd) {
	if (token.size() == 1) {
		sendMessage(ERR_NEEDMOREPARAMS(user->getPrefix(), token[0]), fd);
		return ;
	}
	Channel *channel = searchChannel(token[1]);
	if (channel == NULL) {
		sendMessage(ERR_NOSUCHCHANNEL(user->getPrefix(), token[1]), fd);
	}
	// 요청한 클라이언트가 해당 채널에 존재하는지 확인
	// 클라이언트에 메소드 추가
	// else if () {
	// }
	else if (token.size() == 2) {
		std::vector<std::string> *mode_params = channel->getChannelModeParams();
		sendMessage(RPL_CHANNELMODEIS(user->getPrefix(), channel->getChannelName(), (*mode_params)[0], (*mode_params)[1]), fd);
	}
	else {
		try {
			std::vector<std::string> *mode_params = channel->setChannelMode(token, user);
			if (mode_params) {
				broadcastChannelMessage(RPL_MODE(user->getPrefix(), channel->getChannelName(), (*mode_params)[0], (*mode_params)[1]));
			}
		} catch (std::exception &e) {
			sendMessage(e.what(), fd);
		}
	}
}
