#include "UDP_Server.h"

SOCKET sock;
SOCKADDR_IN serveraddr;
int matrix[2][20][20];
int other, x, y;
int end = -1;

SOCKET m_sock; // multicast용 sock
SOCKADDR_IN remoteaddr;

// 소켓 함수 오류 출력 후 종료
void err_quit(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int Game_End(int other, int x, int y)
{
	int count = 0;
	//가로 5개
	if (matrix[other][(x + 1) / 2][y])
		count++;

	while (count) //←
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2) - j][y] && (((x + 1) / 2) - j) >= 1) //범위내에 있어야 합니다
				count++;
			else
				count = 0; //초기화
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) //→
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2) + j][y] && (((x + 1) / 2) + j) <= 20) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) //↑
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2)][y - j] && (y - j) >= 1) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) // ↓
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2)][y + j] && (y + j) <= 20) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) // ↗
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 + j)][y - j] && (y - j) >= 1 && (((x + 1) / 2) + j) <= 20) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) // ↙
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 - j)][y + j] && (y + j) <= 20 && (((x + 1) / 2) - j) >= 1) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) // ↖
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 - j)][y - j] && (y - j) >= 1 && (((x + 1) / 2) - j) >= 1) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}

	count++;

	while (count) // ↘
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 + j)][y + j] && (y + j) <= 20 && (((x + 1) / 2) + j) <= 20) //범위내에 있어야 합니다
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 2);
	}
	return 0; //게임이 안 끝났으면 0 반환
}

int Set_UDP()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);		// udp 소켓 생성 
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);		// 모든 ip로부터 수신
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));	// udp 서비스 완료
	if (retval == SOCKET_ERROR) err_quit("bind()");
}

int Multi_Set()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	m_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 멀티캐스트 TTL 설정
	int ttl = 2;
	retval = setsockopt(m_sock, IPPROTO_IP, IP_MULTICAST_TTL,
		(char *)&ttl, sizeof(ttl));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// 소켓 주소 구조체 초기화
	remoteaddr;
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(MULTICASTIP);
	remoteaddr.sin_port = htons(REMOTEPORT);

	return 0;
}

int main()
{
	Set_UDP();
	Multi_Set();

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[100];
	int turn = 0;
	DataPack* data;
	DataPack recvBuf;
	DataPack sendBuf;
	bool a1 = true;
	bool a2 = false;
	other = -1;
	x = -1;
	y = -1;
	end = -1;

	while (1)
	{
		int addrlen = sizeof(clientaddr);
		int recvlen;
		int recvRetval = recvfrom(sock, (char *)&recvlen, sizeof(int), 0,
			(SOCKADDR *)&clientaddr, &addrlen);
		if (recvRetval == SOCKET_ERROR) {
			err_display("recvfrom()");
			return 2;
		}

		int getSize = recvfrom(sock, buf, recvlen, 0, (SOCKADDR *)&clientaddr, &addrlen);
		if (getSize == SOCKET_ERROR) {
			err_display("recvfrom()");
			return 2;
		}

		buf[getSize] = '\0';
		data = (DataPack*)buf;

		recvBuf = *data;

		sendBuf.game_end = end;
		sendBuf.m_other = other;
		sendBuf.m_x = x;
		sendBuf.m_y = y;

		if (recvBuf.m_other == 5)
		{
			if (a1)
			{
				sendBuf.m_other = 0;
				a1 = false;
				a2 = true;
			}
			else if (a2)
			{
				sendBuf.m_other = 1;
				a2 = false;
			}
			sendBuf.game_end = 5;
		}

		if (recvBuf.m_other != 6 )
		{
			end = recvBuf.game_end;
			other = recvBuf.m_other;
			x = recvBuf.m_x;
			y = recvBuf.m_y;

			if (recvBuf.m_other != 5)
			{
				int judgment = Game_End(other, x, y);
				judgment += recvBuf.m_other;

				char b[513];
				sprintf_s(b, "%d", judgment);

				int l = strlen(b);
				b[l] = '\0';

				int m_retval = sendto(m_sock, b, 512, 0,
					(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
				if (m_retval == SOCKET_ERROR) {
					err_display("sendto()");
					return 2;
				}

				if (recvBuf.m_x > 0 && recvBuf.m_y > 0 && recvBuf.m_other < 2)
					matrix[recvBuf.m_other][(recvBuf.m_x + 1) / 2][recvBuf.m_y] = 1;
			}
		}

		// 클라이언트 데이터 보내기
		int len = sizeof(sendBuf);
		int retval = sendto(sock, (char*)&len, sizeof(int), 0,
			(SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			return 2;
		}

		retval = sendto(sock, (char*)&sendBuf, sizeof(DataPack), 0,
			(SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			return 2;
		}
	}

	// closesocket()
	closesocket(sock);
	closesocket(m_sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}