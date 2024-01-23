#include "mytcpserver.h"

MyTcpServer::MyTcpServer()
{
}

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket();
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_lstClientSockets.append(pTcpSocket);

    connect(pTcpSocket, SIGNAL(offline(MyTcpSocket *)), this, SLOT(deleteSocket(MyTcpSocket *)));
}

bool MyTcpServer::forwardMsg(const QString caDesName, PDU *pdu)
{
    if (caDesName == nullptr || pdu == nullptr)
    {
        return false;
    }
    for (int i = 0; i < m_lstClientSockets.size(); i++)
    {
        if (m_lstClientSockets.at(i)->getStrName() == caDesName)
        {
            m_lstClientSockets.at(i)->write((char *)pdu, pdu->uiPDULen); // 转发消息
            return true;
        }
    }
    return false;
}

QString MyTcpServer::getStrRootPath() const
{
    return m_strRootPath;
}

void MyTcpServer::setStrRootPath(const QString &strRootPath)
{
    m_strRootPath = strRootPath;
}
void MyTcpServer::deleteSocket(MyTcpSocket *pTcpSocket)
{
    QList<MyTcpSocket *>::iterator it = m_lstClientSockets.begin();
    for (; it != m_lstClientSockets.end(); ++it)
    {
        if (*it == pTcpSocket)
        {
            (*it)->deleteLater();
            *it = nullptr;
            m_lstClientSockets.erase(it);
            break;
        }
    }
    qDebug() << "--------------";
}