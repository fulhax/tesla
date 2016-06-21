#include "ogex.hpp"
#include "sha256.hpp"
#include "sha256versionfile.h"
#include "errorhandler.hpp"

#include <openddlparser/OpenDDLParser.h>
#include <stdio.h>
#include <math.h> // signbit for pritnsubnodes

OGEX_Resource::OGEX_Resource()
{
    indices     = nullptr;
    verts      = nullptr;
    normal_vb   = nullptr;
    binormal_vb = nullptr;
    tangent_vb  = nullptr;
    color_vb    = nullptr;
    uv_vb       = nullptr;
    numFaces    = 0;
    numVerts    = 0;
}
OGEX_Resource::~OGEX_Resource()
{
    if (normal_vb) {
        delete [] normal_vb;
    }

    if (binormal_vb) {
        delete [] binormal_vb;
    }

    if (tangent_vb) {
        delete [] tangent_vb;
    }

    if (color_vb) {
        delete [] color_vb;
    }

    if (uv_vb) {
        delete [] uv_vb;
    }
}

void printsubnodes(ODDLParser::DDLNode *node, int level)
{

    using namespace ODDLParser;
    DDLNode::DllNodeList children = node->getChildNodeList();

    for (size_t i = 0; i < children.size(); i++) {
        DDLNode *child = children[i];

        for (int j = 0; j < level; j++) {
            fprintf(stdout, "    ");
        }

        fprintf(stdout, "node:%s", child->getType().c_str());

        if (strlen(child->getName().c_str()) > 0) {
            fprintf(stdout, " name:%s", child->getName().c_str());
        }

        Property *prop = child->getProperties();

        if (prop != nullptr) {
            fprintf(stdout, ": property");
        }

        while (prop != nullptr) {

            fprintf(stdout, " %s", prop->m_key->m_text.m_buffer);

            if (prop->m_value->m_type == 12) {
                fprintf(stdout, ": %s", prop->m_value->getString());
            }

            prop = prop->m_next;
        }

        Value *values = child->getValue();
        fprintf(stdout, "\n");

        while (values != nullptr) {
            for (int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }

            switch (values->m_type) {
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

        if (array) {

            for (int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }

            fprintf(stdout, "arraylen:%zu\n", array->size() * array->m_numItems);
        }

        array = child->getDataArrayList();

        while (array != nullptr) {
            Value *values = array->m_dataList;

            for (int j = 0; j <= level; j++) {
                fprintf(stdout, "    ");
            }

            while (values != nullptr) {
                switch (values->m_type) {
                    case Value::ddl_none:
                        break;

                    case Value::ddl_float: {
                        float val = values->getFloat();

                        if (__signbitf(val) == false) {
                            fprintf(stdout, " ");
                        }

                        fprintf(stdout, "%f ", val);
                        break;
                    }

                    case Value::ddl_int32: {
                        int val = values->getInt32();
                        fprintf(stdout, "int32 %i ", val);

                        if (val < 10) {
                            fprintf(stdout, " ");
                        }

                        break;
                    }

                    case Value::ddl_unsigned_int32: {
                        unsigned int val = values->getUnsignedInt32();
                        fprintf(stdout, "uint32 %u ", val);

                        if (val < 10) {
                            fprintf(stdout, " ");
                        }

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

    if (array) {
        size_t arraylen = array->size() * array->m_numItems;
        buffer = new float[arraylen];
        size_t i = 0;
        numVerts = array->size(); // should always be the same for all attributes

        while (array != nullptr) {
            Value *values = array->m_dataList;

            if (values->m_type != Value::ddl_float) {
                lprintf(LOG_ERROR, "wrong datatype for VertexArray");
                delete [] buffer;
                return 0;
            }

            while (values != nullptr) {
                buffer[i] = values->getFloat();
                i++;
                values = values->getNext();
            }

            array = array->m_next;
        }
    } else {
        Value *values = node->getValue();

        if (values) {
            if (values->m_type == Value::ddl_float) {

                buffer = new float[values->size()];
                values = node->getValue();

                size_t i = 0;

                while (values != nullptr) {
                    buffer[i] = values->getFloat();
                    i++;
                    values = values->getNext();
                }
            }
        }
    }

    return buffer;
}
unsigned int *OGEX_Resource::load_indexbuffer(ODDLParser::DDLNode *node)
{
    using namespace ODDLParser;
    unsigned int *buffer = nullptr;
    DataArrayList *array = node->getDataArrayList();

    if (array) {
        size_t arraylen = array->size() * array->m_numItems;

        buffer = new unsigned int[arraylen];
        size_t i = 0;

        while (array != nullptr) {
            Value *values = array->m_dataList;

            if (values->m_type != Value::ddl_unsigned_int32) {
                lprintf(LOG_ERROR, "wrong datatype for Index buffer");
                delete [] buffer;
                return 0;
            }

            while (values != nullptr) {
                buffer[i] = values->getUnsignedInt32();
                i++;
                values = values->getNext();
            }

            array = array->m_next;
        }

        numFaces = arraylen / 3; // should always be the same for all attributes
    }

    return buffer;
}
bool OGEX_Resource::load_GeometryObject(ODDLParser::DDLNode *node)
{

    using namespace ODDLParser;
    DDLNode *meshnode = node->getChildNodeList()[0];

    for (DDLNode *n : meshnode->getChildNodeList()) {
        if (strcmp(n->getType().c_str(), "VertexArray") == 0) {
            Property *prop = n->getProperties();

            while (prop != nullptr) {
                if (strcmp(prop->m_key->m_text.m_buffer, "attrib") == 0) {
                    if (prop->m_value->m_type == Value::ddl_string) {
                        const char *attrib = prop->m_value->getString();

                        if (strcmp(attrib, "position") == 0) {
                            verts = load_vertexbuffer(n);

                            if (verts == nullptr) {
                                return false;
                            }
                        } else if (strcmp(attrib, "normal") == 0) {
                            normal_vb = load_vertexbuffer(n);

                            if (normal_vb == nullptr) {
                                return false;
                            }
                        } else if (strcmp(attrib, "texcoord") == 0) {
                            uv_vb = load_vertexbuffer(n);

                            if (uv_vb == nullptr) {
                                return false;
                            }
                        } else if (strcmp(attrib, "tangent") == 0) {
                            tangent_vb = load_vertexbuffer(n);

                            if (tangent_vb == nullptr) {
                                return false;
                            }
                        } else if (strcmp(attrib, "binormalsign") == 0) {
                            binormal_vb = load_vertexbuffer(n);

                            if (binormal_vb == nullptr) {
                                return false;
                            }
                        } else if (strcmp(attrib, "color") == 0) {
                            color_vb = load_vertexbuffer(n);

                            if (color_vb == nullptr) {
                                return false;
                            }
                        }
                    }
                }

                prop = prop->m_next;
            }
        }

        if (strcmp(n->getType().c_str(), "IndexArray") == 0) {
            indices = load_indexbuffer(n);

            if (!indices) {
                return false;
            }
        }
    }

    return true;

}

int OGEX_Resource::load(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    lprintf(LOG_INFO, " opening file:%s", filename);

    if (!f) {
        lprintf(LOG_ERROR, "File not found:%s", filename);
        return 0;
    }

    size_t filesize = 0;
    fseek(f, 0, SEEK_END);
    filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (filesize == 0) {
        lprintf(LOG_ERROR, "Empty file:%s", filename);
        fclose(f);
        return 0;
    }

    char *buffer = new char[filesize + 1];

    if (buffer == nullptr) {
        lprintf(LOG_ERROR, "Out of memory while loading:%s", filename);
        fclose(f);
        return 0;
    }

    buffer[filesize] = 0;

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

        if (success) {
            DDLNode *root = ddlparser.getRoot();
            //printsubnodes(root, 0);
            DDLNode::DllNodeList children = root->getChildNodeList();

            for (size_t i = 0; i < children.size(); i++) {

                DDLNode *child = children[i];
                const char *type = child->getType().c_str();

                if (strcmp(type, "GeometryObject") == 0) {
                    success = load_GeometryObject(child);
                    break; // TODO: handle more than one GeometryObject
                }
            }
        }

    }

    delete[] buffer;

    if (!success) {
        lprintf(LOG_ERROR, "something went wrong loading: %s", filename);
    } else {
        //writeObj(filename);
        SetupGL();
    }

    return success;
}

void OGEX_Resource::SetupGL()
{

    #ifndef NOENGINE // for my test code without access to opengl or a c++11 compiler TODO: remove when done
    num_tris = numFaces;
    num_verts = numVerts;

    for (unsigned int i = 0; i < numVerts; i += 3) {
        updateBoundingBox(glm::vec3(
                              verts[i],
                              verts[i + 1],
                              verts[i + 2]
                          ));
    }

    if (indices) {
        glBindBuffer(GL_ARRAY_BUFFER, indices_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            num_tris * sizeof(unsigned int) * 3,
            indices,
            GL_STATIC_DRAW);
    }

    if (verts) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float) * 3,
            verts,
            GL_STATIC_DRAW);
    }

    if (normal_vb) {
        glBindBuffer(GL_ARRAY_BUFFER, normals_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float) * 3,
            normal_vb,
            GL_STATIC_DRAW);
        has_normals_buffer = true;
    }

    if (tangent_vb) {
        glBindBuffer(GL_ARRAY_BUFFER, tangent_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float) * 3,
            tangent_vb,
            GL_STATIC_DRAW);
        has_tangent_buffer = true;
    }

    if (binormal_vb) {
        glBindBuffer(GL_ARRAY_BUFFER, binormals_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float),
            binormal_vb,
            GL_STATIC_DRAW);
        has_binormals_buffer = true;
    }

    if (color_vb) {
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float) * 3,
            color_vb,
            GL_STATIC_DRAW);
        has_color_buffer = true;
    }

    if (uv_vb) {
        glBindBuffer(GL_ARRAY_BUFFER, uv_buffer);
        glBufferData(
            GL_ARRAY_BUFFER,
            numVerts * sizeof(float) * 2,
            uv_vb,
            GL_STATIC_DRAW);
        has_uv_buffer = true;
    }

    #endif
}

