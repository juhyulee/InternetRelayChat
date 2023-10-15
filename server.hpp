#ifndef SERVER_HPP
# define SERVER_HPP

# include "channel.hpp"

class Server {
	private :
	public :
		std::string servername; //ㅓ버이름
		std::map<std::string, Channel> clist; //채널목록

		std::map<int, Client> usrlist; //유저목록

		std::string serverpassword; //서버비밀번호
		void read_msg(std::string msg, int fd); //메세지 받는 함수
		void send_msg(std::string msg, int fd); //메세지 보내는 함수
		void handle_cmd(std::string cmd, int fd);
		void make_channel(std::string channelname);//채널 만들어주는 함수
		Channel *search_channel(std::string channelname);
		Client *search_user(std::string nickname);
		void adduser(Client user, int fd); //서버에 유저 추가
		void deluser(Client user, int fd); //서버에서 유저 삭제
		void checkuserinformation(Client& user);
		void delchannel(std::string channelname);
};
//메세지 보내는 함수
//메세지 받는 함수

#endif
