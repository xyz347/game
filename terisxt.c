#include "game.h"


/*游戏区域相对屏幕的偏移，单位是字符，经验值*/
#define X0  15 /*X的偏移量*/
#define Y0  3  /*Y的偏移量*/

#define MAX_LEVEL 10  /*最高级别*/

/*方块的一些属性*/
#define BLK_MAX_TYPE   7   //方块类型 7种
#define BLK_MAX_DIR    4   //方块方向 7种
#define BLK_WIDTH      4   //方块宽度 4
#define BLK_HEIGHT     4   //方块高度 4

#define ROOM_WIDTH     16  //游戏空间宽度
#define ROOM_HEIGHT    20  //游戏空间高度

/*边框宽度，为了简化算法，这里把边框当成一个已经被一个方块占据的空间*/
#define EDGE_WIDTH     4

#define INFO_X_OFFSET ((ROOM_WIDTH+EDGE_WIDTH)*2+3)

static int m_count; /*分数*/
static int m_level; //等级

/*各个等级的分数段，随便定义的*/
static int m_lvlmap[MAX_LEVEL] = {10,20,30,50,70,100,140,200,260};
static BOOL m_bpause = FALSE;
static BOOL m_bdead = FALSE;

//一个4*4的数组表示一个方块，也即一个方块是一个4*4的阵列
static uchar m_aucBlk[BLK_MAX_TYPE][BLK_MAX_DIR][BLK_WIDTH][BLK_HEIGHT] =
{
    // ****
    {
        {
            {1,1,1,1},// **** 
            {0,0,0,0},
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},// *
            {0,1,0,0},// *
            {0,1,0,0},// *
            {0,1,0,0},// *
        },
        {
            {1,1,1,1},// ****
            {0,0,0,0},
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},// *
            {0,1,0,0},// *
            {0,1,0,0},// *
            {0,1,0,0},// *
        },
    },
    // **
    // **
    {
        {
            {0,1,1,0},// **
            {0,1,1,0},// **
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,1,0},// **
            {0,1,1,0},// **
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,1,0},// **
            {0,1,1,0},// **
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,1,0},// **
            {0,1,1,0},// **
            {0,0,0,0},
            {0,0,0,0},
        },
    },
    //  *
    // ***
    {
        {
            {0,1,0,0},//  *
            {1,1,1,0},// ***
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {1,0,0,0},// *
            {1,1,0,0},// **
            {1,0,0,0},// *
            {0,0,0,0},
        },
        {
            {1,1,1,0},// ***
            {0,1,0,0},//  *
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},//  *
            {1,1,0,0},// **
            {0,1,0,0},//  *
            {0,0,0,0},
        },
    },
    // ***
    //   *
    {
        {
            {1,1,1,0},// ***
            {0,0,1,0},//   *
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},//  *
            {0,1,0,0},//  *
            {1,1,0,0},// **
            {0,0,0,0},
        },
        {
            {1,0,0,0},// *
            {1,1,1,0},// ***
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {1,1,0,0},// **
            {1,0,0,0},// *
            {1,0,0,0},// *
            {0,0,0,0},
        },
    },
    
    // ***
    // *
    {
        {
            {1,1,1,0},// ***
            {1,0,0,0},// *
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {1,1,0,0},// **
            {0,1,0,0},//  *
            {0,1,0,0},//  *
            {0,0,0,0},
        },
        {
            {0,0,1,0},//   *
            {1,1,1,0},// ***
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {1,0,0,0},// *
            {1,0,0,0},// *
            {1,1,0,0},// **
            {0,0,0,0},
        },
    },
    // **
    //  **
    {
        {
            {1,1,0,0},// **
            {0,1,1,0},//  **
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},//  *
            {1,1,0,0},// **
            {1,0,0,0},// *
            {0,0,0,0},
        },
        {
            {1,1,0,0},// **
            {0,1,1,0},//  **
            {0,0,0,0},
            {0,0,0,0},
        },
        {
            {0,1,0,0},//  *
            {1,1,0,0},// **
            {1,0,0,0},// *
            {0,0,0,0},
        },
    },
    //  **
    // **
    {
        {
            {0,1,1,0},//  **
            {1,1,0,0},// **
            {0,0,0,0},
            {0,0,0,0}, 
        },
        {
            {1,0,0,0},// *
            {1,1,0,0},// **
            {0,1,0,0},//  *
            {0,0,0,0},
        },
        {
            {0,1,1,0},//  **
            {1,1,0,0},// **
            {0,0,0,0},
            {0,0,0,0}, 
        },
        {
            {1,0,0,0},// *
            {1,1,0,0},// **
            {0,1,0,0},//  *
            {0,0,0,0},
        },
    },
};


