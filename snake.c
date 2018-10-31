#include "game.h"

#define DELAY_TIME 1000
#define MAX_X      (GAME_CMD_MAX_X/2)

static GamePoint_t m_Snake[GAME_CMD_MAX_Y*MAX_X]; //蛇身体，
static GamePoint_t m_food; //食物的位置

static int  m_dir;  //当前移动的方向
static int  m_len;  //蛇的长度

static const char *edge=TEXT_SNAKE_EDGE;

static void snakeroom()
{
    int i;
    int j = GAME_CMD_MAX_X;

    cmdMoveToXY(0, 0);
    printf("%.*s", j, edge);

    for (i=1; i<GAME_CMD_MAX_Y; ++i)
    {
        cmdMoveToXY(GAME_CMD_MAX_X-2, i);
        printf("%.2s", &edge[j]);
        j += 2;
    }

    for (i=GAME_CMD_MAX_X-4; i>=0; i-=2)
    {
        cmdMoveToXY(i, GAME_CMD_MAX_Y-1);
        printf("%.2s", &edge[j]);
        j += 2;
    }

    for (i=GAME_CMD_MAX_Y-2; i>=1; --i)
    {
        cmdMoveToXY(0, i);
        printf("%.2s", &edge[j]);
        j += 2;
    }
}


/*初始化*/
static void snakeInit()
{
    int y = GAME_CMD_MAX_Y/2;
    int x = MAX_X/2-1;     //蛇出现在屏幕中心位置
    int i;

    cmdCleanScreen();   //清屏
    srand(time(NULL));  //播撒随机种子

    //画房子
    snakeroom();

    m_len = 3;   //初始化长度为3
    for (i=0; i<m_len; ++i)  //蛇身体位置
    {
        m_Snake[i].y = y;
        m_Snake[i].x = x+(m_len-i-1);
    }

    m_dir = CMD_RIGHT; //初始化方向向右

    cmdMoveToXY(m_Snake[m_len-1].x*2, m_Snake[m_len-1].y); //绘制蛇身
    printf("%s%s%s", TEXT_SNAKE_BODY,TEXT_SNAKE_BODY,TEXT_SNAKE_HEAD);

    cmdMoveToXY(0, 0);
}

//生成新的食物
static void snakeNewFood()
{
    int i;

    while (1)
    {
        m_food.x = 1 + rand()%(MAX_X-2);
        m_food.y = 1 + rand()%(GAME_CMD_MAX_Y-2);//随机生成一个食物位置

        /*
          不能和当前蛇身重合
          算法有问题，如果蛇身很长布满整个屏幕，冲突概率会很高
        */
        for (i=0; i<m_len; ++i)
        {
            if (m_food.x==m_Snake[i].x && m_food.y==m_Snake[i].y)
            {
                break;
            }
        }

        if (i==m_len)
        {
            break;
        }
    }

    cmdMoveToXY(m_food.x*2, m_food.y);
    printf("%c%c", 3,3);  //绘制食物，3是心型，两个心型当食物了
}

