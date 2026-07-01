#pragma once

#include <QWidget>
#include <QTimer>

class ScreenBuffer;

class ScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenWidget(ScreenBuffer *buffer,
                          QWidget *parent = nullptr);


signals:
    void bytesTyped(const QByteArray &data);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;


private:
    ScreenBuffer *buffer_;
    QTimer blinkTimer;
    static constexpr int LEFT_MARGIN = 8;
    static constexpr int TOP_MARGIN = 8;
};