#include "ogex.hpp"
#include "sha256.hpp"
#include "sha256versionfile.h"
#include "errorhandler.hpp"

#include <openddlparser/OpenDDLParser.h>
#include <stdio.h>

OGEX_Resource::OGEX_Resource() {}
OGEX_Resource::~OGEX_Resource() {}

void printsubnodes(ODDLParser::DDLNode *node, int level)
{

    ODDLParser::DDLNode::DllNodeList children = node->getChildNodeList();
    for(size_t i = 0; i < children.size(); i++) {
        ODDLParser::DDLNode *child = children[i];
        for(int j = 0; j < level; j++) {
            fprintf(stdout, "    ");
        }
        fprintf(stdout, "node:%s", child->getType().c_str());
        if(strlen(child->getName().c_str()) > 0) {
            fprintf(stdout, " name:%s", child->getName().c_str());
        }
        ODDLParser::Value *values = child->getValue();
        fprintf(stdout, "\n");
        while(values != nullptr) {
            for(int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }
            switch(values->m_type) {
                case ODDLParser::Value::ddl_none:
                    break;
                case ODDLParser::Value::ddl_float: {
                    float val = values->getFloat();
                    fprintf(stdout, "%f\n", val);
                    break;
                }
                case ODDLParser::Value::ddl_int32: {
                    int val = values->getInt32();
                    fprintf(stdout, "%i\n", val);
                    break;
                }
                default:
                    break;
            }
            values = values->getNext();
        }
        ODDLParser::DataArrayList *array = child->getDataArrayList();
        while(array != nullptr) {
            ODDLParser::Value *values = array->m_dataList;
            while(values != nullptr) {
                for(int j = 0; j <= level; j++) {
                    fprintf(stdout, "    ");
                }
                switch(values->m_type) {
                    case ODDLParser::Value::ddl_none:
                        break;
                    case ODDLParser::Value::ddl_float: {
                        float val = values->getFloat();
                        fprintf(stdout, "%f\n", val);
                        break;
                    }
                    case ODDLParser::Value::ddl_int32: {
                        int val = values->getInt32();
                        fprintf(stdout, "%i\n", val);
                        break;
                    }
                    default:
                        break;
                }
                values=values->getNext();
            }
            array = array->m_next;
        }
        printsubnodes(child, level + 1);
    }
}

int OGEX_Resource::load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    lprintf(LOG_INFO, " opening file:%s", filename);

    if(!f) {
        lprintf(LOG_ERROR, "File not found:%s", filename);
        return 0;
    }

    size_t filesize = 0;
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if(filesize == 0) {
        lprintf(LOG_ERROR, "Empty file:%s", filename);
        return 0;
    }

    char *buffer = new char[filesize];
    memset(buffer, 0, filesize);

    if(buffer == nullptr) {
        lprintf(LOG_ERROR, "Out of memory while loading:%s", filename);
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
        printsubnodes(root, 0);
    }


    delete[] buffer;

    if(!success) {
        lprintf(LOG_ERROR, "something went wrong loading: %s", filename);
    }
    return success;
}
