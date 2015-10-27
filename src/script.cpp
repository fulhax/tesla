#include "script.hpp"

#include <scriptstdstring.h>

#include <string>

#include "engine.hpp"
#include "entity.hpp"
#include "errorhandler.hpp"
#include "resource.hpp"

Script::Script()
{
    ctx = 0;
    core = 0;
}

Script::~Script()
{
    lprintf(LOG_INFO, "Shutting down script engine");

    if(ctx) {
        ctx->Unprepare();
    }

    if(core) {
        core->ShutDownAndRelease();
    }
}

void Script::print(const std::string &in)
{
    lprintf(LOG_SCRIPT, in.c_str());
}

void Script::registerObjects()
{
    core->RegisterGlobalFunction(
        "void print(const string &in)",
        asFUNCTION(Script::print),
        asCALL_CDECL);

    core->RegisterObjectType("Engine", 0, asOBJ_REF);
    core->RegisterGlobalProperty("Engine engine", &engine);
    core->RegisterObjectBehaviour(
        "Engine",
        asBEHAVE_ADDREF,
        "void f()",
        asMETHOD(Engine, addRef),
        asCALL_THISCALL);
    core->RegisterObjectBehaviour(
        "Engine",
        asBEHAVE_RELEASE,
        "void f()",
        asMETHOD(Engine, releaseRef),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Engine",
        "float getTime()",
        asMETHOD(Engine, getTime),
        asCALL_THISCALL);

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
        "void setRot(float x, float y, float z)",
        asMETHOD(Entity, setRot),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setScale(float size)",
        asMETHOD(Entity, setScale),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setModel(const string &in)",
        asMETHOD(Entity, setModel),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setTexture(const string &in, const string &in)",
        asMETHOD(Entity, setTexture),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void attachShader(const string &in)",
        asMETHOD(Entity, attachShader),
        asCALL_THISCALL);
}

int Script::init()
{
    core = asCreateScriptEngine(ANGELSCRIPT_VERSION);

    int ret = core->SetMessageCallback(
                  asFUNCTION(MessageCallback),
                  0,
                  asCALL_CDECL);

    if(ret < 0) {
        lprintf(LOG_ERROR, "Failed to set message callback!");
        return 1;
    }

    RegisterStdString(core);
    registerObjects();

    ctx = core->CreateContext();

    lprintf(LOG_INFO, "Script engine started successfully");

    return 0;
}

void Script::run(ScriptResource *script, const char *func, void *arg)
{
    if(!script->module) {
        lprintf(LOG_WARNING, "No script loaded!");
        script->failed = true;
        return;
    }

    asIScriptFunction *f = script->module->GetFunctionByDecl(func);

    if(!f) {
        lprintf(LOG_WARNING, "Unable to find function ^g\"%s\"^0", func);
        script->failed = true;
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
