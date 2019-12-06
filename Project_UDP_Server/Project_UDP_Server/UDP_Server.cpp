#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ws2tcpip.h>

#define BUFSIZE    512
#define MULTICASTIP "235.7.8.9"
#define LOCALPORT 9000
#define REMOTEPORT  9000

int matrix[2][20][20] = { 0 };
bool a1 = true;
bool a2 = false;

SOCKADDR_IN remoteaddr;
SOCKADDR_IN peeraddr;
int addrlen;
SOCKET s_sock;
SOCKET r_sock;
struct ip_mreq mreq;

struct DataPack {
	int game_end;
	int m_x;
	int m_y;
	int m_other;
};

int game_end(int other, int x, int y);

// ���� �Լ� ���� ��� �� ����
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

// ���� �Լ� ���� ���
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

int main(int argc, char *argv[])
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

	// ������ ��ſ� ����� ����
	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	DataPack sendBuf;
	DataPack recvBuf;

	int turn = 1;

	// Ŭ���̾�Ʈ�� ������ ���
	while (1) {

		if (turn % 2 == 1)
		{
			// ������ �ޱ�
			addrlen = sizeof(clientaddr);

			int recvlen;
			int recvRetval = recvfrom(r_sock, (char *)&recvlen, sizeof(int), 0,
				(SOCKADDR *)&clientaddr, &addrlen);
			if (recvRetval == SOCKET_ERROR) {
				err_display("recvfrom()");
				continue;
			}

			int getSize;
			char suBuffer[BUFSIZE];
			DataPack* data;
			getSize = recvfrom(r_sock, suBuffer, recvlen, 0, (SOCKADDR *)&clientaddr, &addrlen);
			if (getSize == SOCKET_ERROR) {
				err_display("recvfrom()");
				continue;
			}

			suBuffer[getSize] = '\0';
			data = (DataPack*)suBuffer;
			recvBuf = *data;

			printf("[UDP / %s:%d] %d\n", inet_ntoa(clientaddr.sin_addr),
				ntohs(clientaddr.sin_port), recvBuf.m_other);

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
			}
			else
			{
				matrix[recvBuf.m_other][(recvBuf.m_x + 1) / 2][recvBuf.m_y] = 1;
				sendBuf.game_end = game_end(recvBuf.m_other, recvBuf.m_x, recvBuf.m_y);
			}
		}
		else
		{
			// ������ ������
			int len = sizeof(sendBuf);
			retval = sendto(s_sock, (char*)&len, sizeof(int), 0,
				(SOCKADDR *)&clientaddr, sizeof(clientaddr));
			if (retval == SOCKET_ERROR) {
				err_display("sendto()");
				continue;
			}

			retval = sendto(s_sock, (char*)&sendBuf, sizeof(DataPack), 0,
				(SOCKADDR *)&clientaddr, sizeof(clientaddr));
			if (retval == SOCKET_ERROR) {
				err_display("sendto()");
				continue;
			}
		}
	}

	// closesocket()
	closesocket(r_sock);
	closesocket(s_sock);

	// ���� ����
	WSACleanup();
	return 0;
}

int game_end(int other, int x, int y)
{
	int count = 0;
	//���� 5��
	if (matrix[other][(x + 1) / 2][y])
		count++;

	while (count) //��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2) - j][y] && (((x + 1) / 2) - j) >= 1) //�������� �־�� �մϴ�
				count++;
			else
				count = 0; //�ʱ�ȭ
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) //��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2) + j][y] && (((x + 1) / 2) + j) <= 20) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) //��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2)][y - j] && (y - j) >= 1) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) // ��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2)][y + j] && (y + j) <= 20) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) // ��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 + j)][y - j] && (y - j) >= 1 && (((x + 1) / 2) + j) <= 20) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) // ��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 - j)][y + j] && (y + j) <= 20 && (((x + 1) / 2) - j) >= 1) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) // ��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 - j)][y - j] && (y - j) >= 1 && (((x + 1) / 2) - j) >= 1) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}

	count++;

	while (count) // ��
	{
		for (int j = 1; j <= 4; j++)
		{
			if (matrix[other][((x + 1) / 2 + j)][y + j] && (y + j) <= 20 && (((x + 1) / 2) + j) <= 20) //�������� �־�� �մϴ�
				count++;
			else
				count = 0;
		}

		if (count == 5)
			return (other + 1);
	}
	return 0; //������ �� �������� 0 ��ȯ
}