#include "login.h"

int con = 0;

int recvn(SOCKET s, char *buf, int len, int flags)
{
	int received;
	char *ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
	}

	return (len - left);
}

int TCP_Login()
{
	int retval;

	printf("오목 게임\n\n");
	printf("가로, 세로, 혹은 대각선으로 먼저 5개의 돌을 연달아 배치시키면 이기는 게임\n");
	printf("사용자 2명이 번갈아가면서 돌을 배치시킵니다\n");
	printf("백돌이 먼저 시작합니다\n");

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 서버와 데이터 통신
	while (1) {
		// 데이터 입력
		printf("\n비밀번호를 입력하세요 : ");
		if (fgets(buf, BUFSIZE + 1, stdin) == NULL)
			break;

		// '\n' 문자 제거
		len = strlen(buf);
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';
		if (strlen(buf) == 0)
			break;

		// 데이터 보내기
		retval = send(sock, buf, strlen(buf), 0);
		if (retval == SOCKET_ERROR) {
			err_display("send()");
			break;
		}

		int rBuf;

		// 데이터 받기
		retval = recvn(sock, (char*)&rBuf, sizeof(int), 0);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;

		if (rBuf == 0)
		{
			printf("일치합니다. 게임을 시작합니다.\n");
			break;
		}
		else if (rBuf == 1)
		{
			printf("불일치\n비밀번호를 다시 입력해주세요\n");
		}
		else
		{
			printf("방 참가를 거부방했습니다.\n");
			con = 1;
			break;
		}
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return con;
}