//蛇的移动，主要的功能函数了，返回值表示蛇是否死了，TRUE为死了
static BOOL snakeMove()
{
    int x = m_Snake[0].x;
    int y = m_Snake[0].y;  //蛇头
    int i;
    BOOL bEat = FALSE;     //是否吃了食物
    char acBuf[6];         //用来格式化蛇的长度

    switch (m_dir)
    {
      case CMD_UP: //向上移动
        if (m_Snake[0].y == 1) //到了最顶端了，over
        {
            return TRUE;
        }
        --y; //y坐标减去1
        break;
      case CMD_DOWN: //向下移动
        if (m_Snake[0].y == GAME_CMD_MAX_Y-1-1) //到底端了，over
        {
            return TRUE;
        }
        ++y; //y坐标+1
        break;
      case CMD_LEFT: //向左移动
        if (m_Snake[0].x == 1) //到最左了
        {
            return TRUE;
        }
        --x; //x坐标-1
        break;
      case CMD_RIGHT: //向右
        if (m_Snake[0].x == MAX_X-1-1) //到最右端了
        {
            return TRUE;
        }
        ++x; //x坐标+1
        break;
    }

    //判断是否出现了自己咬自己的情况
    for (i=1; i<m_len; ++i)
    {
        if (m_Snake[i].x==x &&m_Snake[i].y==y) //把自己咬了，over
        {
            return TRUE;
        }
    }

    if (x==m_food.x && y==m_food.y)  //吃到食物了
    {
        ++m_len;      //长度+1
        bEat = TRUE;  //吃了
    }
    else //没吃到食物，就清除蛇尾（如果吃到了，蛇长+1，食物变蛇头，蛇尾不能清除）
    {
        cmdMoveToXY(m_Snake[m_len-1].x*2, m_Snake[m_len-1].y);
        printf("%s", "  "); //打印两个空格，蛇尾就不见了
    }

    for (i=m_len-1; i>0; --i) //蛇位置往前挪动
    {
        m_Snake[i].x = m_Snake[i-1].x;
        m_Snake[i].y = m_Snake[i-1].y;
    }

    //移动并绘制蛇头
    m_Snake[0].x = x;
    m_Snake[0].y = y;
    cmdMoveToXY(m_Snake[0].x*2, m_Snake[0].y);
    printf("%s", TEXT_SNAKE_HEAD);

    //食物被吃了，绘制新食物
    if (bEat)
    {
        snakeNewFood();
    }

    //蛇的第二第三节用来显示当前蛇长
    sprintf(acBuf, "%04d", m_len);
    acBuf[5] = acBuf[2];
    acBuf[2] = 0;
    cmdMoveToXY(m_Snake[1].x*2, m_Snake[1].y);
    printf("%s", acBuf);

    acBuf[2] = acBuf[5];
    cmdMoveToXY(m_Snake[2].x*2, m_Snake[2].y);
    printf("%s", &acBuf[2]);

    //后面的蛇身
    if (m_len > 3)
    {
        cmdMoveToXY(m_Snake[3].x*2, m_Snake[3].y);
        printf("%s", TEXT_SNAKE_BODY);
    }

    return FALSE;
}

//控制函数
void snake()
{
    int iDelay = DELAY_TIME;  //延时，越小蛇移动的速度越快，现在是定速
    int key;
    BOOL bDead = FALSE;  //死了？
    BOOL bpause = FALSE; //暂停了？

    snakeInit();     //初始化
    snakeNewFood();  //生成食物

    while (1)   //死循环
    {
        key = cmdWaitInput(&iDelay); //等待输入
        if (key == 27)  //esc退出
        {
            break;
        }
        else if (bDead) //如果已经死了
        {
            iDelay = DELAY_TIME;  //避免死循环
            if (key=='\r' || key=='\n') //回车重新开始
            {
                bDead = FALSE;
                snakeInit();
                snakeNewFood();
            }
            continue;
        }
        else if (bpause) //如果暂停了
        {
            if (key=='p' || key=='P') //按p重新开始
            {
                bpause = FALSE;
                cmdMoveToXY(m_Snake[0].x*2, m_Snake[0].y);
                printf("%s", TEXT_SNAKE_HEAD);
            }
            iDelay = DELAY_TIME;

            continue;
        }
        else if (key=='p' || key=='P') //正在玩的时候按p则暂停
        {
            bpause = TRUE;
            cmdMoveToXY(m_Snake[0].x*2, m_Snake[0].y);
            printf(TEXT_SNAKE_PAUSE);  //蛇头变成“停”
            iDelay = DELAY_TIME;
            continue;
        }
        
        if (key == 0)  //超时，蛇按原来的方向走一步
        {
            iDelay = DELAY_TIME;
        }
        else if ((m_dir==CMD_LEFT && key==CMD_RIGHT)  //如果蛇的移动方向和方向键相反，不处理
              || (m_dir==CMD_RIGHT && key==CMD_LEFT)
              || (m_dir==CMD_UP && key==CMD_DOWN)
              || (m_dir==CMD_DOWN && key==CMD_UP)
              || (key < CMD_LEFT)
              || (key > CMD_UP))
        {
            continue;
        }
        else
        {
            m_dir = key;
            iDelay = DELAY_TIME;
        }

        bDead = snakeMove();  //移动一步
        if (bDead)
        {
            cmdMoveToXY(m_Snake[0].x*2, m_Snake[0].y);
            printf(TEXT_SNAKE_LOOSE); //死了蛇头变成“败”字
        }
    }

    cmdCleanScreen();
}

