#include "server.hpp"
#include <sstream>
//10.11.3.2
//irssi -c 10.12.2.6 -p 6770 -n juhyulee -w 1234
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
		std::string pong = "PONG irc_test \r\n";
		send_msg(pong, fd);
	}
	else if (token[0] == "JOIN") {
		make_channel(token[1]);
		clist[token[1]].adduser(usrlist[fd].nickname, usrlist[fd]);
		if (clist[token[1]].usrlist.size() == 1) {
			clist[token[1]].setchanneloperator(usrlist[fd].nickname);
		}
	}
	else if (token[0] == "PART") { //채널나가는명령어

	}
	else if (token[0] == "NOTICE") {//메세지전송

	}
	else if (token[0] == "PRIVMSG") {//메세지 전송

	}
	else if (token[0] == "KICK") { //채널 방출

	}
	else if (token[0] == "INVITE") { //채널에 유저 초대

	}
	else if (token[0] == "TOPIC") { //채널 토픽 설정

	}
	else if (token[0] == "MODE") { //채널모드설정

	}
	else if (token[0] == "QUIT") { //다른 유저들한테 나갔다고 보냄

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

void Server::adduser(Client user, int fd) {
	usrlist[fd] = user;
}

void Server::deluser(Client user, int fd) {
	usrlist.erase(usrlist.find(fd));
}
