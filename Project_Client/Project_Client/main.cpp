#include "login.h"
#include "Multicast_Rev.h"
#include "main.h"
#include "game.h"

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

void display_stone(int matrix[][20][20])
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

int main(int argc, char *argv[])
{
	int x = 1, y = 1, other = 0;
	char key;
	char *stone[2] = { "○", "●" };
	int matrix[2][20][20] = { 0 };

	int start = TCP_Login();

	if (start == 0)
	{
		system("cls");
		Set_Connection();
		int my_stone = Game_Connection(0, 0, 0, matrix);
		other = my_stone;

		while (1)
		{
			gotoxy(1, 24);
			printf("                                                  "); //에러문 지우기

			gotoxy(1, 1);
			draw_check01(18, 18);
			gotoxy(x, y);

			printf("%s", stone[other]);
			display_stone(matrix);
			gotoxy(1, 20);

			printf("방향키로 움직이고 ");
			printf("스페이스 키를 누르시오.");

			gotoxy(1, 21);

			printf("돌을 놓았으면 상대방 차례입니다. ");
			key = _getch();

			if (key == 27)
				exit(0);

			else if (key == 32)
			{
				matrix[other][(x + 1) / 2][y] = 1;
				Game_Connection(other, x, y, matrix);
			}
			else if (key >= 72)
				move_arrow_key(key, &x, &y, 37, 19);
		}

		end_Set();
	}
}
