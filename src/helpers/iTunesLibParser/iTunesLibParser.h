#pragma once

#include <QString>
#include <fstream>
#include <iostream>

class iTunesLibParser {
    
    public:
        iTunesLibParser(const QString &pathToFile);
        ~iTunesLibParser();
        QString ToJSON();

    private:
        QString _output;
        QString _pathToFile;
        std::ifstream _stream;
        
        int _curLevel;
        QString _curAttr;
        QString _curVal;
        bool _awaitingAttr = false;
        bool _attr_IsClosingAttr = false;
        bool _attr_IsNoPairAttr = false;
        bool _attr_IsEnclosingAttr = false;

        bool isIgnoredChar(char &toIgnore);
        bool isChevronStart(char &toCheck);
        bool isChevronEnd(char &toCheck);

        void processFile();
        void skipUntilInterestingData();
        void attrToJSON(const QString &attr);
        bool ifLastRemove(const QChar &c, QString &target);
        void removeVirgula();

        
        const char * C_NEXT_OBJ = ":";
        const char * C_OPEN_OBJ = "{";
        const char * C_CLOSE_OBJ = "}";
        const char * C_NEXT_ARRAY = ",";
        const char * C_OPEN_ARRAY = "[";
        const char * C_CLOSE_ARRAY = "]";
        const char * C_OPEN_STR = "\"";
        
};