#pragma once

#include <string>
#include <exception>

#include "src/localization/i18n.cpp"

///
/// Exceptions
///

class FTNZXMLLibFileUnreadableException : public std::exception {      
    private:
        std::string exceptionMessage;

    public:
        FTNZXMLLibFileUnreadableException() {
            this->exceptionMessage = I18n::tr()->FTNZXMLLibFileUnreadableException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

class FTNZMissingItunesConfigException : public std::exception {
    private:
        std::string exceptionMessage;
    
    public:
        FTNZMissingItunesConfigException() {
            this->exceptionMessage = I18n::tr()->FTNZMissingItunesConfigException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

class FTNZNoMusicFoundException : public std::exception {
    private:
        std::string exceptionMessage;
    
    public:
        FTNZNoMusicFoundException() {
            this->exceptionMessage = I18n::tr()->FTNZNoMusicFoundException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

///
/// End Exceptions
///