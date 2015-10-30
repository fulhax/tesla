#include "notify.hpp"

#include <string.h>
#include <sys/inotify.h>
#include <poll.h>
#include <dirent.h>
#include <unistd.h>

#include <map>
#include <string>

#include "../errorhandler.hpp"

#define EVENT_SIZE (sizeof(inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))

Notify::Notify()
{
    inotify = inotify_init1(IN_NONBLOCK);

    if(inotify < 0) {
        lprintf(LOG_WARNING, "Failed to start inotify!");
    }
}

Notify::~Notify()
{
    for(auto watch : watchers) {
        inotify_rm_watch(inotify, watch.first);
    }

    watchers.clear();
    close(inotify);
}

void Notify::watchDir(const char *dirname, bool recursive)
{
    DIR *dh;
    dirent *entry;
    char fullpath[FILENAME_MAX];

    int watch = inotify_add_watch(
                    inotify,
                    dirname,
                    IN_CLOSE_WRITE | IN_MOVED_TO);

    watchers[watch] = dirname;

    lprintf(LOG_INFO, "Watching %s for filechanges", dirname);

    if((dh = opendir(dirname)) == NULL) {
        lprintf(LOG_ERROR, "Could not open directory ^g\"%s\"^0", dirname);
        return;
    }

    while((entry = readdir(dh)) != NULL) {
        if(strncmp(entry->d_name, "..", 2) != 0 &&
            strncmp(entry->d_name, ".", 1) != 0) {

            if(entry->d_type == DT_DIR) {
                snprintf(
                    fullpath,
                    FILENAME_MAX,
                    "%s/%s",
                    dirname,
                    entry->d_name);

                if(recursive) {
                    watchDir(fullpath);
                }
            }
        }
    }
}

std::map<std::string, std::string> Notify::checkForChanges()
{
    std::map<std::string, std::string> output;
    int length = 0;
    int i = 0;
    char buffer[BUF_LEN] = {0};

    do {
        length = read(inotify, buffer, BUF_LEN);

        if(length < 0) {
            break;
        }

        inotify_event *event = reinterpret_cast<inotify_event *>(&buffer[i]);

        if(event->mask & IN_CLOSE_WRITE || event->mask & IN_MOVED_TO) {
            char fullpath[FILENAME_MAX];
            auto watch = watchers[event->wd];

            snprintf(
                fullpath,
                FILENAME_MAX,
                "%s/%s",
                watch.c_str(),
                event->name);

            output[event->name] = fullpath;
        }

        i += EVENT_SIZE + event->len;
    } while(i < length);

    return output;
}
