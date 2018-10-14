#pragma once

#include <QLocale>
#include <string>

#include "i18n/Ii18n.h"
#include "i18n/fr_FR.cpp"
#include "i18n/en_EN.cpp"

class I18n {
     public:
        static IFeedTNZTranslator* tr() {   
            if (!m_pInstance) {
                m_pInstance = setTranslator();
            }
            return m_pInstance;
        }

    private:

        I18n(){};  // Private so that it can  not be called
        I18n(I18n const&){};             // copy constructor is private
        I18n& operator=(I18n const&){};  // assignment operator is private

        static inline IFeedTNZTranslator* m_pInstance = nullptr;

        static IFeedTNZTranslator* setTranslator() {
            auto lName = QLocale::system().name().toStdString();

            if (lName == "fr_FR") {
                return new FeedTNZTranslator_FR();
            } else {
                return new FeedTNZTranslator_EN();
            }
        } 
};