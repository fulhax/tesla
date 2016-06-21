#include "script.hpp"

#include <scriptstdstring.h>

#include <string>

#include "entity.hpp"
#include "engine.hpp"
#include "eventhandler.hpp"
#include "camera.hpp"
#include "resource.hpp"
#include "errorhandler.hpp"

Script::Script()
{
    core = nullptr;
    memset(ctx, 0, sizeof(asIScriptContext) * MAX_CONTEXTS);
}

Script::~Script()
{
    lprintf(LOG_INFO, "Shutting down AngelScript");

    for (int i = 0; i < MAX_CONTEXTS; i++) {
        if (ctx[i]) {
            ctx[i]->Unprepare();
        }
    }

    if (core) {
        core->ShutDownAndRelease();
    }
}

void Script::print(const std::string &in)
{
    lprintf(LOG_SCRIPT, in.c_str());
}

void asVec3Construct(float x, float y, float z, void *mem)
{
    new(mem) glm::vec3(x, y, z);
}

void asVec2Construct(float x, float y, void *mem)
{
    new(mem) glm::vec2(x, y);
}

void Script::registerObjects()
{
    core->RegisterGlobalFunction(
        "void print(const string &in)",
        asFUNCTION(Script::print),
        asCALL_CDECL);

    core->RegisterObjectType(
        "vec3",
        sizeof(glm::vec3),
        asOBJ_VALUE | asGetTypeTraits<glm::vec3>());
    core->RegisterObjectBehaviour(
        "vec3",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(asConstructor<glm::vec3>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectBehaviour(
        "vec3",
        asBEHAVE_CONSTRUCT,
        "void f(float x, float y, float z)",
        asFUNCTION(asVec3Construct),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectBehaviour(
        "vec3",
        asBEHAVE_DESTRUCT,
        "void f()",
        asFUNCTION(asDestructor<glm::vec3>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectProperty(
        "vec3",
        "float x",
        asOFFSET(glm::vec3, x));
    core->RegisterObjectProperty(
        "vec3",
        "float y",
        asOFFSET(glm::vec3, y));
    core->RegisterObjectProperty(
        "vec3",
        "float z",
        asOFFSET(glm::vec3, z));
    core->RegisterObjectMethod(
        "vec3",
        "vec3& opAssign(const vec3 &in)",
        asMETHODPR(
            glm::vec3,
            operator=,
            (const glm::vec3 &),
            glm::vec3 &),
        asCALL_THISCALL);

    core->RegisterObjectType(
        "vec2",
        sizeof(glm::vec2),
        asOBJ_VALUE | asGetTypeTraits<glm::vec2>());
    core->RegisterObjectBehaviour(
        "vec2",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(asConstructor<glm::vec2>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectBehaviour(
        "vec2",
        asBEHAVE_CONSTRUCT,
        "void f(float x, float y)",
        asFUNCTION(asVec2Construct),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectBehaviour(
        "vec2",
        asBEHAVE_DESTRUCT,
        "void f()",
        asFUNCTION(asDestructor<glm::vec2>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectProperty(
        "vec2",
        "float x",
        asOFFSET(glm::vec2, x));
    core->RegisterObjectProperty(
        "vec2",
        "float y",
        asOFFSET(glm::vec2, y));
    core->RegisterObjectMethod(
        "vec2",
        "vec2& opAssign(const vec2 &in)",
        asMETHODPR(
            glm::vec2,
            operator=,
            (const glm::vec2 &),
            glm::vec2 &),
        asCALL_THISCALL);

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
        asMETHOD(Ui, printDef),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Ui",
        "void print(const string &in, int x, int y, const string &in)",
        asMETHOD(Ui, print),
        asCALL_THISCALL);

    core->RegisterObjectType("Camera", 0, asOBJ_REF);
    core->RegisterObjectBehaviour(
        "Camera",
        asBEHAVE_ADDREF,
        "void f()",
        asMETHOD(Camera, addRef),
        asCALL_THISCALL);
    core->RegisterObjectBehaviour(
        "Camera",
        asBEHAVE_RELEASE,
        "void f()",
        asMETHOD(Camera, releaseRef),
        asCALL_THISCALL);
    core->RegisterObjectProperty(
        "Camera",
        "float pitch",
        asOFFSET(Camera, pitch));
    core->RegisterObjectProperty(
        "Camera",
        "float yaw",
        asOFFSET(Camera, yaw));

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
        "float getTick()",
        asMETHOD(Engine, getTick),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Engine",
        "int getFPS()",
        asMETHOD(Engine, getFPS),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Engine",
        "float getMS()",
        asMETHOD(Engine, getMS),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Engine",
        "void createEntityType(string &in, string &in)",
        asMETHOD(Engine, createEntityType),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Engine",
        "void spawnEntity(string &in, vec3 &in, vec3 &in = vec3(0,0,0))",
        asMETHOD(Engine, spawnEntity),
        asCALL_THISCALL);
    core->RegisterObjectProperty(
        "Engine",
        "Camera camera",
        asOFFSET(Engine, camera));

    core->RegisterObjectType(
        "Event",
        sizeof(Event),
        asOBJ_VALUE | asGetTypeTraits<Event>());
    core->RegisterObjectBehaviour(
        "Event",
        asBEHAVE_CONSTRUCT,
        "void f()",
        asFUNCTION(asConstructor<Event>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectBehaviour(
        "Event",
        asBEHAVE_DESTRUCT,
        "void f()",
        asFUNCTION(asDestructor<Event>),
        asCALL_CDECL_OBJLAST);
    core->RegisterObjectProperty(
        "Event",
        "string event",
        asOFFSET(Event, event));
    core->RegisterObjectProperty(
        "Event",
        "string data",
        asOFFSET(Event, data));
    core->RegisterObjectMethod(
        "Event",
        "Event& opAssign(const Event &in)",
        asMETHODPR(
            Event,
            operator=,
            (const Event &),
            Event &),
        asCALL_THISCALL);

    core->RegisterObjectType("Events", 0, asOBJ_REF);
    core->RegisterGlobalProperty("Events events", &engine.events);
    core->RegisterObjectBehaviour(
        "Events",
        asBEHAVE_ADDREF,
        "void f()",
        asMETHOD(Engine, addRef),
        asCALL_THISCALL);
    core->RegisterObjectBehaviour(
        "Events",
        asBEHAVE_RELEASE,
        "void f()",
        asMETHOD(Engine, releaseRef),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Events",
        "Event &poll()",
        asMETHOD(EventHandler, poll),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Events",
        "int count()",
        asMETHOD(EventHandler, count),
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
    core->RegisterObjectProperty(
        "Entity",
        "float scale",
        asOFFSET(Entity, scale));
    core->RegisterObjectMethod(
        "Entity",
        "void setModel(const string &in)",
        asMETHOD(Entity, setModel),
        asCALL_THISCALL);
    core->RegisterObjectMethod(
        "Entity",
        "void setMass(const float mass)",
        asMETHOD(Entity, setMass),
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

    if (ret < 0) {
        lprintf(LOG_ERROR, "Failed to set message callback!");
        return 1;
    }

    RegisterStdString(core);
    registerObjects();

    for (int i = 0; i < MAX_CONTEXTS; ++i) {
        ctx[i] = core->CreateContext();
    }

    lprintf(LOG_INFO, "AngelScript started successfully");

    return 0;
}

void Script::run(ScriptResource *script, const char *func, void *arg)
{
    if (script == nullptr) {
        return;
    }

    if (!script->module) {
        lprintf(LOG_WARNING, "No script loaded!");
        script->failed = true;
        return;
    }

    asIScriptFunction *f = script->module->GetFunctionByDecl(func);

    if (!f) {
        lprintf(LOG_WARNING, "Unable to find function ^g\"%s\"^0", func);
        script->failed = true;
        return;
    }

    int curr = 0;
    bool found = false;

    do {
        found = false;

        for (curr = 0; curr < MAX_CONTEXTS; curr++) {
            int r = ctx[curr]->GetState();

            if (r == asEXECUTION_ERROR) {
                lprintf(
                    LOG_WARNING,
                    "Script failed ^g\"%s\"^0",
                    ctx[curr]->GetExceptionString());

                found = true;
                break;
            } else if (r == asEXECUTION_FINISHED ||
                       r == asEXECUTION_UNINITIALIZED) {

                found = true;
                break;
            }
        }
    } while (!found);

    if (curr != MAX_CONTEXTS) {
        ctx[curr]->Prepare(f);

        if (arg) {
            ctx[curr]->SetArgObject(0, arg);
        }

        ctx[curr]->Execute();
    }
}

void Script::MessageCallback(const asSMessageInfo *msg, void *param)
{
    logType type = LOG_ERROR;

    if (msg->type == asMSGTYPE_WARNING) {
        type = LOG_WARNING;
    } else if (msg->type == asMSGTYPE_INFORMATION) {
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
