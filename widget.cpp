#include "widget.h"
#include "./ui_widget.h"
#include "mytcpserver.h"
#include <QMessageBox>
Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadConfig()
{
    QFile file(":/server.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();
        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        MyTcpServer::getInstance().setStrRootPath(strList[2]);
        qDebug() << "ip : " << m_strIP << " port : " << m_usPort << " root path : " << MyTcpServer::getInstance().getStrRootPath();
    }
    else
    {
        QMessageBox::critical(this, "错误", "配置文件打开失败");
        exit(0);
    }
}
