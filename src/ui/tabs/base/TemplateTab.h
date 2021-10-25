#pragma once

#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QScrollBar>
#include <QScrollArea>
#include <QWindow>
#include <QStyle>

#include "src/workers/base/ITNZThread.hpp"

#include "LogScrollView.hpp"


class TemplateTab : public QWidget {

    public:
        TemplateTab(QWidget *parent = 0);
        QPushButton* tButton = nullptr;
        void bindWithWorker(ITNZThread *bThread);

    protected:
        ITNZThread* bThread = nullptr;
        LogScrollView* lsv = nullptr;
        QScrollArea* scrollArea = nullptr;
        
        void scrollUpdate(int min, int max);

        void createNewLog();
        void printLog(const QString &message, const bool replacePreviousLine = false, const bool isError = false);
        void onThreadEnd();
        void onThreadStart();
};