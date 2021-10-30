// SoundBuddy
// Small companion app that feeds SoundVitrine with ITunes / Music library metadata
// Copyright (C) 2019-2021 Guillaume Vara <guillaume.vara@gmail.com>

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Any graphical or audio resources available within the source code may
// use a different license and copyright : please refer to their metadata
// for further details. Resources without explicit references to a
// different license and copyright still refer to this GPL.

#include "TemplateTab.h"

TemplateTab::TemplateTab(QWidget* parent) : QWidget(parent),
    tButton(new QPushButton(this)),
    scrollArea(new QScrollArea(this)) {
        //
        QObject::connect(
            this->scrollArea->verticalScrollBar(), &QScrollBar::rangeChanged,
            this, &TemplateTab::scrollUpdate
        );

        this->setLayout(new QBoxLayout(QBoxLayout::TopToBottom, this));
        this->scrollArea->setWidgetResizable(true);
}

void TemplateTab::createNewLog() {
    if(!this->lsv) delete this->lsv;
    this->lsv = new LogScrollView(nullptr);
    this->scrollArea->setWidget(this->lsv);
}

void TemplateTab::printLog(const QString &message, const bool replacePreviousLine, const bool isError) {
    //
    if(replacePreviousLine) {
        this->lsv->updateLatestMessage(message);
    } else {
        this->lsv->addMessage(message, isError);
    }
}

void TemplateTab::scrollUpdate(int min, int max) {
    // to perform heavy CPU consuming action
    auto tabScrollBar = this->scrollArea->verticalScrollBar();
    tabScrollBar->setValue(tabScrollBar->maximum());
}

void TemplateTab::onThreadEnd() {
    this->tButton->setEnabled(true);
}

void TemplateTab::onThreadStart() {
    this->tButton->setEnabled(false);
    this->createNewLog();
}

void TemplateTab::bindWithWorker(ITNZThread *bThread) {
    QObject::connect(
        bThread, &ITNZThread::printLog,
        this, &TemplateTab::printLog
    );

    QObject::connect(
        bThread, &QThread::started,
        this, &TemplateTab::onThreadStart
    );

    QObject::connect(
        bThread, &QThread::finished,
        this, &TemplateTab::onThreadEnd
    );
}
