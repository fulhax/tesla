#include "notify.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "errorhandler.hpp"

struct notify_directory {
    bool recursive;
    std::string dir_name;
    HANDLE handle;
};

Notify::Notify()
{

}
Notify::~Notify()
{
    for(auto watch : watchers) {
        CloseHandle(watch.first);
    }

    watchers.clear();

}

void Notify::watchDir(const char *dirname, bool recursive)
{
    HANDLE watch = CreateFile(dirname,
                              FILE_LIST_DIRECTORY,
                              FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_FLAG_BACKUP_SEMANTICS,
                              nullptr);

    if(watch != nullptr) {
        watchers[watch].dir_name = dirname;
        watchers[watch].recursive = recursive;
        watchers[watch].handle = FindFirstChangeNotification(dirname, true,
                                 FILE_NOTIFY_CHANGE_LAST_WRITE);
        lprintf(LOG_INFO, "Watching %s for filechanges", dirname);
    }
}

std::map<std::string, std::string> Notify::checkForChanges()
{
    std::map<std::string, std::string> output;

    for(auto watch : watchers) {
        HANDLE watchhandle = watch.first;
        notify_directory &dir = watchers[watchhandle];
        HANDLE waitHandle = watchers[watchhandle].handle;

        FILE_NOTIFY_INFORMATION strFileNotifyInfo[1024] = {0};
        DWORD dwBytesReturned = 0;

        DWORD wait = WaitForMultipleObjects(1, &waitHandle, false, 0);

        if(wait == WAIT_OBJECT_0) {
            printf("waitHandle:%p\n", waitHandle);

            //if(WaitForSingleObject(&waitHandle, 0))
            //{
            if(ReadDirectoryChangesW(watchhandle, (LPVOID)&strFileNotifyInfo,
                                     sizeof(strFileNotifyInfo),
                                     TRUE,
                                     FILE_NOTIFY_CHANGE_LAST_WRITE,
                                     &dwBytesReturned,
                                     nullptr,
                                     nullptr) != 0) {
                printf("stuff\n");

                if(strFileNotifyInfo[0].Action == FILE_ACTION_MODIFIED) {
                    printf("File Modified: %ls\n", strFileNotifyInfo[0].FileName);
                    char fullpath[FILENAME_MAX];
                    char filename[FILENAME_MAX];
                    wcstombs(filename, strFileNotifyInfo[0].FileName, FILENAME_MAX);
                    snprintf(fullpath, FILENAME_MAX, "%s/%s", dir.dir_name.c_str(), filename);
                    output[filename] = fullpath;
                }
            }

            printf("afterreaddirectorychanges\n");

            HANDLE newhandle = nullptr;

            if(FindNextChangeNotification(&newhandle) != 0) {
                watchers[watchhandle].handle = newhandle;
                printf("going by FindNextChangeNotification\n");
            } else {

                printf("before FindFirstChangeNotification\n");
                watchers[watchhandle].handle = FindFirstChangeNotification(
                                                   dir.dir_name.c_str(), true, FILE_NOTIFY_CHANGE_LAST_WRITE);
                printf("going by FindFirstChangeNotification\n");

            }

            CloseHandle(waitHandle);

            //printf("newHandle:  %p\nfindnextreturn:%i\n", newhandle, findnextreturncode);
        }
    }

    return output;
}
