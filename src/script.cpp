#include "script.hpp"

#include <scriptstdstring.h>

#include "errorhandler.hpp"

Script::Script()
{
    ctx = 0;
    core = 0;
}

Script::~Script()
{
    lprintf(LOG_INFO, "Shutting down script engine");

    ctx->Unprepare();

    if(core) {
        core->ShutDownAndRelease();
    }
}

void Script::print(std::string in)
{
    lprintf(LOG_SCRIPT, in.c_str());
}

int Script::init()
{
    core = asCreateScriptEngine();

    int ret = core->SetMessageCallback(
                  asFUNCTION(MessageCallback),
                  0,
                  asCALL_CDECL);

    if(ret < 0) {
        lprintf(LOG_ERROR, "Failed to set message callback!");
        return 1;
    }

    RegisterStdString(core);

    core->RegisterGlobalFunction(
        "void print(string &in)",
        asFUNCTION(Script::print),
        asCALL_CDECL);

    ctx = core->CreateContext();

    lprintf(LOG_INFO, "Script engine started successfully");

    return 0;
}

void Script::run(asIScriptModule* module, const char* func)
{
    asIScriptFunction *f = module->GetFunctionByName(func);

    // if(!f) {
    //     lprintf(LOG_WARNING, "Unable to find function ^g\"%s\"^0", func);
    //     return;
    // }

    ctx->Prepare(f);
    ctx->Execute();

    // TODO(c0r73x): Mer contexts, loopa inte!

    int r = 0;
    do {
        r = ctx->GetState();

        if(r == asEXECUTION_ERROR) {
            lprintf(
                LOG_WARNING,
                "Script failed ^g\"%s\"^0",
                ctx->GetExceptionString());
            break;
        }
    } while(r != asEXECUTION_FINISHED);
}

void Script::MessageCallback(const asSMessageInfo *msg, void *param)
{
    logType type = LOG_ERROR;

    if(msg->type == asMSGTYPE_WARNING) {
        type = LOG_WARNING;
    } else if(msg->type == asMSGTYPE_INFORMATION) {
        type = LOG_INFO;
    }

    lprintf(type, "%s (%d,%d):%s\n",
            msg->section, msg->row, msg->col, msg->message);
}
