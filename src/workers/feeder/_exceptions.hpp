#pragma once

#include <string>
#include <exception>

#include "src/localization/i18n.hpp"

///
/// Exceptions
///

class FTNZXMLLibFileUnreadableException : public std::exception {      
    private:
        QString exceptionMessage;

    public:
        FTNZXMLLibFileUnreadableException() {
            this->exceptionMessage = I18n::tr()->FTNZXMLLibFileUnreadableException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.toUtf8();
        }
};

class FTNZMissingItunesConfigException : public std::exception {
    private:
        QString exceptionMessage;
    
    public:
        FTNZMissingItunesConfigException() {
            this->exceptionMessage = I18n::tr()->FTNZMissingItunesConfigException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.toUtf8();
        }
};

class FTNZNoMusicFoundException : public std::exception {
    private:
        QString exceptionMessage;
    
    public:
        FTNZNoMusicFoundException() {
            this->exceptionMessage = I18n::tr()->FTNZNoMusicFoundException();
        }
        const char * what () const throw () {
            return this->exceptionMessage.toUtf8();
        }
};

///
/// End Exceptions
///