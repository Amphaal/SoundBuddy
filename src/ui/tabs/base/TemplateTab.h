#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QScrollArea>
#include <QtGui/QWindow>
#include <QStyle>

#include "../../../workers/base/ITNZWorker.h"
#include "../../../helpers/stringHelper.cpp"

#include "LogScrollView.cpp"


class TemplateTab : public QWidget {

    public:
        TemplateTab(QWidget *parent);
        QPushButton *tButton = 0;
        void bindWithWorker(ITNZWorker *bThread);

    protected:
        ITNZWorker *bThread = 0;
        LogScrollView *lsv = 0;
        QScrollArea *scrollArea = 0;
        
        bool mustForceScrollPosition = false;

        void createNewLog();
        void printLog(const std::string &message, const bool replacePreviousLine = false, const bool isError = false);
        void onThreadEnd();
        void onThreadStart();
};