/*
定义有和无两种积木状态，为了好看点，需要加一个空格
也即一个格子需要占据两个字符宽度，这也是为什么后面很多地方要乘以2
*/
static char *m_pacBlk[] = {"  ", TEXT_SOLID_BLOCK};

/*
 * 定义游戏空间，宽16，高20
 * 高度+4是为了给底部加上墙壁
 * 宽度+4+4是要给左边和右边加墙壁
*/
static uchar m_aucRoom[ROOM_HEIGHT+EDGE_WIDTH][ROOM_WIDTH+2*EDGE_WIDTH];

/*一些全局变量*/
static int m_CurX;     //当前方块的X坐标
static int m_CurY;     //当前方块的Y坐标
static int m_CurBlkType;  //当前方块的类型
static int m_CurBlkDir;   //当前方块的方向
static int m_NextBlkType;  //下一个方块的类型
static int m_NextBlkDir;   //下一个方块的方向

//打印分数
static void txPrintCount()
{
    cmdMoveToXY(X0+INFO_X_OFFSET, Y0+4);
    printf("scroe: %d", m_count);
    cmdMoveToXY(X0+INFO_X_OFFSET, Y0+5);
    printf("speed: %d", m_level);
}

static void txDrawRoom()
{
    int i;

    cmdMoveToXY(X0+2*EDGE_WIDTH-1, Y0-1);
    for (i=0; i<=ROOM_WIDTH; i++)
    {
        printf("--");
    }

    for (i=0; i<20; i++)
    {
        cmdMoveToXY(X0+2*EDGE_WIDTH-1, i+Y0);
        printf("|%*c|", 2*ROOM_WIDTH, ' ');
    }

    cmdMoveToXY(X0+2*EDGE_WIDTH-1, Y0+ROOM_HEIGHT);
    for (i=0; i<=ROOM_WIDTH; i++)
    {
        printf("--");
    }
}

/*
 * 在(x, y)处画出一个方块，方块由4*4个积木构成
 * 注意这里的(x, y)不是屏幕坐标，是游戏空间坐标
 * 方块是在一个“容器”里面进行移动变换的，这个
 * 坐标是容器坐标，另外(x, y)是方块的右上角坐标
 */
static void txDrawBlk(int x, int y, uchar aucBlk[BLK_HEIGHT][BLK_WIDTH])
{
    int i;
    int j;

    for (i=0; i<BLK_HEIGHT; ++i)
    {
        //cmdMoveToXY(2*x+X0, y+i+Y0);
        for (j=0; j<BLK_WIDTH; ++j)
        {
            cmdMoveToXY(X0+2*x+(j)*2, y+i+Y0);
            if (aucBlk[i][j]==0 && m_aucRoom[y+i][x+j]==0)
            {
                printf("  ");
            }
            else if (aucBlk[i][j] == 1 || m_aucRoom[y+i][x+j] == 1)
            {
                printf(TEXT_SOLID_BLOCK);
            }
        }
    }

    cmdMoveToXY(0, 0);//将光标移动到左上角，避免在图形中心区闪烁
}

static void txGameOver()
{
    int i, j;
    int infolen = (sizeof(TEXT_GAME_OVER)-1)/2;
    int offset = (ROOM_WIDTH-infolen)/2-1;

    for (i=ROOM_HEIGHT-1; i>=0; --i)
    {
        cmdMoveToXY(X0+2*EDGE_WIDTH, i+Y0);
        for (j=0; j<ROOM_WIDTH; ++j)
        {
            printf("%s", m_pacBlk[1]);
            if (i==ROOM_HEIGHT/2 && j==offset)
            {
                printf("%s", TEXT_GAME_OVER);
                j += infolen;
            }
        }
        Sleep(50);
    }

    cmdMoveToXY(0, 0);
}


/*
 * 生成一个新的方块
*/
static void txNewBlk()
{
    int i,j;

    m_CurX = EDGE_WIDTH+ROOM_WIDTH/2-BLK_WIDTH/2;
    m_CurY = 0;

    m_CurBlkType = m_NextBlkType;
    m_CurBlkDir = m_NextBlkDir;

    for (i=0; i<BLK_HEIGHT; ++i)
    {
        for (j=0; j<BLK_WIDTH; ++j)
        {
            if (m_aucBlk[m_CurBlkType][m_CurBlkDir][i][j] == 1
             && m_aucRoom[m_CurY+i][m_CurX+j] == 1)
            {
                m_bdead = TRUE;
                txGameOver();
                return;
            }
        }
    }

    m_NextBlkType = rand()%7;
    m_NextBlkDir = rand()%4;

    for (i=0; i<BLK_HEIGHT; i++)
    {
        cmdMoveToXY(X0+INFO_X_OFFSET, Y0-2+i);
        for (j=0; j<BLK_WIDTH; j++)
        {
            if (m_aucBlk[m_NextBlkType][m_NextBlkDir][i][j] == 0)
            {
                printf("  ");
            }
            else
            {
                printf(TEXT_SOLID_BLOCK);
            }
        }
    }

    txDrawBlk(m_CurX, m_CurY, m_aucBlk[m_CurBlkType][m_CurBlkDir]);

    cmdMoveToXY(0, 0);//将光标移动到左上角，避免在图形中心区闪烁
}

