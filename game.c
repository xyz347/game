#include "game.h"

#ifdef WINNT
/*
 * 将屏幕的输出位置移动到(x, y)处，单位是字符而不是像素
 */
void cmdMoveToXY(int x, int y)
{
	COORD stPos = {x, y};
	HANDLE hWhand;	

	hWhand = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hWhand, stPos);
}

/*清屏*/
void cmdCleanScreen()
{
    system("cls");
}
#else
void cmdMoveToXY(int x, int y)
{
    char buf[32];
    sprintf(buf, "\033[%d;%dH", y, x);
    printf("%s", buf);
    //printf("\033[%d;%dH", x, y);
}
void cmdCleanScreen()
{
    printf("\033[2J");
}
int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
BOOL kbhit()
{
    struct termios term, term2;
    int byteswaiting;
    
    tcgetattr(0, &term);
    term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}
int Sleep(int ms)
{
    usleep(ms*1000);
}
#endif

/*
  等待键盘输入
  输入  waittime  等待时间，单位毫秒
  输出  waittime  剩余时间
  返回  按键
*/
int cmdWaitInput(int *waittime)
{
    int iDelay = *waittime;
    int i = 0;

    while (iDelay > 0)
	{
		iDelay -= 10;
		if (kbhit())
		{
			i = getch();
            if (224 == i) //组合键
            {
    			i = getch();
                switch (i)
                {
                  case 72:
                    i = CMD_UP;
                    break;
                  case 80:
                    i = CMD_DOWN;
                    break;
    			  case 75:
    				i = CMD_LEFT;
                    break;
                  case 77:
    			    i = CMD_RIGHT;
    				break;
                  default:
                    i = CMD_OTHER;
                    break;
    			}
            }
			break;
		}
		Sleep(10);
	}

    *waittime = iDelay;

    return i;
}

