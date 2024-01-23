#ifndef TEXTFORM_H
#define TEXTFORM_H

#include <QWidget>
#include "mytcpserver.h"
namespace Ui
{
    class TextForm;
}

class TextForm : public QWidget
{
    Q_OBJECT

public:
    explicit TextForm(QWidget *parent = nullptr);
    ~TextForm();
    void appendText(QString text);
    void textChanged(QString text);
signals:
    void value(QString &text);

private:
    Ui::TextForm *ui;
    MyTcpSocket *mytcpsocket;
};

#endif // TEXTFORM_H
