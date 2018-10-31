#ifndef __GAME_H
#define __GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef WINNT
  #include <conio.h>
  #include <windows.h>
  #include "text_windows.h"
#else
  #include <unistd.h>
  #include <termios.h>
  #include <sys/ioctl.h>
  #include "text_linux.h"

  typedef int BOOL;
  #define TRUE  1
  #define FALSE 0
  int Sleep(int ms);
  int getch(void);
#endif

#define GAME_CMD_MAX_X        80   //CMD默认宽度
#define GAME_CMD_MAX_Y        25   //CMD一屏的默认高度


/*一些按键，随便定义一些很大的值，防止冲突*/
#define CMD_LEFT     0X123401 /*左*/
#define CMD_RIGHT    0X123402 /*右*/
#define CMD_DOWN     0X123403 /*下*/
#define CMD_UP       0X123404 /*上*/
#define CMD_SPACE    0X123405 /*空格*/
#define CMD_OTHER    0X123406

typedef unsigned char uchar;

typedef struct
{
    int x;
    int y;
}GamePoint_t;

void cmdMoveToXY(int x, int y);
void cmdCleanScreen();
int cmdWaitInput(int *waittime);


void terisxt();
void snake();
void starwar();


#endif


