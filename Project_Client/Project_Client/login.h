#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <string.h>
#include "main.h"

#define SERVERIP   "127.0.0.1" // stand alone
#define SERVERPORT 9000
#define BUFSIZE 512 // 512 ����Ʈ������ ���� ��� ����

int recvn(SOCKET s, char *buf, int len, int flags);
int TCP_Login();

