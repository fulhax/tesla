#include "engine.hpp"

int main(int argc, char *argv[])
{
    if(engine.init() == 0) {
        while(engine.running) {
            engine.update();
        }
        engine.shutdown();
    }
    return 0;
}
