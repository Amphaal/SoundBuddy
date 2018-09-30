#ifdef _WIN32
    #include <windows.h>
#endif

#include <string>
#include <filesystem>
#include "../../libs/plistcpp/Plist.hpp"

using namespace std;

class PlatformHelper {
    public:
    #ifdef _WIN32
        static void openFileInOS(wchar_t* cpURL) {
            ShellExecute(NULL, L"open", cpURL, NULL, NULL, SW_SHOWNORMAL);
        }

        static string getITunesPrefFileProbableLocation() {
            return getenv("APPDATA") + string("\Apple Computer\Preferences\com.apple.iTunes.plist");
        }

        static string getProbableITunesLibraryLocation(std::ifstream *fileStream) {

        } 
    #endif
    
}