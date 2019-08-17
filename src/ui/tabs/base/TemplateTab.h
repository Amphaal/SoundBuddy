#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QScrollArea>
#include <QWindow>
#include <QStyle>

#include "src/workers/base/ITNZThread.h"
#include "src/helpers/stringHelper/stringHelper.hpp"

#include "LogScrollView.hpp"


class TemplateTab : public QWidget {

    public:
        TemplateTab(QWidget *parent = 0);
        QPushButton *tButton = 0;
        void bindWithWorker(ITNZThread *bThread);

    protected:
        ITNZThread *bThread = 0;
        LogScrollView *lsv = 0;
        QScrollArea *scrollArea = 0;
        
        void scrollUpdate(int min, int max);

        void createNewLog();
        void printLog(const QString &message, const bool replacePreviousLine = false, const bool isError = false);
        void onThreadEnd();
        void onThreadStart();
};