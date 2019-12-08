#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define SERVERPORT 9000	// server port ����
#define MULTICASTIP "235.7.8.8"
#define REMOTEPORT  9001

struct DataPack {
	int game_end;
	int m_x;
	int m_y;
	int m_other;
};

void err_quit(char *msg);
void err_display(char *msg);
int Game_End(int other, int x, int y);
int Multi_Set();