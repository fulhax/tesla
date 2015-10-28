#include "notify.hpp"

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "errorhandler.hpp"

Notify::Notify()
{

}
Notify::~Notify()
{
    for(auto watch : watchers)
    {
        CloseHandle(watch.first);
    }

    watchers.clear();

}

void Notify::watchDir(const char* dirname, bool recursive)
{
    HANDLE watch = CreateFile(dirname,
                              FILE_LIST_DIRECTORY,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                              nullptr);

    if(watch != nullptr)
    {
        watchers[watch].dir_name = dirname;
        watchers[watch].recursive = recursive;
        lprintf(LOG_INFO, "Watching %s for filechanges", dirname);
    }
}

std::map<std::string, std::string> Notify::checkForChanges()
{
    std::map<std::string, std::string> output;

    for(auto watch : watchers)
    {
        HANDLE watchhandle = watch.first;
        notify_directory& dir = watchers[watchhandle];

        FILE_NOTIFY_INFORMATION fni;
        OVERLAPPED over = {0};
        DWORD returnedbytes = 0;
        ReadDirectoryChangesW(watchhandle,
                              &fni, sizeof(fni),
                              dir.recursive,
                              FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
                              &returnedbytes,
                              &over,
                              nullptr);
        GetOverlappedResult(watchhandle, &over, &returnedbytes, false);

        if(fni.Action != 0)
        {
            char fullpath[FILENAME_MAX];
            char filename[FILENAME_MAX];
            wcstombs(filename, fni.FileName, FILENAME_MAX);
            snprintf(fullpath, FILENAME_MAX, "%s/%s", dir.dir_name.c_str(), filename);
            output[filename] = fullpath;
        }
    }

    return output;
}
