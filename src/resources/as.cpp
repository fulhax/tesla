#include "as.hpp"

#include <scriptbuilder.h>

#include "../engine.hpp"

AS_Resource::AS_Resource()
{

}

AS_Resource::~AS_Resource()
{

}

int AS_Resource::load(const char *filename)
{
    CScriptBuilder builder;

    if (builder.StartNewModule(engine.script.core, filename) >= 0) {

        if (builder.AddSectionFromFile(filename) < 0) {
            lprintf(LOG_ERROR, "Unable to read ^g\"%s\"^0", filename);
            return 0;
        }

        if (builder.BuildModule() > 0) {
            lprintf(LOG_ERROR, "Failed to build script");
            return 0;
        }

        module = builder.GetModule();

        return 1;
    }

    lprintf(LOG_WARNING, "Unable to create module for ^g\"%s\"^0", filename);

    return 0;
}
