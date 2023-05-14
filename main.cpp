#include "NetMng.h"
#include <signal.h>
static bool running = true;

static void sig_int(int num)
{
    NetMng::GetInstance()->stop();
    running = false;
}

int main()
{
    NetMng::GetInstance()->start();

    signal(SIGINT, sig_int);

    while(running)
    {
        //idle
    }
    return 0;
}