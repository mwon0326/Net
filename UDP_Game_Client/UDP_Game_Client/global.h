#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <ws2tcpip.h>
#include <stdio.h>
struct DataPack {
	int game_end;
	int m_x;
	int m_y;
	int m_other;
};

void err_quit(char *msg);
void err_display(char *msg);
