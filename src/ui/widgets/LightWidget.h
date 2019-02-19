#pragma once

#include <QtWidgets>

class LightWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
    
    public:
        LightWidget(const QColor &color, QWidget *parent = 0);
        bool isOn() const;
        void setOn(bool on);

    public slots:
        void turnOff();
        void turnOn();

    protected:
        void paintEvent(QPaintEvent *) override;

    private:
        QColor m_color;
        bool m_on;
};
