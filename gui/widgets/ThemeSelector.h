#ifndef THEMESELECTOR_H
#define THEMESELECTOR_H

#include <QWidget>

class QPushButton;
class QMenu;
class QString;

class ThemeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ThemeSelector(QWidget *parent = nullptr);

private slots:
    void setTheme(const QString &themeName);

private:
    QPushButton *button;
    QMenu *menu;
    QString currentTheme;
    void initUI();
};

#endif
