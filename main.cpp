#include "widget.h"
#include "textform.h"
#include <QApplication>
#include "dboperate.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DBoperate::getInstacne().init();
    Widget w;
    w.show();
    return a.exec();
}
