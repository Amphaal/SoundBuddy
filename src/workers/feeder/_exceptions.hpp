#pragma once

#include <string>
#include <exception>

#include "src/localization/i18n.hpp"

///
/// Exceptions
///

class FTNZXMLLibFileUnreadableException : public std::exception {      
    private:
        std::string exceptionMessage;

    public:
        FTNZXMLLibFileUnreadableException() {
            this->exceptionMessage = I18n::tr()->FTNZXMLLibFileUnreadableException().toStdString();
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
            this->exceptionMessage = I18n::tr()->FTNZMissingItunesConfigException().toStdString();
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
            this->exceptionMessage = I18n::tr()->FTNZNoMusicFoundException().toStdString();
        }
        const char * what () const throw () {
            return this->exceptionMessage.c_str();
        }
};

///
/// End Exceptions
///