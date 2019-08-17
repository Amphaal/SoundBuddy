#pragma once

#include <QTextStream>

class StringHelper {
    
    public:
        static QString boolToString(bool val) {
             return val ? "true" : "false";
        }

        static QString replaceFirstOccurrence(QString subjectCopy, const QString& toReplace, const QString& replaceWith) {   
            subjectCopy.replace(subjectCopy.indexOf(toReplace), toReplace.size(), replaceWith);
            return subjectCopy;
        }
};