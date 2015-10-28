#ifndef NOTIFY_HPP_
#define NOTIFY_HPP_

#include <map>
#include <string>

#ifdef _WIN32
struct notify_directory
{
    bool recursive;
    std::string dir_name;
};
#endif

class Notify
{
public:
    Notify();
    virtual ~Notify();

    void watchDir(const char *dirname, bool recursive = true);
    std::map<std::string, std::string> checkForChanges();
private:
    #ifndef _WIN32
    std::map<int, std::string> watchers;

    int inotify;
    #else
    std::map<void *, notify_directory> watchers;
    #endif
};

#endif // NOTIFY_HPP_
