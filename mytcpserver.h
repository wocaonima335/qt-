#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer> // 派生类实现客户端连接的监听和接收
#include <QList>      // 链表
#include "mytcpsocket.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT;

public:
    MyTcpServer();
    static MyTcpServer &getInstance();
    void incomingConnection(qintptr socketDescriptor);
    bool forwardMsg(const QString caDesName, PDU *pdu);

    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);

public slots:
    void deleteSocket(MyTcpSocket *MyTcpSocket);

private:
    QList<MyTcpSocket *> m_lstClientSockets;
    QString m_strRootPath;
};
#endif

