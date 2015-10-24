#include "glsl.hpp"

struct shader_file_extension {
    const char *ext;
    uint32_t type;
};

struct shader_file_extension extensions[] = {
    {".vs", GL_VERTEX_SHADER},
    {".vert", GL_VERTEX_SHADER},
    {".gs", GL_GEOMETRY_SHADER},
    {".geom", GL_GEOMETRY_SHADER},
    {".tcs", GL_TESS_CONTROL_SHADER},
    {".tes", GL_TESS_EVALUATION_SHADER},
    {".fs", GL_FRAGMENT_SHADER},
    {".frag", GL_FRAGMENT_SHADER}
};

GLSL_Resource::GLSL_Resource()
{
}

GLSL_Resource::~GLSL_Resource()
{
}

int GLSL_Resource::load(const char *filename)
{
    int num_ext = sizeof(extensions) / sizeof(shader_file_extension);
    const char *ext = strrchr(filename, '.');

    if(!ext) {
        return 0;
    }

    uint32_t type = 0;

    for(int i = 0; i < num_ext; i++) {
        if(strcmp(ext, extensions[i].ext) == 0) {
            type = extensions[i].type;
            break;
        }
    }

    if(type == 0) {
        lprintf(LOG_ERROR, "Unrecognized extension ^g%s^0", ext);
        return 0;
    }

    FILE *file = fopen(filename, "rb");

    if(file == NULL) {
        lprintf(LOG_WARNING, "Unable to open ^g\"%s\"^0", filename);
        return 0;
    }

    fseek(file, 0, SEEK_END);
    int64_t size = ftell(file);
    char *code = new char[size + 1];
    fseek(file, 0, SEEK_SET);
    fread(code, size, 1, file);
    code[size] = 0;

    fclose(file);

    handle = glCreateShader(type);
    glShaderSource(handle, 1, const_cast<const char **>(&code), NULL);
    delete [] code;

    glCompileShader(handle);

    int result;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &result);

    if(result == GL_FALSE) {
        int length = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &length);

        if(length > 0) {
            char *log = new char[length];
            int written = 0;
            glGetShaderInfoLog(handle, length, &written, log);

            lprintf(
                LOG_ERROR,
                "^g\"%s\"^0, shader compilation failed: %s",
                filename, log);

            delete [] log;

            return 0;
        }
    }

    lprintf(LOG_INFO, "GLSL loaded ^g\"%s\"^0", filename);

    return 1;
}
