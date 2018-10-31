#include "game.h"

typedef void (*game)();

static const char *list=TEXT_MAIN_USAGE;

static void usage()
{
    cmdCleanScreen();
    printf("%s", list);
}

static game allgame[] = 
{
    terisxt,
    snake,
    starwar
};

int main()
{
    int key;

    usage();

    while (1)
    {
        key = getch();
        if (key == 27)
        {
            cmdCleanScreen();
            break;
        }
        else if (key>='1' && key<='3')
        {
            allgame[key-'1']();
            usage();
        }
    }
    return 0;
}
