#pragma once

#include <QtWidgets>

enum class TLW_Colors {
    RED = 0,
    YELLOW = 1,
    GREEN = 2
};

Q_DECLARE_METATYPE(TLW_Colors)

class LightWidget : public QWidget {
    Q_OBJECT
    Q_PROPERTY(bool on READ isOn WRITE setOn)
    
    public:
        LightWidget(const QColor &color, QWidget* parent = nullptr);
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