void OGEX_Resource::writeObj(const char *filename)
{
    char outputfilename[FILENAME_MAX] = {0};

    snprintf(outputfilename, FILENAME_MAX, "%s.obj", filename);
    FILE *f = fopen(outputfilename, "wt");

    if (!f) {
        return;
    }

    fprintf(f, "#numFaces:%u\n", numFaces);

    if (verts) {
        for (size_t i = 0; i < numVerts; i++) {
            size_t offset = i * 3;
            fprintf(f, "v %f %f %f\n", verts[offset], verts[offset + 1],
                    verts[offset + 2]);
        }
    }

    if (normal_vb) {
        for (size_t i = 0; i < numVerts; i++) {
            size_t offset = i * 3;
            fprintf(f, "vn %f %f %f\n", normal_vb[offset], normal_vb[offset + 1],
                    normal_vb[offset + 2]);
        }
    }

    if (uv_vb) {
        for (size_t i = 0; i < numVerts; i++) {
            size_t offset = i * 2;
            fprintf(f, "vt %f %f\n", uv_vb[offset], uv_vb[offset + 1]);
        }
    }

    for (size_t i = 0; i < numFaces; i++) {
        size_t offset = i * 3;

        fprintf(f, "f ");

        for (size_t j = 0; j < 3; j++) {

            char pospart[32]    = {0};
            char uvpart[32]     = {0};
            char normalpart[32] = {0};
            size_t index = offset + j;
            snprintf(pospart, 32, "%u", indices[index] + 1);
            snprintf(uvpart, 32, "/%u", indices[index] + 1);
            snprintf(normalpart, 32, "/%u", indices[index] + 1);

            if (!uv_vb && normal_vb) {
                snprintf(uvpart, 32, "/");
            }

            if (!normal_vb) {
                normalpart[0] = 0;
            }

            if (!uv_vb && !normal_vb) {
                uvpart[0] = 0;
                normalpart[0] = 0;
            }

            fprintf(f, "%s%s%s ", pospart, uvpart, normalpart);

        }

        fprintf(f, "\n");
    }

    fclose(f);
}
