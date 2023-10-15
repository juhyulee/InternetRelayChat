#include "server.hpp"
#include <iterator>
#include <ostream>
#include <set>
#include <sstream>
#include <utility>
//10.11.3.2
//irssi -c 10.12.7.6 -p 6770 -n juhyulee -w 1234
//서버네임 숫자 닉네임 메세지

void Server::send_msg(std::string msg, int fd) { //메세지 전송하는 함수
	send(fd, msg.c_str(), msg.size(), 0);
}

void Server::read_msg(std::string msg, int fd) { // 메세지 읽는 함수
	std::vector<std::string> token;
	std::istringstream iss(msg);
	std::string line;

	while (std::getline(iss, line))
		token.push_back(line);
	for (size_t i = 0; i < token.size(); i++) {
		std::cout << token[i] << std::endl;
		handle_cmd(token[i], fd); //개행 기준으로 tokenizer해서 들어감
	}
}

void Server::handle_cmd(std::string cmd, int fd) { // 메세지 파싱하는 함수
	std::vector<std::string> token;
	std::istringstream iss(cmd);
	std::string word;

	while (iss >> word) {
		token.push_back(word);
	}
	std::cout << "token: " << token[0] << std::endl;
	if (token[0] == "PASS") {
		if (token[1] == serverpassword) {
			usrlist[fd].pass += 1;
			usrlist[fd].fd = fd;
		}
	}
	else if (token[0] == "NICK") {
		std::map<int, Client>::iterator it;
		for (it = usrlist.begin(); it != usrlist.end(); ++it) {
			if (it->second.nickname == token[1]) {
				std::string msg = token[1] + " :Nickname is already in use\r\n";
				send_msg(msg, fd);
			}
		}
		usrlist[fd].nickname = token[1];
		usrlist[fd].pass += 1;
	}
	else if (token[0] == "USER") { //생성자로 대체
		usrlist[fd].username = token[1];
		usrlist[fd].hostname = token[2];
		usrlist[fd].servername = token[3];
		usrlist[fd].realname = token[4];
		usrlist[fd].pass += 1;
		checkuserinformation(usrlist[fd]);
	}

	else if (token[0] == "PING") {
		std::string pong = "PONG irc_test\r\n";
		send_msg(pong, fd);
	}
	else if (token[0] == "JOIN") {
		if (this->search_channel(token[1]) == NULL) {
			make_channel(token[1]);
		}
		clist[token[1]].adduser(fd, usrlist[fd]);
		if (clist[token[1]].usrlist.size() == 1) {
			clist[token[1]].setchanneloperator(usrlist[fd].nickname);
		}
		std::string join_message = usrlist[fd].nickname + " JOIN " + token[1] + "\r\n";
		this->send_msg(join_message, fd);
	}
	else if (token[0] == "PART") { //채널나가는명령어
		for (int i = 1; i <= token.size(); i++) {
			Channel	targetChannel = this->clist[token[i]];
			targetChannel.deleteuser(fd);
			if (targetChannel.usrlist.size() == 0) {
				this->clist.erase(clist.find(token[i]));
			}
		}
	}
	else if (token[0] == "NOTICE") {//메세지전송
		Channel *target_channel = this->search_channel(token[1]);
		for (std::map<int, Client>::iterator iter = target_channel->usrlist.begin(); \
			iter != target_channel->usrlist.end(); ++iter) {
			this->send_msg(token[token.size()], iter->second.fd);
		}
	}
	else if (token[0] == "PRIVMSG") { //메세지 전송
		for (int i = 1; i < token.size() - 1; i++) {
			if (token[i][0] == '#') {
				Channel *target_channel = this->search_channel(token[i]);
				for (std::map<int, Client>::iterator iter = target_channel->usrlist.begin();
					iter != target_channel->usrlist.end(); ++iter) {
					this->send_msg(token[token.size()], iter->second.fd);
				}
			}
			else if (token[i][0] == '$') {
				int	send_fd = this->search_user(token[i])->fd;
				this->send_msg(token[token.size()], send_fd);
			}
			else {
			// Error: 올바르지 않은 형식의 command
			}
		}
	}
	else if (token[0] == "KICK") { //채널 방출
		Channel *target_channel = this->search_channel(token[1]);
		Client *target_user = target_channel->search_user(token[2]);
		target_channel->deleteuser(target_user->fd);
		std::stringstream kick_message;
		kick_message << target_channel->getchannelname() << ". kicked from the channel.";
		if (token.size() < 3) {
			kick_message << " : " << token[3];
		}
		kick_message << std::endl;
		this->send_msg(kick_message.str(), target_user->fd);
	}
	else if (token[0] == "INVITE") { //채널에 유저 초대
		if (token.size() != 3) {
			// 유효하지 않은 명령어
		}
		Client *invite_client = this->search_user(token[1]);
		if (invite_client == NULL) {
			// 존재하지 않는 유저
		}
		Channel *invite_channel = this->search_channel(token[2]);
		if (invite_channel == NULL) {
			// 존재하지 않는 채널
		}
		invite_channel->adduser(invite_client->fd, *invite_client);
	}
	else if (token[0] == "TOPIC") { //채널 토픽 설정
		if (token.size() == 1) {
			std::string view_topic = this->search_channel(token[1])->getchanneltopic();
			send_msg(view_topic, fd);
		}
		else {
			Channel *target_channel = this->search_channel(token[1]);
			target_channel->setchanneltopic(token[2]);
		}
	}
	else if (token[0] == "MODE") { //채널모드설정

	}
	else if (token[0] == "QUIT") { //다른 유저들한테 나갔다고 보냄
		std::string quit_message = usrlist[fd].nickname + " :Quit\r\n";
		std::set<int> receive_user;
		for (std::map<int, Client>::iterator iter = usrlist.begin(); iter != usrlist.end(); iter++) {
			if (iter->first != fd) {
				receive_user.insert(iter->first);
			}
		}
		for (std::set<int>::iterator iter = receive_user.begin(); iter != receive_user.end(); iter++) {
			send_msg(quit_message, *iter);
			std::cout << "iter test: " << *iter << std::endl;
		}
		close(fd);
	}
}

void Server::checkuserinformation(Client& user) {
	std::cout << "pass " << user.nickname << std::endl;
	if (user.pass == 3) {
		std::string welcome = ":irc_test 001 " + user.nickname + " :Welcome to the irc Network, " + user.nickname + "!root@" + user.servername + "\r\n";
		std::cout << welcome << std::endl;
		send_msg(welcome, user.fd);
	}
}

void Server::make_channel(std::string channelname) {
	Channel newchannel;

	newchannel.setchannelname(channelname);

	clist[channelname] = newchannel;
}

Channel *Server::search_channel(std::string channelname) {
	for (std::map<std::string, Channel>::iterator iter = this->clist.begin(); iter != this->clist.end(); ++iter) {
		if (iter->second.getchannelname() == channelname)
		return &iter->second;
	}
	return NULL;
}

Client *Server::search_user(std::string nickname) {
	for (std::map<int, Client>::iterator iter = this->usrlist.begin(); iter != this->usrlist.end(); ++iter) {
		if (iter->second.nickname == nickname)
		return &iter->second;
	}
	return NULL;
}

void Server::adduser(Client user, int fd) {
	usrlist[fd] = user;
}

void Server::deluser(Client user, int fd) {
	usrlist.erase(usrlist.find(fd));
}

// std::map<std::string, ICommand*> command;

// command[token[0]]->run(token);

// class ICommand
// {

// };
