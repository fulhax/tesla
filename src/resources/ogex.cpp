#include "ogex.hpp"
#include "sha256.hpp"
#include "sha256versionfile.h"
#include "errorhandler.hpp"

#include <openddlparser/OpenDDLParser.h>
#include <stdio.h>

OGEX_Resource::OGEX_Resource() {}
OGEX_Resource::~OGEX_Resource() {}

int OGEX_Resource::load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    lprintf(LOG_INFO, " opening file:%s\n", filename);

    if(!f) {
        lprintf(LOG_ERROR, "File not found:%s\n", filename);
        return 0;
    }

    size_t filesize = 0;
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(filesize == 0) {
        lprintf(LOG_ERROR, "Empty file:%s\n", filename);
        return 0;
    }

    char *buffer = new char[filesize];
    memset(buffer, 0, filesize);

    if(buffer == nullptr) {
        lprintf(LOG_ERROR, "Out of memory while loading:%s\n", filename);
        return 0;
    }

    size_t readbytes = fread(buffer, filesize, 1, f);
    unsigned char checksum[32] = {0};

    calculate_sha256(buffer, readbytes, checksum);

    ODDLParser::OpenDDLParser ddlparser;
    ddlparser.setBuffer(buffer, filesize);
    bool success = false;

    success = ddlparser.parse();

    if(success) {
        ODDLParser::DDLNode *root = ddlparser.getRoot();
    }


    delete[] buffer;

    if(!success) {
        lprintf(LOG_ERROR, "something went wrong loading: %s\n", filename);
    }
    return success;
}
