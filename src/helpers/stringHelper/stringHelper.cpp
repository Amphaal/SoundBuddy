#pragma once

#include <string>
#include <sstream>
#include <set>

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

        static string ucwords(string input) {
            input[0] = toupper(input[0]);
            for (int i = 1; i < input.length(); i++)
            {   
                if (input[i - 1] == ' ') input[i] = toupper(input[i]);
                else tolower(input[i]);
            }
            return input;
        }

        static std::string join(std::set<string> vect, const std::string& separator = ", ") {

            std::stringstream ss;

            for(set<string>::iterator it=vect.begin(); it!=vect.end(); ++it)
            {
                if (std::next(it) != vect.end()) {
                    ss << *it << separator;
                } else {
                    ss << *it;
                }
            }

            return ss.str();
        }


        static string boolToString(bool val) {
             return val ? "true" : "false";
        }

        static std::string replaceFirstOccurrence(std::string& s, const std::string& toReplace, const std::string& replaceWith)
        {
            std::size_t pos = s.find(toReplace);
            if (pos == std::string::npos) return s;
            return s.replace(pos, toReplace.length(), replaceWith);
        }
};