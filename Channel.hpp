#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "util.h"

class Server;

class Client;

class Channel {
	public :
		class ChannelModeException : public std::exception {
			public :
				ChannelModeException();
				ChannelModeException(const std::string& message);
				~ChannelModeException() throw();
				virtual const char	*what() const throw();
			private:
				std::string _message;
		};

		Channel(const std::string& name, Client *client);
		~Channel();

		// Getter
		const std::string&					getChannelName() const;
		const std::string&					getChannelPassword() const;
		const std::string&					getChannelTopic() const;
		const std::set<char>&				getChannelMode() const;
		size_t								getUserLimit() const;
		std::map<int, Client *>::size_type	getUserCount() const;
		const std::map<int, Client *>&		getUserList() const;
		const std::map<int, Client *>&		getInviteList() const;
		const std::map<int, Client *>&		getChannelOperator() const;

		// Setter
		void 						setChannelName(std::string new_name);
		void 						setChannelPassword(std::string new_password);
		void 						removeChannelPassword();
		void  						setChannelTopic(std::string new_topic);
		std::vector<std::string>	*setChannelMode(std::vector<std::string> token, Client *client);
		void 						setUserLimit(int new_limits);


		//msg		보류: Server에서 처리하게 될 것 같음
		// void	broadcastChannelMessage(int send_fd, std::string message);// 채널에 메세지 보내는 함수
		// ^^^ send_fd == -1 자기자신 포함, 유저일 때는 비포함
		//특정 유저한테 보낼 메세지 서버로 보냄
		//전체 유저한테 보낼 메세지 서버로 보냄

		// Mode
		void						clearChannelMode();
		std::vector<std::string>	*getChannelModeParams() const;

		// Operator
		bool	isChannelOperator(Client *client);
		Client	*findChannelOperator(std::string nickname);
		bool	addChannelOperator(Client *new_operator);
		bool	removeChannelOperator(Client *old_operator);
		void	clearChannelOperator();

		// User
		bool	isChannelUser(Client *client);				// 유저 목록에 있는지 확인
		Client	*findChannelUser(std::string nickname);
		Client	*findChannelUser(Client *client);			// 유저 목록에서 검색
		bool	addChannelUser(Client *client);				// 유저 목록에 추가
		bool	removeChannelUser(Client *client);			// 유저 목록에서 지우기
		void	clearUserList();

		// Invite
		bool	isInvitedUser(Client *client);				// 초대 목록에 있는지 확인
		Client	*findInvitedUser(std::string nickname);	// 초대 목록에서 검색
		bool	addInvitedUser(Client *client);				// 초대 목록에 추가
		bool	removeInvitedUser(Client *client);			// 채널 목록에서 지우기
		void	clearInviteList();

		// Check
		int	checkUserLimit() ;
		int	checkInvite(Client *client);
		int	checkInvite(int fd);
		int	checkPassword(std::string password);

		private :
			std::string				_name;			// 채널 이름
			std::string				_password;		// 채널 비밀번호
			std::string				_topic;			// 채널 토픽
			std::set<char>			_mode;			// 채널 모드
			size_t					_user_limit;	// 최대 유저 인원수
			std::map<int, Client *>	_user_list;		// 유저 목록
			std::map<int, Client *>	_invite_list;	// 유저 초대 목록
			std::map<int, Client *>	_operator;		// 채널 오퍼레이터
			// int						_user_count;	// get으로 대체(_user_list.size())

			Channel();
			Channel(const Channel& copy);
			Channel& operator=(const Channel& assign);

			void	initialize();
};

#endif