/*
 * 移动变换方块，(x, y)是方块移动前的空间右上角坐标
 * iDir 是方块的移动方向
 */
static void txMoveBlk(int x, int y, uchar aucBlk[4][4], int iDir)
{
    int    i;

    switch(iDir)
    {
    case CMD_DOWN://向下移动
        //cmdMoveToXY(2*x+X0, y+Y0);
        for (i=0; i<BLK_WIDTH; i++)//擦除上一行的痕迹
        {
            cmdMoveToXY(X0+2*x+(i)*2, y+Y0);//
            if (m_aucRoom[y][x+i] == 0)
            {
                printf("  ");
            }
            else if (m_aucRoom[y][x+i] == 1)
            {
                printf(TEXT_SOLID_BLOCK);
            }
        }
        txDrawBlk(x, y+1, aucBlk);
        break;

    case CMD_LEFT://向左边移动
        for (i=0; i<BLK_HEIGHT; i++)
        {
            if (m_aucRoom[y+i][x+BLK_WIDTH-1] == 0)
            {
                cmdMoveToXY(2*(x+BLK_WIDTH-1)+X0, y+i+Y0);
                printf("  ");
            }
        }
        txDrawBlk(x-1, y, aucBlk);
        break;

    case CMD_RIGHT:
        for (i=0; i<BLK_HEIGHT; i++)
        {
            if (m_aucRoom[y+i][x] == 0)
            {
                cmdMoveToXY(2*x+X0, y+i+Y0);
                printf("  ");
            }
        }
        txDrawBlk(x+1, y, aucBlk);
        break;
    }
}


