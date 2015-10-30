#include "engine.hpp"

#ifndef _WIN32
int main(int argc, char *argv[])
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
#endif
{
    if(engine.init() == 0) {
        while(engine.running) {
            engine.update();
        }

        engine.shutdown();
    }

    return 0;
}
