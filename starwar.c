#include "game.h"

#define DELAY_TIME 100

#define SW_LEVEL 0

#define SW_MAX_BULLET 100
#define SW_MAX_ENEMY  100

#define SW_MAX_LIFE 5

//我军战舰样式
static const char *fighters[] = {TEXT_FIGHTER, NULL};

//敌军战舰样式
static const char *enemys[] = {TEXT_ENEMY, NULL};

//我军炮弹样式
static const char *fbullets[] = {" ."};

//敌军炮弹样式
static const char *ebullets[] = {TEXT_EBULLET};

static GamePoint_t m_myPos;

//我军炮弹
static GamePoint_t astFBuPos[SW_MAX_BULLET];
static int m_mybuidx;

static GamePoint_t astEnemyPos[SW_MAX_ENEMY];
static int m_enemyidx;
static int m_lifes;
static int m_count;
static BOOL m_bpause;

static void info()
{
    int i;

    cmdMoveToXY(0, 0);
    for (i=0; i<m_lifes; ++i)
    {
        printf("%c", 3);
    }

    printf("%*cScore:%-4d %s",
           SW_MAX_LIFE+5-m_lifes, ' ', m_count,
           m_bpause?"Pause":"      ");
}

static void init()
{
    int i;

    cmdCleanScreen();

    srand(time(NULL));

    m_lifes = SW_MAX_LIFE;
    m_count = 0;
    m_bpause = FALSE;

    for (i=0; i<SW_MAX_BULLET; ++i)
    {
        astFBuPos[i].x = -1;
    }

    for (i=0; i<SW_MAX_ENEMY; ++i)
    {
        astEnemyPos[i].x = -1;
    }

    m_myPos.x = GAME_CMD_MAX_X/2;
    m_myPos.y = GAME_CMD_MAX_Y-1;

    m_mybuidx = 0;
    m_enemyidx = 0;

    info();

    cmdMoveToXY(m_myPos.x, m_myPos.y);
    printf("%s", fighters[SW_LEVEL]);
}

static void hit()
{
    int i;
    int j;

    for (i=0; i<SW_MAX_BULLET; ++i)
    {
        for (j=0; j<SW_MAX_ENEMY; ++j)
        {
            if (astFBuPos[i].x != -1 && astEnemyPos[j].x != -1
             && astFBuPos[i].y == astEnemyPos[j].y
             && (astFBuPos[i].x==astEnemyPos[j].x||astFBuPos[i].x+1==astEnemyPos[j].x))
            {
                cmdMoveToXY(astFBuPos[i].x, astFBuPos[i].y);
                printf("%s", "  ");
                astFBuPos[i].x = -1;
                astEnemyPos[j].x = -1;
                ++m_count;
            }
        }
    }
}

static void newenemy()
{
    int x;
    int i;

    for (i=0; i<SW_MAX_ENEMY; ++i)
    {
        if (astEnemyPos[i].x != -1)
        {
            cmdMoveToXY(astEnemyPos[i].x, astEnemyPos[i].y);
            printf("%s", "  ");
            ++astEnemyPos[i].y;
            if (astEnemyPos[i].y >= GAME_CMD_MAX_Y-1)
            {
                astEnemyPos[i].x = -1;
                --m_lifes;
            }
            else
            {
                cmdMoveToXY(astEnemyPos[i].x, astEnemyPos[i].y);
                printf("%s", enemys[SW_LEVEL]);
            }
        }
    }

    if (m_lifes == 0)
    {
        cmdMoveToXY(GAME_CMD_MAX_X/2-5, GAME_CMD_MAX_Y/2);
        printf("Game over");
        return;
    }

    hit();

    while (1)
    {
        x = rand()%(GAME_CMD_MAX_X-1);
        for (i=0; i<SW_MAX_BULLET; ++i)
        {
            if (x==astFBuPos[i].x && astFBuPos[i].y==0)
            {
                break;
            }
        }

        if (i == SW_MAX_BULLET)
        {
            break;
        }
    }

    astEnemyPos[m_enemyidx].x = x;
    astEnemyPos[m_enemyidx].y = 1;
    m_enemyidx = (m_enemyidx+1)%SW_MAX_ENEMY;

    cmdMoveToXY(x, 1);
    printf("%s", enemys[SW_LEVEL]);
}

static void mynewbullet()
{
    int i;

    for (i=0; i<SW_MAX_BULLET; ++i)
    {
        if (astFBuPos[i].x != -1)
        {
            cmdMoveToXY(astFBuPos[i].x, astFBuPos[i].y);
            printf("%s", "  ");
            --astFBuPos[i].y;
            if (astFBuPos[i].y < 1)
            {
                astFBuPos[i].x = -1;
            }
            else
            {
                cmdMoveToXY(astFBuPos[i].x, astFBuPos[i].y);
                printf("%s", fbullets[SW_LEVEL]);
            }
        }
    }

    hit();

    astFBuPos[m_mybuidx].x = m_myPos.x;
    astFBuPos[m_mybuidx].y = m_myPos.y-1;

    cmdMoveToXY(astFBuPos[m_mybuidx].x, astFBuPos[m_mybuidx].y);
    printf("%s", fbullets[SW_LEVEL]);

    m_mybuidx = (m_mybuidx+1)%SW_MAX_BULLET;
}

void starwar()
{
    int key;
    int iDelay = DELAY_TIME;
    int pos = GAME_CMD_MAX_Y-1;
    int x;
    int bucnt = 0;

    init();

    while (1)
    {
        key = cmdWaitInput(&iDelay);
        info();
        if (key == 27)
        {
            break;
        }
        else if (m_lifes == 0)
        {
            iDelay = DELAY_TIME;
            if (key=='\r' || key=='\n')
            {
                init();
            }
            continue;
        }
        else if (m_bpause)
        {
            iDelay = DELAY_TIME;
            if (key=='p' || key=='P')
            {
                m_bpause = FALSE;
            }
            continue;
        }
        else if (key=='p' || key=='P')
        {
            iDelay = DELAY_TIME;
            m_bpause = TRUE;
            continue;
        }
        else if (iDelay <= 0)
        {
            iDelay = DELAY_TIME;

            if (bucnt%8 == 0)
            {
                newenemy();
            }
            else
            {
                mynewbullet();
            }

            ++bucnt;

            if (key == 0)
            {
                continue;
            }
        }

        x = m_myPos.x;

        switch (key)
        {
          case CMD_LEFT:
            --x;
            break;
          case CMD_RIGHT:
            ++x;
            break;
        }

        if (x<0 || x>=GAME_CMD_MAX_X-2 || x==m_myPos.x)
        {
            continue;
        }

        cmdMoveToXY(m_myPos.x, m_myPos.y);
        printf("%s", "  ");
        m_myPos.x = x;
        cmdMoveToXY(m_myPos.x, m_myPos.y);
        printf("%s", fighters[SW_LEVEL]);
    }

    cmdCleanScreen();
}

