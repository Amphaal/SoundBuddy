#ifdef _WIN32

    #include <windows.h>
    #include <stdlib.h>  

    #include "platformHelper.h"

    void PlatformHelper::openFileInOS(std::string cpURL) {
        ShellExecuteA(NULL, "open", "notepad", cpURL.c_str(), NULL, SW_SHOWNORMAL);
    };

    void PlatformHelper::openUrlInBrowser(std::string cpURL) {
        ShellExecuteA(NULL, "open", cpURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
    };

    std::string PlatformHelper::getenv(char* variable) {
        char* buf = nullptr;
        size_t sz = 0;
        if (_dupenv_s(&buf, &sz, variable) == 0 && buf != nullptr)
        {
            std::string ret = buf;
            free(buf);
            return ret;
        } else {
            return NULL;
        }
    };
    
    std::string PlatformHelper::getITunesPrefFileProbableLocation() {
        return PlatformHelper::getenv("APPDATA") + std::string("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
    };

#endif