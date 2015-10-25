#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include <angelscript.h>
#include <string>

class Script
{
public:
    Script();
    virtual ~Script();

    int init();
    void run(asIScriptModule* module, const char* func, void* arg = 0);
    void shutdown();

    asIScriptEngine *core;
private:
    asIScriptContext* ctx;

    void registerObjects();
    static void MessageCallback(const asSMessageInfo *msg, void *param);

    // AngelScript functions
    static void print(const std::string &in);
};

#endif // SCRIPT_HPP_
