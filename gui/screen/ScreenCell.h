#pragma once

#include <QChar>
#include <QColor>
#include <QFont>

struct ScreenCell
{
    QChar ch{' '};

    QColor fg{Qt::white};
    QColor bg{Qt::black};

    bool bold{false};
};