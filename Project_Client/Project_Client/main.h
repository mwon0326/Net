#define _WINSOCK_DEPRECATED_NO_WARNINGS // �ֽ� VC++ ������ �� ��� ����
#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <conio.h>
#include <ws2tcpip.h>

void err_quit(char *msg);
void err_display(char *msg);

void gotoxy(int x, int y);
void move_arrow_key(char key, int *x1, int *y1, int x_b, int y_b);
void draw_check01(int c, int r);
void display_stone(int matrix[][20][20]);