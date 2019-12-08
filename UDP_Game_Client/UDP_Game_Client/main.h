#pragma once
#include "global.h"
#include <conio.h>

#define SERVERIP   "127.0.0.1"
#define SERVERPORT 9000

int Set_UDP();
void gotoxy(int x, int y);
void move_arrow_key(char key, int *x1, int *y1, int x_b, int y_b);
void draw_check01(int c, int r);
void display_stone();
