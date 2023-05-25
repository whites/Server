#include <signal.h>
#include "NetMng.h"
#include "ServerApp.h"

static void sig_int(int num)
{
    ServerApp::active_ = false;
}

int main(int argc, char** argv)
{
    signal(SIGINT, sig_int);

    ServerApp serverApp;

    // register modules
    if(serverApp.init(argc, argv))
    {
        serverApp.registerModule(NetModule::GetInstance());

        serverApp.start();
    }

    return 0;
}