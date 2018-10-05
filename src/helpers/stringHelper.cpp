#pragma once
#include <string>

using namespace std;

class StringHelper {
    public:
        static string splitPath(string path, string filter = "/") {
            std::size_t found = path.find_last_of(filter);
            if (found == string::npos) return path;
            path = path.substr(0, found);
            return path;
        }

        static bool has_only_digits(const string s) {
            return s.find_first_not_of( "0123456789" ) == string::npos;
        }
};