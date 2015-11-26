#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include <angelscript.h>
#include <string>

#define MAX_CONTEXTS 8

class ScriptResource;

class ASClass
{
    int ref_count;
public:
    ASClass()
    {
        ref_count = 1;
    }
    virtual ~ASClass() {}

    void addRef()
    {
        ref_count++;
    }
    void releaseRef()
    {
        if(--ref_count == 0) {
            delete this;
        }
    }
};

class Script
{
public:
    Script();
    virtual ~Script();

    int init();
    void run(ScriptResource *script, const char *func, void *arg = 0);

    asIScriptEngine *core;
private:
    asIScriptContext *ctx[MAX_CONTEXTS];

    void registerObjects();
    static void MessageCallback(const asSMessageInfo *msg, void *param);

    // AngelScript functions
    static void print(const std::string &in);
};

#endif // SCRIPT_HPP_
