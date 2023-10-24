#ifndef SERVER_HPP
# define SERVER_HPP

# include "util.h"
# include "Channel.hpp"
# include "Client.hpp"

class Server {
	public :
		Server();
		~Server();

		// Getter
		const std::string&						getServerName() const;		// 서버 이름 가져오기
		const std::string&						getServerPassword() const;	// 서버 비밀번호 가져오기
		const std::map<int, Client *>&			getUserList() const;		// 유저 목록 가져오기
		const std::map<std::string, Channel *>&	getChannelList() const;		// 채널 목록 가져오기

		// Setter
		void	setServerName(const std::string& server_name);						// 서버 이름 설정
		void	setServerPassword(const std::string& server_password);				// 서버 비밀번호 설정
		void	addUserList(int fd, Client *user);									// 유저목록에 유저 추가
		void	removeUserList(int fd);												// 유저목록에서 유저 삭제
		void	addChannelList(const std::string& channel_name, Channel *channel);	// 채널목록에 채널을 추가
		void	removeChannelList(const std::string& channel_name);					// 채널목록에서 채널 삭제

		// Server
		void	serverInit(int argc, char **argv);
		void	changeEvents(std::vector<struct kevent>& change_list, uintptr_t ident,
				int16_t filter, uint16_t flags, uint32_t fflags, intptr_t data, void *udata);
		void	disconnectClient(int client_fd,	std::map<int, std::string>	clients);
		void	parsingData(int fd);

		// Channel
		Channel	*makeChannel(std::string channel_name, Client *client);	// 채널 객체 생성
		void	deleteChannel(Channel **Channel);						// 채널 객체 삭제
		Channel	*searchChannel(std::string channel_name);				// 채널 검색

		// Message
		void	sendMessage(std::string message, int fd);						// 메세지 보내기
		void	broadcastChannelMessage(std::string message);					// 채널에 메세지 보내기 (본인 포함)
		void	broadcastChannelMessage(std::string message, int socket_fd);	// 채널에 메세지 보내기 (본인 제외)

		// Command
		void	handleCommand(std::string command, int fd);								// 명령어 처리
		void	pong(void);
		void	kickUser(std::string channel_name, std::vector<std::string> param);		// 유저 강퇴
		void	inviteUser(std::string channel_name, std::vector<std::string> param);	// 유저 초대

		Client	*searchClient(std::string nickname);									// 유저 검색
		Client	*searchClient(int fd);													// 유저 검색
		bool	getAuth(Client *user);													// 인증
		bool	getAuth(int fd);

		// Command
		void	commandPass(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		void	commandNick(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		void	commandUser(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		void	commandJoin(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		void	commandPing(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		void	commandPart(std::vector<std::string> token, Client * user, int fd);

		//구현전 - 매개변수 임의로 넣어둠
		// void	commandTopic(std::vector<std::string> token, int paramcnt, Client * user, int fd);
		// void	commandQuit(std::vector<std::string> token, Client * user, int fd);
		void	commandInvite(std::vector<std::string> token, int paramcnt, Client * user, int fd);	// 수도코드
		void	commandKick(std::vector<std::string> token, int paramcnt, Client * user, int fd);	// 수도코드
		void	commandMode(std::vector<std::string> token, Client *user, int fd);
		//prvmsg

	private :
		int							_server_socket;
		struct sockaddr_in			_server_addr;
		std::map<int, std::string>	_clients;
		std::vector<struct kevent>	_change_list;
		struct kevent				_event_list[8];
		int							_new_events;
		struct kevent*				_curr_event;

		//============================서버구동부 건들지 마시오===================================
		std::map<int, std::string>	_send_data; //전송할 데이터
		std::map<int, std::string>	_recv_data; //읽은 데이터

		std::string							_server_name;		//서버이름
		std::string							_server_password;	//서버비밀번호
		std::map<std::string, Channel *>	_channel_list;		//채널목록
		std::map<int, Client *>				_user_list;			//유저목록

		Server(const Server& copy);
		Server& operator=(const Server& obj);
};

#endif
