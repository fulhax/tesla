#ifndef CONFIG_HPP_
#define CONFIG_HPP_

#include <map>
#include <string>

class Config
{
    std::map<std::string, std::string> config;

public:
    Config() {}
    virtual ~Config() {}

    void readConfig(const char *filename);
    void saveConfig(const char *filename) const;

    std::string getString(const char *key, std::string def);
    int getInt(const char *key, int def);
    bool getBool(const char *key, bool def);
};

#endif // CONFIG_HPP_
