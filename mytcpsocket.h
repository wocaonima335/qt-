#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include "Cpdu.h"
#include "dboperate.h"
#include <QTimer>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getStrName();
signals:
    void offline(MyTcpSocket *socket);
public slots:
    void recvMsg();
    void clinetOffline();
    void handledownloadFileData();
    QString showMsg();

private:
    QString m_strName = "test";
    TransFile *m_uploadFile;
    QFile *m_pDownloadFile;
    QTimer *m_pTimer;
    Cpdu *m_cpdu;
};
#endif