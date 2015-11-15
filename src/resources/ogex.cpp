#include "ogex.hpp"
#include "sha256versionfile.h"
#include <stdio.h>
#include <openddlparser/OpenDDLParser.h>
#include <openssl/sha.h>

OGEX_Resource::OGEX_Resource() {}
OGEX_Resource::~OGEX_Resource() {}

int OGEX_Resource::load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    fprintf(stdout, " opening file:%s\n", filename);

    if(!f) {
        fprintf(stderr, "File not found:%s\n", filename);
        return 0;
    }

    size_t filesize = 0;
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(filesize == 0) {
        fprintf(stderr, "Empty file:%s\n", filename);
        return 0;
    }

    char *buffer = new char[filesize];
    memset(buffer, 0, filesize);

    if(buffer == nullptr) {
        fprintf(stderr, "Out of memory while loading:%s\n", filename);
        return 0;
    }

    size_t readbytes = fread(buffer, filesize, 1, f);
    unsigned char checksum[32] = {0};

    SHA256((unsigned char *)&buffer, filesize, checksum);

    ODDLParser::OpenDDLParser ddlparser;
    ddlparser.setBuffer(buffer, filesize);
    //ddlparser cc.setLogCallback(
    bool success = false;

    try {
        success = ddlparser.parse();
    } catch(...) {
        return 0;
    }

    //if(success)
    //{
    //ODDLParser::DDLNode* root = ddlparser.getRoot();
    //}

    delete[] buffer;
    return 1;
}
