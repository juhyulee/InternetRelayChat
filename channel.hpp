#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "client.hpp"
#include <set>

class Server;

class Channel {
	private :
		std::string channelname; // 채널이름
		std::string channelpassword; //채널비밀번호
		std::map<std::string, Client> channeloperator; //채널오퍼레이터
		std::string channeltopic; //채널토픽
		std::set<char> channelmode; //채널모드
		int usrcnt; //유저 인원수
		int usrlimits; //최대 유저 인원수
	public :
		Channel();

		std::map<int, Client> usrlist; //유저목록

		const std::string& getchannelname() const;
		const std::string& getchannelpassword() const;
		const std::map<std::string, Client>& getchanneloperator() const;
		const std::string& getchanneltopic() const;
		const std::set<char>& getchannelmode() const;
		int	getusrcnt() const;
		int	getusrlimits() const;

		void setchannelname(std::string chanellname);
		void setchannelpassword(std::string password);
		void deletechannelpassword();
		void addchanneloperator(Client newoperator);
		void removechanneloperator(Client newoperator);
		void setchanneltopic(std::string newtopic);
		std::vector<std::string> *setchannelmode(Server &server, std::vector<std::string> token);
		void setusrlimits(int new_limits);
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄

		void adduser(int fd, Client client); //유저 채널에 추가하는 함수
		Client *search_user(std::string nickname);
		void deleteuser(int fd);//유저 usrlist에서 지우는 함수
		void inviteuser(int fd, Client client);//채널에 유저 초대하는 함수
		void broadcastChannelMessage(int send_fd, std::string message);// 채널에 메세지 보내는 함수
};

#endif
