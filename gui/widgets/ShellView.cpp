#include "ShellView.h"
#include "../renderer/ScreenRenderer.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QTextOption>

ShellView::ShellView(QWidget *parent)
    : QWidget(parent)
{
    screenWidget = new ScreenWidget(&Buffer,this);

    connect(screenWidget,
            &ScreenWidget::bytesTyped, 
            this,
            &ShellView::bytesTyped);
    connect(screenWidget,
            &ScreenWidget::terminalResized,
            this,
            &ShellView::terminalResized);

    // Monospace font
    QFont font("JetBrains Mono", 12);
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    screenWidget->setFont(font);

    // Layout
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->addWidget(screenWidget);
}

void ShellView::insert(const QByteArray &data)
{
   renderer.render(data, Buffer);
   Buffer.setCursorVisible(true);
   screenWidget->update();
}