#include "script.hpp"

#include <scriptstdstring.h>

#include "errorhandler.hpp"

Script::Script()
{
    engine = 0;
}

Script::~Script()
{
    lprintf(LOG_INFO, "Shutting down script engine");

    if(engine) {
        engine->ShutDownAndRelease();
    }
}

int Script::init()
{
    engine = asCreateScriptEngine();

    int ret = engine->SetMessageCallback(
                  asFUNCTION(MessageCallback),
                  0,
                  asCALL_CDECL);

    if(ret < 0) {
        lprintf(LOG_ERROR, "Failed to set message callback!");
        return 1;
    }

    RegisterStdString(engine);

    lprintf(LOG_INFO, "Script engine started successfully");

    return 0;
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
