#include "config.hpp"

#include <stdio.h>
#include <string.h>

#include <string>

#include "errorhandler.hpp"

#define MAX_LINE 300

void Config::readConfig(const char *filename)
{
    FILE *f = fopen(filename, "rb");

    if(f) {
        lprintf(LOG_INFO, "Reading config from ^g\"%s\"^0", filename);

        while(!feof(f)) {
            char line[MAX_LINE] = {0};
            std::string key;
            std::string val;
            std::string *current = &key;

            fgets(line, MAX_LINE, f);

            char *tmp = line;

            while(*tmp != '\0' && *tmp != '\n' && *tmp != '\r') {
                if(*tmp == '=') {
                    current = &val;
                } else if(*tmp == '#') {
                    break;
                } else if(*tmp != ' ') {
                    current->append(tmp, 1);
                }

                tmp++;
            }

            if(key.length() > 0 && val.length() > 0) {
                config[key] = val;
            }
        }

        fclose(f);
    } else {
        lprintf(LOG_ERROR, "Unable to read config file ^g\"%s\"^0", filename);
    }
}

void Config::saveConfig(const char *filename)
{
    FILE *o = fopen(filename, "wb");
    char segment[256] = {0};

    fprintf(o, "###################################################\n");
    fprintf(o, "# Autogenerated config file for Tesla Game Engine #\n");
    fprintf(o, "###################################################\n");

    for(auto c : config) {
        char *pch = strrchr(const_cast<char *>(c.first.c_str()), '.');

        if(pch) {
            char new_segment[256] = {0};

            strncpy(
                new_segment,
                c.first.c_str(),
                strlen(c.first.c_str()) - strlen(pch));

            if(strcmp(new_segment, segment) != 0) {
                snprintf(segment, sizeof(segment), "%s", new_segment);

                fprintf(o, "\n# %s\n", segment);
            }
        } else {
            if(strlen(segment) > 0) {
                fprintf(o, "\n# uncategorized\n");
                segment[0] = '\0';
            }
        }

        fprintf(o, "%s = %s\n", c.first.c_str(), c.second.c_str());
    }

    fclose(o);
}

std::string Config::getString(const char *key, std::string def)
{
    auto conf = config.find(key);

    if(conf == config.end()) {
        config[key] = def;
    }

    return config[key];
}

int Config::getInt(const char *key, int def)
{
    int ret = atoi(getString(key, std::to_string(def)).c_str());
    return ret;
}

bool Config::getBool(const char *key, bool def)
{
    std::string ret = getString(key, (def) ? "true" : "false");

    if(ret == "1" || ret == "yes" || ret == "true") {
        return true;
    }

    return false;
}
