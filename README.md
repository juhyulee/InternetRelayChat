

##1장: 소개##
IRC(Internet Relay Chat)는 인터넷에서 사용되는 텍스트 기반의 실시간 커뮤니케이션 프로토콜입니다.
사용자는 공개 또는 비공개 메시지를 주고받을 수 있으며, 직접 메시지를 교환하거나 그룹 채널에 참여할 수 있습니다.
IRC 클라이언트는 IRC 서버에 연결하여 채널에 참여하며, 여러 IRC 서버가 서로 연결되어 네트워크를 형성합니다.

##2장: 일반 규칙##
프로그램은 어떤 경우에도 크래시하거나 예기치 않게 종료되지 않아야 합니다.
만약 그렇다면 프로젝트는 비기능적인 것으로 간주되며 점수는 0점이 됩니다.
Makefile을 작성하여 소스 파일을 컴파일해야 합니다. 또한 불필요한 재링크(relink)가 발생하지 않도록 해야 합니다.
Makefile에는 최소한 다음 규칙이 포함되어야 합니다.
$(NAME), all, clean, fclean, re
프로그램은 C++ 언어로 작성해야 하며, 다음 컴파일 플래그를 적용해야 합니다.
-Wall -Wextra -Werror
C++ 98 표준을 준수해야 하며, -std=c++98 플래그를 추가해도 정상적으로 컴파일되어야 합니다.
가능하면 C++ 기능을 적극적으로 활용해야 하며, 예를 들어 <cstring>을 <string.h> 대신 사용해야 합니다.
외부 라이브러리 및 Boost 라이브러리는 사용할 수 없습니다.

##3장: 필수 구현 사항##
프로그램 이름: ircserv
제출 파일: Makefile, *.h, *.hpp, *.cpp, *.tpp, *.ipp, (선택 사항: 설정 파일)
실행 방법:

./ircserv <port> <password>
port: IRC 서버가 수신할 포트 번호
password: IRC 클라이언트가 연결 시 사용할 인증 비밀번호
사용할 수 있는 외부 함수:

socket, close, setsockopt, getsockname, getprotobyname, gethostbyname, getaddrinfo, freeaddrinfo, bind, connect, listen, accept, htons, htonl, ntohs, ntohl, inet_addr, inet_ntoa, send, recv, signal, sigaction, lseek, fstat, fcntl, poll (또는 유사 기능)
제한 사항:

서버는 여러 클라이언트를 동시에 처리할 수 있어야 하며, 절대로 멈추면 안 됩니다.
포킹(forking)은 금지됩니다.
모든 입출력(I/O) 작업은 논블로킹(non-blocking) 방식으로 처리해야 합니다.
poll() (또는 select(), kqueue(), epoll()과 같은 동등한 함수) 한 개만 사용할 수 있습니다.
IRC 클라이언트는 반드시 서버에 정상적으로 연결될 수 있어야 합니다.
통신은 TCP/IP (IPv4 또는 IPv6) 를 사용해야 합니다.
클라이언트에서 다음과 같은 기능이 정상적으로 동작해야 합니다.
사용자 인증, 닉네임 및 유저네임 설정
채널 참여 및 개인 메시지 송수신
채널 내에서 메시지 송수신 및 브로드캐스트
일반 사용자와 운영자(Operator) 구분
운영자(Operator) 전용 명령어:

KICK: 클라이언트를 채널에서 강제 퇴장
INVITE: 특정 클라이언트를 채널로 초대
TOPIC: 채널 주제 변경 또는 조회
MODE: 채널의 설정 변경
i: 초대 전용 채널 설정/해제
t: 운영자만 TOPIC 변경 가능
k: 채널 비밀번호 설정/해제
o: 채널 운영자 권한 부여/회수
l: 채널 최대 사용자 수 제한 설정/해제


4장: MacOS 전용 설정
MacOS는 write() 함수의 동작 방식이 다르므로 fcntl()을 사용할 수 있습니다.
하지만 다음과 같은 방식으로만 허용됩니다.

fcntl(fd, F_SETFL, O_NONBLOCK);
다른 플래그 사용은 금지됩니다.

5장: 테스트 예제
모든 에러 상황을 철저히 검증해야 합니다.
부분 데이터 수신, 저속 네트워크 환경에서도 정상 작동해야 합니다.
간단한 nc(netcat) 테스트 예제:

$ nc -C 127.0.0.1 6667
com^Dman^Dd
com → man → d\n 처럼 명령을 부분적으로 전송하여 서버가 정상적으로 처리하는지 확인합니다.
