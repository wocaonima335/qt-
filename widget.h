#ifndef WIDGET_H
#define WIDGET_H
#include "textform.h"
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void loadConfig();

private:
    Ui::Widget *ui;
    QString m_strIP;
    qint16 m_usPort;
};
#endif // WIDGET_H
