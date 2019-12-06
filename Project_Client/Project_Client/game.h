#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")

#include "main.h"

#define BUFSIZE    512
#define MULTICASTIP "235.7.8.9"
#define LOCALPORT 9000
#define REMOTEPORT  9000

struct DataPack {
	int game_end;
	int m_x;
	int m_y;
	int m_other;
};

int Game_Connection(int other, int x, int y, int matrix[][20][20]);
int Set_Connection();
int end_Set();