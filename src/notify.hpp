#ifndef NOTIFY_HPP_
#define NOTIFY_HPP_

#include <map>
#include <string>

class Notify
{
public:
    Notify();
    virtual ~Notify();

    void watchDir(const char *dirname, bool recursive = true);
    std::map<std::string, std::string> checkForChanges();
private:
    std::map<int, std::string> watchers;

    int inotify;
};

#endif // NOTIFY_HPP_
