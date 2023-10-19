#ifndef SERVER_HPP
# define SERVER_HPP

# include "util.h"
//# include "Channel.hpp"

class Server {
	private :
		int _server_socket;
		struct sockaddr_in _server_addr;
		std::map<int, std::string> _clients;
		std::vector<struct kevent> _change_list;
		struct kevent _event_list[8];
		int _new_events;
		struct kevent* _curr_event;
		//============================서버구동부 건들지 마시오===================================
		std::map<int, std::string> _send_data; //전송할 데이터
		std::map<int, std::string> _recv_data; //읽은 데이터

		std::string _server_name;														//서버이름
		std::map<std::string, Channel> _channel_list;									//채널목록
		std::map<int, Client> _user_list;												//유저목록
		std::string _server_password;													//서버비밀번호

		Server(const Server& copy);														//복사생성자
		Server& operator=(const Server& obj);											//할당연산자

	public :
		Server();																		//기본생성자
		~Server();																		//소멸자

		const std::string& getServerName() const;										//서버이름 가져오는 함수
		const std::map<std::string, Channel>& getChannelList() const;					//채널목록 가져오는 함수
		const std::map<int, Client>& getUserList() const;								//유저목록 가져오는 함수
		const std::string& getServerPassword() const;									//서버비밀번호 가져오는 함수

		void setServerName(const std::string& server_name);								//서버이름 설정하는 함수
		void addChanelList(const std::string& channel_name, const Channel& channel);		//채널목록에 채널 추가하는 함수
		void deleteChannelList(const std::string& channel_name);							//채널목록에서 채널 삭제하는 함수
		void addUserList(int fd, const Client& user);									//유저목록에 유저 추가하는 함수
		void deleteUserList(int fd);													//유저목록에서 유저 삭제하는 함수
		void setServerPassword(const std::string& server_password);						//서버비밀번호 설정하는 함수

		void readMessage(std::string message, int fd);									//메세지 받는 함수
		void sendMessage(std::string message, int fd);									//메세지 보내는 함수
		void broadcastChannelMessage(std::string message, int send_fd);					//채널에 메세지 보내는 함수
		void handleCommand(std::string command, int fd);								//명령어 처리하는 함수
		void makeChannel(std::string channel_name);										//채널 만들어주는 함수
		Channel *searchChannel(std::string channel_name);								//채널 찾아주는 함수
		Client *searchUser(std::string nickname);										//유저 찾아주는 함수
		void checkUserInformation(Client& user);										//유저 정보 확인하는 함수

		void setChannelMode(std::string channel_name, std::vector<std::string> param);	//채널 모드 설정하는 함수
		void kickUser(std::string channel_name, std::vector<std::string> param);		//유저 강퇴하는 함수
		void inviteUser(std::string channel_name, std::vector<std::string> param);		//유저 초대하는 함수

		void serverInit(int argc, char **argv);
		void changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident, \
		int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void disconnectClient(int client_fd, std::map<int, std::string>& clients);
		void parsingData(std::string message, int fd);

		void pong(void);
};
//메세지 보내는 함수
//메세지 받는 함수

#endif
