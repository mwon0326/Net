#include "main.h"
#include "login.h"

SOCKET sock;
SOCKADDR_IN serveraddr;
DataPack sendBuf;
SOCKADDR_IN peeraddr;

int matrix[2][20][20];
int my_ston = 0;

int Set_UDP()
{
	int retval;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// socket()
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
		err_quit("socket()");

	// 소켓 주소 구조체 초기화
	serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
}

void gotoxy(int x, int y)
{
	COORD Pos = { x - 1, y - 1 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}

void move_arrow_key(char key, int *x1, int *y1, int x_b, int y_b)
{
	switch (key)
	{
	case 72:
		*y1 = *y1 - 1; //위쪽 방향의 화살표 키 입력
		if (*y1 < 1)
			*y1 = 1;
		break;
	case 75:
		*x1 = *x1 - 2; //왼쪽 방향의 화살표 키 입력
		if (*x1 < 1)
			*x1 = 1;
		break;
	case 77:
		*x1 = *x1 + 2; //오른쪽 방향의 화살표 키 입력
		if (*x1 > x_b)
			*x1 = x_b;
		break;
	case 80:
		*y1 = *y1 + 1; //아래쪽 방향의 화살표 키 입력
		if (*y1 > y_b)
			*y1 = y_b;
		break;
	default:
		return;
	}
}

void draw_check01(int c, int r)
{
	int i, j;
	unsigned char a = 0xa6;
	unsigned char b[12];

	for (i = 1; i < 12; i++)
		b[i] = 0xa0 + i;

	printf("%c%c", a, b[3]);

	for (i = 0; i < c - 1; i++)
		printf("%c%c", a, b[8]);

	printf("%c%c", a, b[4]);
	printf("\n");

	for (i = 0; i < r - 1; i++)
	{
		printf("%c%c", a, b[7]);
		for (j = 0; j < c - 1; j++)
			printf("%c%c", a, b[11]);

		printf("%c%c", a, b[9]);
		printf("\n");
	}

	printf("%c%c", a, b[6]);

	for (i = 0; i < c - 1; i++)
		printf("%c%c", a, b[10]);

	printf("%c%c", a, b[5]);
	printf("\n");
}

void display_stone()
{
	int i, x, y;
	char *stone[2] = { "○", "●" };

	for (i = 0; i < 2; i++)
		for (x = 1; x < 20; x++)
			for (y = 1; y < 20; y++)
			{
				if (matrix[i][x][y] == 1)
				{
					gotoxy(x * 2 - 1, y);
					printf("%s", stone[i]);
				}
			}
}

int UDP_Connect(int other, int x, int y)
{
	// 데이터 통신에 사용할 변수
	char buf[100];
	int len;

	sendBuf.m_other = other;
	sendBuf.m_x = x;
	sendBuf.m_y = y;
	sendBuf.game_end = -1;

	// 데이터 보내기
	len = sizeof(sendBuf);
	int retval = sendto(sock, (char*)&len, sizeof(int), 0,
		(SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		err_display("sendto()");
		return 2;
	}

	retval = sendto(sock, (char*)&sendBuf, sizeof(DataPack), 0,
		(SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) {
		err_display("sendto()");
		return 2;
	}

	// 데이터 받기
	int addrlen = sizeof(peeraddr);
	int recvlen;
	int recvRetval = recvfrom(sock, (char *)&recvlen, sizeof(int), 0,
		(SOCKADDR *)&peeraddr, &addrlen);
	if (recvRetval == SOCKET_ERROR) {
		err_display("recvfrom()");
		return 2;
	}

	int getSize = recvfrom(sock, buf, recvlen, 0, (SOCKADDR *)&peeraddr, &addrlen);
	if (getSize == SOCKET_ERROR) {
		err_display("recvfrom()");
		return 2;
	}

	buf[getSize] = '\0';
	DataPack* data = (DataPack*)buf;

	DataPack recvBuf = *data;

	if (recvBuf.game_end == 5)
		my_ston = recvBuf.m_other;
	else
	{
		if (recvBuf.m_x > 0 && recvBuf.m_y > 0 && recvBuf.m_other < 2)
			matrix[recvBuf.m_other][(recvBuf.m_x + 1) / 2][recvBuf.m_y] = 1;
	}
}

int main()
{
	int start = TCP_Login();
	if (start != 0)
		return 0;

	Set_UDP();

	matrix[2][20][20] = { 0 };
	my_ston = -1;

	int x = 1, y = 1, other = 0;
	char key;
	char *stone[2] = { "○", "●" };

	UDP_Connect(5, 1, 1);

	system("cls");

	while (1)
	{
		gotoxy(1, 24);
		printf("                                                  "); //에러문 지우기

		gotoxy(1, 1);
		draw_check01(18, 18);
		gotoxy(x, y);

		printf("%s", stone[my_ston]);
		display_stone();
		gotoxy(1, 20);

		printf("방향키로 움직이고 ");
		printf("스페이스 키를 누르시오.");

		gotoxy(1, 21);

		printf("돌을 놓았으면 상대방 차례입니다. ");

		gotoxy(1, 22);
		printf("돌을 움직이기 전 엔터 키를 누르세요.");

		key = _getch();

		if (key == 27)
			exit(0);

		else if (key == 32)
		{
			matrix[my_ston][(x + 1) / 2][y] = 1;
			UDP_Connect(my_ston, x, y);
		}
		else if (key >= 72)
		{
			move_arrow_key(key, &x, &y, 37, 19);
		}
		else if (key == 13)
			UDP_Connect(6, 0, 0);
	}

	// closesocket()
	closesocket(sock);

	// 윈속 종료
	WSACleanup();

	return 0;
}