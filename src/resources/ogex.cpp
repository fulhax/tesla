#include "ogex.hpp"
#include "sha256.hpp"
#include "sha256versionfile.h"
#include "errorhandler.hpp"

#include <openddlparser/OpenDDLParser.h>
#include <stdio.h>
#include <math.h> // signbit for pritnsubnodes

OGEX_Resource::OGEX_Resource()
{
    indices   = nullptr;
    normal_vb = nullptr;
    pos_vb    = nullptr;
    uv_vb     = nullptr;
}
OGEX_Resource::~OGEX_Resource()
{
    if(indices) {
        delete [] indices;
    }
    if(normal_vb) {
        delete [] normal_vb;
    }
    if(pos_vb) {
        delete [] pos_vb;
    }
    if(uv_vb) {
        delete [] uv_vb;
    }
}

void printsubnodes(ODDLParser::DDLNode *node, int level)
{

    using namespace ODDLParser;
    DDLNode::DllNodeList children = node->getChildNodeList();
    for(size_t i = 0; i < children.size(); i++) {
        DDLNode *child = children[i];
        for(int j = 0; j < level; j++) {
            fprintf(stdout, "    ");
        }
        fprintf(stdout, "node:%s", child->getType().c_str());
        if(strlen(child->getName().c_str()) > 0) {
            fprintf(stdout, " name:%s", child->getName().c_str());
        }
        Property *prop = child->getProperties();
        if(prop != nullptr) {
            fprintf(stdout, ": property");
        }
        while(prop != nullptr) {

            fprintf(stdout, " %s", prop->m_key->m_text.m_buffer);
            if(prop->m_value->m_type == 12) {
                fprintf(stdout, ": %s", prop->m_value->getString());
            }
            prop = prop->m_next;
        }
        Value *values = child->getValue();
        fprintf(stdout, "\n");
        while(values != nullptr) {
            for(int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }
            switch(values->m_type) {
                case Value::ddl_none:
                    break;
                case Value::ddl_float: {
                    float val = values->getFloat();
                    fprintf(stdout, "%f\n", val);
                    break;
                }
                case Value::ddl_int32: {
                    int val = values->getInt32();
                    fprintf(stdout, "%i\n", val);
                    break;
                }
                case Value::ddl_string: {
                    const char *val = values->getString();
                    fprintf(stdout, "%s\n", val);
                    break;
                }
                default:
                    fprintf(stdout, "unhandeled value type\n");
                    break;
            }
            values = values->getNext();
        }
        DataArrayList *array = child->getDataArrayList();
        if(array) {
            size_t arraylen = 0;

            while(array != nullptr) {
                arraylen += array->m_numItems;
                array = array->m_next;
            }
            for(int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }
            fprintf(stdout, "arraylen:%zu\n", arraylen);
        }
        array = child->getDataArrayList();
        while(array != nullptr) {
            Value *values = array->m_dataList;
            for(int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }
            while(values != nullptr) {
                switch(values->m_type) {
                    case Value::ddl_none:
                        break;
                    case Value::ddl_float: {
                        float val = values->getFloat();
                        if(__signbitf(val) == false) {
                            fprintf(stdout, " ");
                        }
                        fprintf(stdout, "%f ", val);
                        break;
                    }
                    case Value::ddl_int32: {
                        int val = values->getInt32();
                        if(val < 10) {
                            fprintf(stdout, " ");
                        }
                        fprintf(stdout, "%i ", val);
                        break;
                    }
                    default:
                        break;
                }
                values = values->getNext();
            }
            fprintf(stdout, "\n");
            array = array->m_next;
        }
        printsubnodes(child, level + 1);
    }
}
float *OGEX_Resource::load_vertexbuffer(ODDLParser::DDLNode *node)
{
    using namespace ODDLParser;
    float *buffer = nullptr;
    DataArrayList *array = node->getDataArrayList();
    if(array) {
        size_t arraylen = 0;

        while(array != nullptr) {
            arraylen += array->m_numItems;
            array = array->m_next;
        }
        buffer = new float[arraylen];
        size_t i = 0;
        array = node->getDataArrayList();
        while(array != nullptr) {
            Value *values = array->m_dataList;
            while(values != nullptr) {
                buffer[i] = values->getFloat();
                i++;
                values = values->getNext();
            }
            array = array->m_next;
        }
        fprintf(stdout, "%zu\n", arraylen);
        numVerts = arraylen; // should always be the same for all attributes
    }
    return buffer;
}
unsigned int *OGEX_Resource::load_indexbuffer(ODDLParser::DDLNode *node)
{
    using namespace ODDLParser;
    return nullptr;

}
bool OGEX_Resource::load_GeometryObject(ODDLParser::DDLNode *node)
{

    using namespace ODDLParser;
    DDLNode *meshnode = node->getChildNodeList()[0];
    for(DDLNode *n : meshnode->getChildNodeList()) {
        fprintf(stdout, "n:%s\n", n->getType().c_str());
        if(strcmp(n->getType().c_str(), "VertexArray") == 0) {
            Property *prop = n->getProperties();
            while(prop != nullptr) {
                if(strcmp(prop->m_key->m_text.m_buffer, "attrib") == 0) {
                    if(prop->m_value->m_type == Value::ddl_string) {
                        const char *attrib = prop->m_value->getString();
                        if(strcmp(attrib, "position") == 0) {
                            pos_vb = load_vertexbuffer(n);
                        } else if(strcmp(attrib, "normal") == 0) {
                            normal_vb = load_vertexbuffer(n);
                        } else if(strcmp(attrib, "texcoord") == 0) {
                            uv_vb = load_vertexbuffer(n);
                        }
                    }
                }
                prop = prop->m_next;
            }
        }
        if(strcmp(n->getType().c_str(), "IndexArray") == 0) {
            indices = load_indexbuffer(n);
        }
    }
    return true;

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
        fclose(f);
        return 0;
    }

    char *buffer = new char[filesize];

    if(buffer == nullptr) {
        lprintf(LOG_ERROR, "Out of memory while loading:%s", filename);
        fclose(f);
        return 0;
    }

    size_t readbytes = fread(buffer, filesize, 1, f);
    fclose(f);

    unsigned char checksum[32] = {0};

    calculate_sha256(buffer, readbytes, checksum);

    bool success = false;
    {
        using namespace ODDLParser;

        OpenDDLParser ddlparser;
        ddlparser.setBuffer(buffer, filesize);

        success = ddlparser.parse();

        if(success) {
            DDLNode *root = ddlparser.getRoot();
            //printsubnodes(root, 0);
            DDLNode::DllNodeList children = root->getChildNodeList();
            for(size_t i = 0; i < children.size(); i++) {

                DDLNode *child = children[i];
                const char *type = child->getType().c_str();
                if(strcmp(type, "GeometryObject") == 0) {
                    fprintf(stdout, "geometryObjectFound\n");
                    load_GeometryObject(child);
                    break; // TODO: handle more than one GeometryObject
                }
                //fprintf(stdout, "node:%s", child->getType().c_str());
            }
            for(unsigned int i = 0; i < numVerts; i += 3) {
                if(pos_vb) {
                    fprintf(stdout, "pos: %f %f %f\n", pos_vb[i], pos_vb[i + 1], pos_vb[i + 2]);
                }
                if(normal_vb) {
                    fprintf(stdout, "normal: %f %f %f\n", normal_vb[i], normal_vb[i + 1],
                            normal_vb[i + 2]);
                }
                if(uv_vb) {
                    fprintf(stdout, "pos: %f %f %f\n", uv_vb[i], uv_vb[i + 1], uv_vb[i + 2]);
                }
            }
        }

    }

    delete[] buffer;

    if(!success) {
        lprintf(LOG_ERROR, "something went wrong loading: %s", filename);
    }
    return success;
}
