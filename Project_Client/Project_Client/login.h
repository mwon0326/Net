#define _WINSOCK_DEPRECATED_NO_WARNINGS // 최신 VC++ 컴파일 시 경고 방지
#pragma comment(lib, "ws2_32")
#include <string.h>
#include "main.h"

#define SERVERIP   "127.0.0.1" // stand alone
#define SERVERPORT 9000
#define BUFSIZE 512 // 512 바이트까지만 버퍼 사용 가능

int recvn(SOCKET s, char *buf, int len, int flags);
int TCP_Login();

