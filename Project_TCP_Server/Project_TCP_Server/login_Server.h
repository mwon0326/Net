#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SERVERPORT 9000
#define BUFSIZE    512

void err_quit(char *msg);
void err_display(char *msg);
DWORD WINAPI ProcessClient(LPVOID arg);
int Login();
