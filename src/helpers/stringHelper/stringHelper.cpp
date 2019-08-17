#pragma once

#include <string>
#include <sstream>
#include <set>

class StringHelper {
    public:
        static std::string splitPath(std::string path, std::string filter = "/") {
            std::size_t found = path.find_last_of(filter);
            if (found == std::string::npos) return path;
            path = path.substr(0, found);
            return path;
        }

        static bool has_only_digits(const std::string s) {
            return s.find_first_not_of( "0123456789" ) == std::string::npos;
        }

        static std::string ucwords(std::string input) {
            input[0] = toupper(input[0]);
            for (int i = 1; i < input.length(); i++)
            {   
                if (input[i - 1] == ' ') input[i] = toupper(input[i]);
                else tolower(input[i]);
            }
            return input;
        }

        static std::string join(std::set<std::string> vect, const std::string& separator = ", ") {

            std::stringstream ss;

            for(std::set<std::string>::iterator it = vect.begin(); it!=vect.end(); ++it)
            {
                if (std::next(it) != vect.end()) {
                    ss << *it << separator;
                } else {
                    ss << *it;
                }
            }

            return ss.str();
        }


        static std::string boolToString(bool val) {
             return val ? "true" : "false";
        }

        static std::string replaceFirstOccurrence(std::string& s, const std::string& toReplace, const std::string& replaceWith)
        {
            std::size_t pos = s.find(toReplace);
            if (pos == std::string::npos) return s;
            return s.replace(pos, toReplace.length(), replaceWith);
        }
};