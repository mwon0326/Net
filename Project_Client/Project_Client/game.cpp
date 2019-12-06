#include "game.h"

int turn = 0;
DataPack sendBuf;
DataPack recvBuf;
SOCKADDR_IN remoteaddr;
SOCKADDR_IN peeraddr;
int addrlen;
SOCKET s_sock;
SOCKET r_sock;
struct ip_mreq mreq;

int Set_Connection()
{
	int retval;
	unsigned int yes = 1;
	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� �ּ� ����ü �ʱ�ȭ(������ ��)
	ZeroMemory(&remoteaddr, sizeof(remoteaddr));
	remoteaddr.sin_family = AF_INET;
	remoteaddr.sin_addr.s_addr = inet_addr(MULTICASTIP);
	remoteaddr.sin_port = htons(REMOTEPORT);

	// socket() // ���ſ� ����
	r_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (r_sock == INVALID_SOCKET) err_quit("socket()");

	// ��Ƽĳ��Ʈ �׷� ����
	mreq.imr_multiaddr.s_addr = inet_addr(MULTICASTIP);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	retval = setsockopt(r_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	if (setsockopt(r_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&yes, sizeof(yes)) < 0)
		return 1;

	// bind()
	SOCKADDR_IN localaddr;
	ZeroMemory(&localaddr, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(LOCALPORT);
	retval = bind(r_sock, (SOCKADDR *)&localaddr, sizeof(localaddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// socket() // �޼��� ������
	s_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (s_sock == INVALID_SOCKET) err_quit("socket()");
}

int Game_Connection(int other, int x, int y, int matrix[][20][20])
{
	if (turn == 0)
	{
		sendBuf.m_other = 5;
	}
	else
	{
		sendBuf.m_other = other;
		sendBuf.m_x = x;
		sendBuf.m_y = y;
	}

	if (turn != 0 && turn % 2 == 0)
	{
		// ������ ������
		int len = sizeof(sendBuf);
		int retval = sendto(s_sock, (char*)&len, sizeof(int), 0,
			(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			return 2;
		}

		retval = sendto(s_sock, (char*)&sendBuf, sizeof(DataPack), 0,
			(SOCKADDR *)&remoteaddr, sizeof(remoteaddr));
		if (retval == SOCKET_ERROR) {
			err_display("sendto()");
			return 2;
		}
	}
	else if (turn != 0 && turn % 2 == 1)
	{ 
		addrlen = sizeof(peeraddr);
		int recvlen;
		int recvRetval = recvfrom(r_sock, (char *)&recvlen, sizeof(int), 0,
			(SOCKADDR *)&peeraddr, &addrlen);
		if (recvRetval == SOCKET_ERROR) {
			err_display("recvfrom()");
			return 2;
		}

		int getSize;
		char suBuffer[BUFSIZE];
		DataPack* data;

		getSize = recvfrom(r_sock, suBuffer, recvlen, 0, (SOCKADDR *)&peeraddr, &addrlen);
		if (getSize == SOCKET_ERROR) {
			err_display("recvfrom()");
			return 2;
		}

		suBuffer[getSize] = '\0';
		data = (DataPack*)suBuffer;

		recvBuf = *data;

		matrix[recvBuf.m_other][(recvBuf.m_x + 1) / 2][recvBuf.m_y] = 1;

		if (recvBuf.game_end == 1)
		{
			gotoxy(1, 24);
			printf("�浹�� �̰���ϴ�\n");
			exit(0);
		}
		else if (recvBuf.game_end == 2)
		{
			gotoxy(1, 24);
			printf("�鵹�� �̰���ϴ�\n");
			exit(0);
		}
	}

	// �۽����� IP �ּ� üũ
	/*if (memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))) {
		printf("[����] �߸��� �������Դϴ�!\n");
		return 2;
	}*/

	turn++;

	return recvBuf.game_end;
}

int end_Set()
{
	// ��Ƽĳ��Ʈ �׷� Ż��
	int retval = setsockopt(r_sock, IPPROTO_IP, IP_DROP_MEMBERSHIP,
		(char *)&mreq, sizeof(mreq));
	if (retval == SOCKET_ERROR) err_quit("setsockopt()");

	// closesocket()
	closesocket(r_sock);
	closesocket(s_sock);

	// ���� ����
	WSACleanup();

	return 0;
}