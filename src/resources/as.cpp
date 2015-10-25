#include "as.hpp"

#include "../engine.hpp"

AS_Resource::AS_Resource()
{

}

AS_Resource::~AS_Resource()
{

}

int AS_Resource::load(const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if(file) {
        int len = 0;
        char *script = 0;

        fseek(file, 0, SEEK_END);
        len = ftell(file);
        fseek(file, 0, SEEK_SET);

        script = new char[len];
        fread(script, len, 1, file);

        fclose(file);

        module = engine.script.core->GetModule(
                    filename,
                    asGM_CREATE_IF_NOT_EXISTS);

        module->AddScriptSection(filename, script, len, 0);
        delete [] script;

        if(module->Build() > 0) {
            lprintf(LOG_ERROR, "Failed to build script");
            return 0;
        }

        return 1;
    }

    lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);

    return 0;
}