//碰撞检测
static BOOL txHitCheck(int x, int y, uchar aucBlk[BLK_HEIGHT][BLK_WIDTH])
{
    int i, j;

    for (i=0; i<BLK_HEIGHT; i++)
    {
        for (j=0; j<BLK_WIDTH; j++)
        {
            if (aucBlk[i][j]!=0 && m_aucRoom[y+i][x+j]!=0)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

static void txLevel()
{
    if (m_level<MAX_LEVEL && m_count>m_lvlmap[m_level-1])
    {
        ++m_level;
    }
}

static void txPrintPause()
{
    cmdMoveToXY(X0+INFO_X_OFFSET, Y0+7);

    if (m_bpause)
    {
        printf("Pause");
    }
    else
    {
        printf("     ");
    }
}

//检测方块是否到底了
static void txHitProcess(int x, int y, uchar aucBlk[BLK_HEIGHT][BLK_WIDTH])
{
    int i,j,k;
    int iMaxY, iMinY;
    int iCount = 0;

    for (i=0; i<BLK_HEIGHT; i++)
    {
        for (j=0; j<BLK_WIDTH; j++)
        {
            if (aucBlk[i][j] == 1)
            {
                m_aucRoom[y+i][x+j] = 1;
            }
        }
    }

    //消去行，需要检测的Y轴范围
    iMaxY = y+BLK_HEIGHT-1>=ROOM_HEIGHT?ROOM_HEIGHT-1:y+BLK_HEIGHT-1;
    iMinY = y-(BLK_HEIGHT-1)<0?0:y-(BLK_HEIGHT-1);

    for (i=iMaxY; i>=iMinY; )
    {
        for (j=EDGE_WIDTH; j<EDGE_WIDTH+ROOM_WIDTH; ++j)
        {
            if (m_aucRoom[i][j] == 0)
            {
                break;
            }
        }

        if (j < EDGE_WIDTH+ROOM_WIDTH)
        {
            --i;
            continue;
        }

        for (k=i; k>0; k--)
        {
            for (j=EDGE_WIDTH; j<EDGE_WIDTH+ROOM_WIDTH; ++j)
            {
                m_aucRoom[k][j] = m_aucRoom[k-1][j];
            }
        }

        ++iCount;
    }

    if (iCount == 0)
    {
        return;
    }

    m_count += 2*iCount-1;
    txLevel();

    for (i=iMaxY; i>=0; i--)
    {
        cmdMoveToXY(X0+EDGE_WIDTH*2, Y0+i);
        for (j=EDGE_WIDTH; j<EDGE_WIDTH+ROOM_WIDTH; ++j)
        {
            printf("%s", m_pacBlk[m_aucRoom[i][j]]);
        }
    }

    txPrintCount();
}

/*
 * 初始化游戏空间，墙壁初始化为2
 */
static void trisxtInit()
{
    int  i,j;

    cmdCleanScreen();

    m_bdead = FALSE;
    m_bpause = FALSE;
    m_count = 0;
    m_level = 1;

    /*播撒随即种子，以便生成随即的方块*/
    srand(time(NULL));

    memset(m_aucRoom, 0, sizeof(m_aucRoom));

    /*生成左右墙壁*/
    for (i=0; i<ROOM_HEIGHT; i++)
    {
        for (j=0; j<EDGE_WIDTH; ++j)
        {
            m_aucRoom[i][j] = 2;
            m_aucRoom[i][ROOM_WIDTH+EDGE_WIDTH+j] = 2;
        }
    }

    /*生成底部墙壁*/
    for (i=ROOM_HEIGHT; i<ROOM_HEIGHT+EDGE_WIDTH; ++i)
    {
        for (j=0; j<ROOM_WIDTH+2*EDGE_WIDTH; ++j)
        {
            m_aucRoom[i][j] = 2;
        }
    }

    m_NextBlkType = rand()%7;
    m_NextBlkDir = rand()%4;

    txDrawRoom();
    txNewBlk();
    txPrintCount();
}

void terisxt()
{
    int i = 15;
    int key;
    int iDelay = 1000;

    int tx;
    int ty;
    int iDir = 0;

    trisxtInit();

    //txDrawBlk(3,0,m_aucBlk[0][1]);

    while (1)
    {
        i = 0;

        key = cmdWaitInput(&iDelay); //等待输入
        if (key == 27)  //esc退出
        {
            break;
        }
        else if (m_bdead)
        {
            if (key=='\r' || key=='\n')
            {
                trisxtInit();
            }
            iDelay = 1000;
            continue;
        }
        else if (m_bpause) //如果暂停了
        {
            if (key=='p' || key=='P') //按p重新开始
            {
                m_bpause = FALSE;
                txPrintPause();
            }
            iDelay = 1000;
            continue;
        }
        else if (key=='p' || key=='P') //正在玩的时候按p则暂停
        {
            m_bpause = TRUE;
            txPrintPause();
            iDelay = 1000;
            continue;
        }

        if (key == 0)
        {
            iDelay = (MAX_LEVEL-m_level+1)*100;
        }

        tx = m_CurX;
        ty = m_CurY;

        switch(key)
        {
        case 0:
            iDir = CMD_DOWN;
            ty++;
            break;

        case ' ':
            while (!txHitCheck(tx, ty+1, m_aucBlk[m_CurBlkType][m_CurBlkDir]))
            {
                txMoveBlk(tx, ty, m_aucBlk[m_CurBlkType][m_CurBlkDir], CMD_DOWN);
                ++ty;
            }
            m_CurX = tx;
            m_CurY = ty;
            txHitProcess(m_CurX, m_CurY, m_aucBlk[m_CurBlkType][m_CurBlkDir]);
            txNewBlk();
            iDir = CMD_SPACE;
            break;

        case CMD_UP:
            iDir = key;
            break;

        case CMD_DOWN:
            iDir = key;
            ++ty;
            break;

        case CMD_LEFT:
            iDir = key;
            --tx;
            break;

        case CMD_RIGHT:
            iDir = key;
            ++tx;
            break;

        default:
            iDir = CMD_OTHER;
            break;
        }

        if (iDir == CMD_OTHER || iDir == CMD_SPACE)
        {
            continue;
        }

        if (iDir == CMD_UP)
        {
            int tmpdir;
            tmpdir = (m_CurBlkDir+1)%BLK_MAX_DIR;
            if (!txHitCheck(tx, ty, m_aucBlk[m_CurBlkType][tmpdir]))
            {
                m_CurBlkDir = tmpdir;
                txDrawBlk(tx, ty, m_aucBlk[m_CurBlkType][m_CurBlkDir]);
            }
            continue;
        }

        if (txHitCheck(tx, ty, m_aucBlk[m_CurBlkType][m_CurBlkDir]))
        {
            if (iDir == CMD_DOWN)
            {
                txHitProcess(m_CurX, m_CurY, m_aucBlk[m_CurBlkType][m_CurBlkDir]);
                txNewBlk();
            }
            
            continue;
        }

        txMoveBlk(m_CurX, m_CurY, m_aucBlk[m_CurBlkType][m_CurBlkDir], iDir);
        if (iDir == CMD_DOWN)
        {
            ++m_CurY;
        }
        else if (iDir == CMD_LEFT)
        {
            m_CurX -= 1;
        }
        else if (iDir == CMD_RIGHT)
        {
            m_CurX += 1;
        }
    }

    cmdCleanScreen();
}


