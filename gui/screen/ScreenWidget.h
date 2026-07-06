#pragma once

#include <QWidget>
#include <QTimer>
#include <QFont>
#include <QPoint>

class ScreenBuffer;

class ScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenWidget(ScreenBuffer *buffer,
                          QWidget *parent = nullptr);


signals:
    void bytesTyped(const QByteArray &data);
    void terminalResized(int rows,int cols);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


private:
    void updateFonts();

    void copySelection();

    bool selecting = false;
    QPoint selectionStart; 
    QPoint selectionEnd;

    QPoint cellFromPosition(const QPointF &pos) const;
    bool isCellSelected(int row, int col) const;

    ScreenBuffer *buffer_;

    QTimer blinkTimer;
    
    QFont normalFont_;
    QFont boldFont_;
    
    static constexpr int LEFT_MARGIN = 5;
    static constexpr int TOP_MARGIN = 5;
};