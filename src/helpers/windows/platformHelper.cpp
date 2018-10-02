#include <windows.h>
#include <stdlib.h>  
#include "../../libs/plistcpp/Plist.hpp"

class PlatformHelper {
    
    public:
        static void openFileInOS(std::string cpURL) {
            ShellExecuteA(NULL, "open", "notepad", cpURL.c_str(), NULL, SW_SHOWNORMAL);
        }

        static void openUrlInBrowser(std::string cpURL) {
            ShellExecuteA(NULL, "open", cpURL.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }

        static std::string getITunesPrefFileProbableLocation() {
            return PlatformHelper::getenv("APPDATA") + std::string("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
        }

        static std::string getenv(char* variable) {
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
        }

        static std::string getProbableITunesLibraryLocation(std::ifstream *fileStream) {

        } 
};