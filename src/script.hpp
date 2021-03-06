#ifndef SCRIPT_HPP_
#define SCRIPT_HPP_

#include <angelscript.h>
#include <stdio.h>

#include <string>

#define MAX_CONTEXTS 8

class ScriptResource;


template<typename T> void asConstructor(void *mem)
{
    new(mem) T();
}

template<typename T> void asDestructor(void *mem)
{
    static_cast<T *>(mem)->~T();
}

template<typename T> class ASClass
{
    int ref_count;
public:
    ASClass()
    {
        printf("Running asclass constructor\n");
        ref_count = 1;
    }
    virtual ~ASClass() {}

    void addRef()
    {
        ref_count++;
        printf("Adding ref %d\n", ref_count);
    }
    void releaseRef()
    {
        printf("Releasing ref %d\n", ref_count);
        if (--ref_count == 0) {
            delete this;
        }
    }
    static T *factory()
    {
        printf("Called factory\n");
        return new T();
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
