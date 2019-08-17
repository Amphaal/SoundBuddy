#pragma once

#include <QLocale>

#include "i18n/Ii18n.h"
#include "i18n/fr_FR.cpp"
#include "i18n/en_EN.cpp"

class I18n {
     
     public:
        static IAppTranslator* tr() {   
            if (!m_pInstance) {
                m_pInstance = setTranslator();
            }
            return m_pInstance;
        }

        static QString getLocaleName() {
            return QLocale::system().name();
        }

    private:
        I18n(){};  // Private so that it can  not be called
        I18n(I18n const&){};             // copy constructor is private
        //I18n& operator=(I18n const&){};  // assignment operator is private

        static inline IAppTranslator* m_pInstance = nullptr;

        static IAppTranslator* setTranslator() {
            auto lName = I18n::getLocaleName();

            if (lName.mid(0,2) == "fr") {
                return new AppTranslator_FR();
            } else {
                return new AppTranslator_EN();
            }
        } 
};