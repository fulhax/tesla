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
            printsubnodes(root, 0);
            DDLNode::DllNodeList children = root->getChildNodeList();
            for(size_t i = 0; i < children.size(); i++) {

                DDLNode *child = children[i];
                const char *type = child->getType().c_str();
                if(strcmp(type, "GeometryObject") == 0) {
                    fprintf(stdout, "geometryObjectFound\n");
                    for(DDLNode *n :
                        child->getChildNodeList()[0]->getChildNodeList()) {
                        fprintf(stdout, "n:%s\n", n->getType().c_str());
                        if(strcmp(n->getType().c_str(), "VertexArray") == 0) {

                            Property *prop = n->getProperties();
                            //if(prop != nullptr) {
                            //fprintf(stdout, ": property");
                            //}
                            while(prop != nullptr) {
                                if(strcmp(prop->m_key->m_text.m_buffer, "attrib") == 0) {
                                    if(prop->m_value->m_type == Value::ddl_string) {
                                        const char *attrib = prop->m_value->getString();
                                        if(strcmp(attrib, "position") == 0) {
                                            fprintf(stdout, "  position buffer: ");
                                            DataArrayList *array = n->getDataArrayList();
                                            if(array) {
                                                size_t arraylen = 0;

                                                while(array != nullptr) {
                                                    arraylen += array->m_numItems;
                                                    array = array->m_next;
                                                }
                                                pos_vb = new float[arraylen];
                                                fprintf(stdout, "%zu\n", arraylen);
                                            }

                                        } else if(strcmp(attrib, "texcoord") == 0) {
                                            fprintf(stdout, "  uv buffer\n");
                                        } else if(strcmp(attrib, "normal") == 0) {
                                            fprintf(stdout, "  normal buffer\n");
                                        }
                                    }
                                    break; // does not care about any other properties for vertex arrays
                                }
                                prop = prop->m_next;
                            }
                        }
                    }


                    break; // TODO: handle more than one GeometryObject
                }

                //fprintf(stdout, "node:%s", child->getType().c_str());
            }

        }
    }

    delete[] buffer;

    if(!success) {
        lprintf(LOG_ERROR, "something went wrong loading: %s", filename);
    }
    return success;
}
