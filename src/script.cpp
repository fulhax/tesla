#include "script.hpp"

#include <scriptstdstring.h>

#include <string>

#include "engine.hpp"
#include "entity.hpp"
#include "errorhandler.hpp"
#include "resource.hpp"

Script::Script()
{
    core = nullptr;
    memset(ctx, 0, sizeof(asIScriptContext) * MAX_CONTEXTS);
}

Script::~Script()
{
    lprintf(LOG_INFO, "Shutting down AngelScript");

    for(int i = 0; i < MAX_CONTEXTS; i++) {
        if(ctx[i]) {
            ctx[i]->Unprepare();
        }
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

    core->RegisterObjectType("Ui", 0, asOBJ_REF);
    core->RegisterGlobalProperty("Ui ui", &engine.ui);
    core->RegisterObjectBehaviour(
        "Ui",
        asBEHAVE_ADDREF,
        "void f()",
        asMETHOD(Ui, addRef),
        asCALL_THISCALL);
    core->RegisterObjectBehaviour(
        "Ui",
        asBEHAVE_RELEASE,
        "void f()",
        asMETHOD(Ui, releaseRef),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Ui",
        "void print(int x, int y, const string &in)",
        asMETHOD(Ui, print),
        asCALL_THISCALL);

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
    core->RegisterObjectMethod(
        "Engine",
        "int getFPS()",
        asMETHOD(Engine, getFPS),
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
                  nullptr,
                  asCALL_CDECL);

    if(ret < 0) {
        lprintf(LOG_ERROR, "Failed to set message callback!");
        return 1;
    }

    RegisterStdString(core);
    registerObjects();

    for(int i = 0; i < MAX_CONTEXTS; ++i) {
        ctx[i] = core->CreateContext();
    }

    lprintf(LOG_INFO, "AngelScript started successfully");

    return 0;
}

void Script::run(ScriptResource *script, const char *func, void *arg)
{
    if(!script) {
        lprintf(LOG_WARNING, "No script loaded!");
        script->failed = true;
        return;
    }

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

    int curr = 0;
    bool found = false;

    do {
        found = false;

        for(curr = 0; curr < MAX_CONTEXTS; curr++) {
            int r = ctx[curr]->GetState();

            if(r == asEXECUTION_ERROR) {
                lprintf(
                    LOG_WARNING,
                    "Script failed ^g\"%s\"^0",
                    ctx[curr]->GetExceptionString());

                found = true;
                break;
            } else if(r == asEXECUTION_FINISHED ||
                      r == asEXECUTION_UNINITIALIZED) {

                found = true;
                break;
            }
        }
    } while(!found);

    if(curr != MAX_CONTEXTS) {
        ctx[curr]->Prepare(f);

        if(arg) {
            ctx[curr]->SetArgObject(0, arg);
        }

        ctx[curr]->Execute();
    }
}

void Script::MessageCallback(const asSMessageInfo *msg, void *param)
{
    logType type = LOG_ERROR;

    if(msg->type == asMSGTYPE_WARNING) {
        type = LOG_WARNING;
    } else if(msg->type == asMSGTYPE_INFORMATION) {
        type = LOG_INFO;
    }

    lprintf(
        type,
        "^b%s^0 (^y%d^0,^y%d^0): %s",
        msg->section,
        msg->row,
        msg->col,
        msg->message);
}
