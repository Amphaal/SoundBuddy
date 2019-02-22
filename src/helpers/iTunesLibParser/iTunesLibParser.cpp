#include "iTunesLibParser.h"

iTunesLibParser::iTunesLibParser(std::string pathToFile) : _pathToFile(pathToFile), _stream(pathToFile) {}
iTunesLibParser::~iTunesLibParser() {
    _stream.close();
};

std::string iTunesLibParser::ToJSON() {
    skipUntilInterestingData();
    processFile();
    return _output;
};

void iTunesLibParser::processFile() {
    char c;
    do {
        c = _stream.get();
        
        if(isChevronStart(c)) {

            _awaitingAttr = true;

        } else if (isChevronEnd(c)) {
            
            if (_attr_IsClosingAttr) _curVal = "";
            _attr_IsNoPairAttr = ifLastRemove("/", _curAttr);
            attrToJSON(_curAttr);
            _curAttr = "";
            _awaitingAttr = false;

        } else {
            
           if(_awaitingAttr) { 

               if(!_curAttr.size() && c == *"/") {
                   _attr_IsClosingAttr = true;
                } else {
                    _curAttr += c; 
                } 

           }
           else if (!isIgnoredChar(c)){ 
               if(c == *C_OPEN_STR || c == '\\') {
                   _output += "\\";
                }
               _output += c;
               _curVal += c; 
           }
        }
    }
    while (_stream.good());

    removeVirgula();
};

void iTunesLibParser::removeVirgula() {
    ifLastRemove(",", _output);
}

bool iTunesLibParser::ifLastRemove(const char * c, std::string & target) {
    if(target.back() == *c) {
        target.pop_back();
        return true;
    }
    return false;
}

void iTunesLibParser::attrToJSON(std::string &attr) {
    
    auto c_next = C_NEXT_ARRAY;
    auto c_open = C_OPEN_STR;
    auto c_close = C_OPEN_STR;

    //define collating values
    if (attr == "dict" || attr == "array") {
        
        _attr_IsEnclosingAttr = true;
        
        if (attr == "array") {
            c_open = C_OPEN_ARRAY;
            c_close = C_CLOSE_ARRAY;
        } else {
            c_open = C_OPEN_OBJ;
            c_close = C_CLOSE_OBJ;
        }

    } else if (attr == "key") {

        c_next = C_NEXT_OBJ;

    } else if (attr == "integer" || attr == "real") {

        c_open = "";
        c_close = "";

    } else if (attr.find("plist") == 0) {
        c_open = "";
        c_close = "";
        c_next = "";
    }

    //apply parsing
    if(_attr_IsNoPairAttr) {
        if(_attr_IsEnclosingAttr) _output += (std::string)c_open + c_close + c_next;
        else _output += attr + c_next;
    } else if (_attr_IsEnclosingAttr) {
        if (_attr_IsClosingAttr) { 
            removeVirgula();
            _output += c_close;
            _output += c_next;
            _curLevel--;
        } else {
            _output += c_open;
             _curLevel++;
        }
    } else if (_attr_IsClosingAttr) {
        _output += c_close;
        _output += c_next;
    } else {
        _output += c_open;
    }
    
    //reset tags
    _attr_IsNoPairAttr = false;
    _attr_IsEnclosingAttr = false;
    _attr_IsClosingAttr = false;
};

/////////////
// Helpers //
/////////////

void iTunesLibParser::skipUntilInterestingData() {
    int skip = 0;
    char c;

    do {
        c = _stream.get();
        if(c == 10) {
            skip++;
        }
    }
    while (_stream.good() && skip < 2);
};

bool iTunesLibParser::isIgnoredChar(char &toIgnore) {
    return toIgnore == 9 || toIgnore == 10 || toIgnore == -1;
};

bool iTunesLibParser::isChevronStart(char &toCheck) {
    return toCheck == 60;
};

bool iTunesLibParser::isChevronEnd(char &toCheck) {
    return toCheck == 62;
};