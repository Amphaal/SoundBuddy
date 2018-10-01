#include <windows.h>
#include  <stdlib.h>  
#include <string>
#include "../../libs/plistcpp/Plist.hpp"

using namespace std;

class PlatformHelper {
    
    static void openFileInOS(wchar_t* cpURL) {
        ShellExecute(NULL, L"open", cpURL, NULL, NULL, SW_SHOWNORMAL);
    }

    static string getITunesPrefFileProbableLocation() {
        return PlatformHelper::getenv("APPDATA") + string("\\Apple Computer\\Preferences\\com.apple.iTunes.plist");
    }

    static string getenv(char* variable) {
        char* buf = nullptr;
        size_t sz = 0;
        if (_dupenv_s(&buf, &sz, "EnvVarName") == 0 && buf != nullptr)
        {
            string ret = variable;
            free(buf);
            return ret;
        }
    }

    static string getProbableITunesLibraryLocation(std::ifstream *fileStream) {

    } 
}