#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
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
