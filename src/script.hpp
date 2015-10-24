#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include <angelscript.h>

class Script
{
public:
    Script();
    virtual ~Script();

    int init();
    void shutdown();
private:
    static void MessageCallback(const asSMessageInfo *msg, void *param);
    asIScriptEngine *engine;
};

#endif // SCRIPT_HPP_
