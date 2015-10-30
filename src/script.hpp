#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include <angelscript.h>
#include <string>

class ScriptResource;

class Script
{
public:
    Script();
    virtual ~Script();

    int init();
    void run(ScriptResource *script, const char *func, void *arg = 0);

    asIScriptEngine *core;
private:
    asIScriptContext *ctx;

    void registerObjects();
    static void MessageCallback(const asSMessageInfo *msg, void *param);

    // AngelScript functions
    static void print(const std::string &in);
};

#endif // SCRIPT_HPP_
