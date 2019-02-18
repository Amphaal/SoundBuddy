#include <string>
#include <fstream>
#include <iostream>

class iTunesLibParser {
    
    public:
        iTunesLibParser(std::string pathToFile);
        ~iTunesLibParser();
        std::string ToJSON();

    private:
        std::string _output;
        std::string _pathToFile;
        std::ifstream _stream;
        
        int _curLevel;
        std::string _curAttr;
        std::string _curVal;
        bool _awaitingAttr = false;
        bool _attr_IsClosingAttr = false;
        bool _attr_IsNoPairAttr = false;
        bool _attr_IsEnclosingAttr = false;

        bool isIgnoredChar(char &toIgnore);
        bool isChevronStart(char &toCheck);
        bool isChevronEnd(char &toCheck);

        void processFile();
        void skipUntilInterestingData();
        void attrToJSON(std::string &attr);
        bool ifLastRemove(const char * c, std::string & target);
        void removeVirgula();

        
        const char * C_NEXT_OBJ = ":";
        const char * C_OPEN_OBJ = "{";
        const char * C_CLOSE_OBJ = "}";
        const char * C_NEXT_ARRAY = ",";
        const char * C_OPEN_ARRAY = "[";
        const char * C_CLOSE_ARRAY = "]";
        const char * C_OPEN_STR = "\"";
        
};