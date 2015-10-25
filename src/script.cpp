#include "script.hpp"

#include <scriptstdstring.h>

#include "errorhandler.hpp"
#include "entity.hpp"

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

void Script::print(const std::string &in)
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
        "void print(const string &in)",
        asFUNCTION(Script::print),
        asCALL_CDECL);

    core->RegisterObjectType("Entity", 0, asOBJ_REF);
    core->RegisterObjectBehaviour(
        "Entity",
        asBEHAVE_FACTORY,
        "Entity@ f()",
        asFUNCTION(Entity::factory),
        asCALL_CDECL);
    core->RegisterObjectBehaviour(
        "Entity",
        asBEHAVE_ADDREF,
        "void f()",
        asMETHOD(Entity, addRef),
        asCALL_THISCALL);
    core->RegisterObjectBehaviour(
        "Entity",
        asBEHAVE_RELEASE,
        "void f()",
        asMETHOD(Entity, releaseRef),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setPos(float x, float y, float z)",
        asMETHOD(Entity, setPos),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setModel(const string &in)",
        asMETHOD(Entity, setModel),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setTexture(const string &in)",
        asMETHOD(Entity, setTexture),
        asCALL_THISCALL);

    ctx = core->CreateContext();

    lprintf(LOG_INFO, "Script engine started successfully");

    return 0;
}

void Script::run(asIScriptModule *module, const char *func, void *arg)
{
    asIScriptFunction *f = module->GetFunctionByDecl(func);

    if(!f) {
        lprintf(LOG_WARNING, "Unable to find function ^g\"%s\"^0", func);
        return;
    }

    ctx->Prepare(f);
    ctx->SetArgObject(0, arg);
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

    lprintf(type, "^b%s^0 (^y%d^0,^y%d^0): %s",
            msg->section, msg->row, msg->col, msg->message);
}
