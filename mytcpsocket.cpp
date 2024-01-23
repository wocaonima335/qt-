#include "mytcpsocket.h"
#include <QDebug>
#include <QDir> // 操作文件夹的库
#include <QDateTime>
#include <qtimer.h>
#include <QByteArray>
#include "textform.h"
MyTcpSocket::MyTcpSocket()
{
    m_uploadFile = new TransFile;
    m_uploadFile->bTransForm = false;
    m_pDownloadFile = new QFile;
    m_pTimer = new QTimer;
    m_cpdu = new Cpdu();

    connect(this, SIGNAL(readyRead()), this, SLOT(recvMsg()));
    connect(this, SIGNAL(disconnected()), this, SLOT(clinetOffline()));
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(handledownloadFileData()));
    connect(this, SIGNAL(readyRead()), this, SLOT(showMsg()));
}

static PDU *temp;
QString MyTcpSocket::getStrName()
{
    return m_strName;
}
void MyTcpSocket::clinetOffline()
{
    DBoperate::getInstacne().handleoffline(m_strName.toStdString().c_str());
    emit offline(this); // 发送给Tcpserver该socket删除信号
}
void MyTcpSocket::handledownloadFileData()
{
    m_pTimer->stop();
    char *pBuffer = new char[4096];
    qint64 iActualSize = 0;
    while (true)
    {
        iActualSize = m_pDownloadFile->read(pBuffer, 4096);
        if (iActualSize > 0 && iActualSize <= 4096)
        {
            this->write(pBuffer, iActualSize);
        }
        else if (iActualSize == 0)
        {
            break;
        }
        else
        {
            qDebug() << "发送文件数据给客户端出错";
            break;
        }
    }
    m_pDownloadFile->close();
    delete[] pBuffer;
    pBuffer = nullptr;
    m_pDownloadFile->setFileName("");
}
QString MyTcpSocket::showMsg()
{
    qDebug() << "接受信息类型";
    if (m_uploadFile->bTransForm)
    {
        return getStrName() + " UpLoadFile Request";
    }
    else
    {
        qDebug() << this->bytesAvailable();
        uint uiPDULen = 0;
        PDU *pdu = temp;
        TextForm textform;
        switch (pdu->uiMsgType)
        {
        case ENUM_MSG_TYPE_REGIST_REQUEST:
        {
            textform.appendText(getStrName() + " Regist Request");
            return getStrName() + " Regist Request";
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
        {
            textform.appendText(getStrName() + " Login Request");
            return getStrName() + " Login Request";
            break;
        }
        case ENUM_MSG_TYPE_ONLINE_USERS_REQUEST:
        {
            return getStrName() + " Seach onlineUser Request";
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USERS_REQUEST:
        {
            return getStrName() + " Seach User Request";

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        {
            return getStrName() + " Add Friend Request";
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
        {
            return getStrName() + " Add Friend Respone";
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REJECT:
        {
            return getStrName() + " Reject Friend Respone";
            break;
        }
        case ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST:
        {
            return getStrName() + " FLUSH Friend Request";
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        {
            return getStrName() + " Private Chat Request";
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: // 群聊请求
        {
            return getStrName() + " Group Chat Request";
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: // 创建文件夹请求
        {
            return getStrName() + " Create Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST: // 刷新文件夹请求
        {
            return getStrName() + " Flush Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST: // 删除文件请求
        {
            return getStrName() + " Delete Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_REQUEST: // 重命名文件请求
        {
            return getStrName() + " Rname Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_ENTRY_DIR_REQUEST: // 进入文件夹请求
        {
            return getStrName() + " Entry Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_PRE_DIR_REQUEST: // 上一目录请求
        {
            return getStrName() + " Pre Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: // 上传文件请求
        {
            return getStrName() + " Upload Dir Request";
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST: // 下载文件请求
        {
            return getStrName() + " Download file Request";
            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_REQUEST: // 移动文件请求
        {
            return getStrName() + " Move file Request";
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST: // 分享文件请求
        {
            return getStrName() + " Share file Request";
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 分享文件通知响应处理
        {
            return getStrName() + " Share file Reponse";
            break;
        }
        default:
            break;
        }
        if (NULL != pdu)
        {
            // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;

            // 释放空间
            free(pdu);
            pdu = nullptr;
        }
        // 释放空间
        free(pdu);
        pdu = NULL;
    }
}
void MyTcpSocket::recvMsg()
{
    qDebug() << "接受信息";
    if (m_uploadFile->bTransForm)
    {
        QByteArray baBuffer = this->readAll();
        m_uploadFile->file.write(baBuffer);
        m_uploadFile->iReceivedSize += baBuffer.size();
        PDU *respdu = nullptr;
        qDebug() << "上传文件中：" << m_uploadFile->iReceivedSize;
        if (m_uploadFile->iReceivedSize == m_uploadFile->iTotalSize)
        {
            m_uploadFile->file.close();
            m_uploadFile->bTransForm = false;
            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(respdu->caData, UPLOAD_FILE_OK, 32);
        }
        else if (m_uploadFile->iReceivedSize > m_uploadFile->iTotalSize)
        {
            m_uploadFile->file.close();
            m_uploadFile->bTransForm = false;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strncpy(respdu->caData, UPLOAD_FILE_FAILED, 32);
        }

        if (respdu != nullptr)
        {
            this->write((char *)respdu, respdu->uiPDULen);
            free(respdu);
            respdu = nullptr;
        }
        return;
    }
    qDebug() << this->bytesAvailable();
    uint uiPDULen = 0;

    this->read((char *)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    this->read((char *)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    temp = pdu;
    PDU *respdu = nullptr;
    switch (pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        respdu = m_cpdu->hanndleRegistRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        respdu = m_cpdu->handleLoginRequest(pdu, m_strName);
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USERS_REQUEST:
    {
        respdu = m_cpdu->handleOnlineUserRequest();
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USERS_REQUEST:
    {
        respdu = m_cpdu->handleSearchUserRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        respdu = m_cpdu->handleAddFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
    {
        m_cpdu->handleAddFriendAgreeRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT:
    {
        m_cpdu->handleAddFriendRejectRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST:
    {
        respdu = m_cpdu->handleFlushFriendRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        respdu = m_cpdu->handlePrivateChatRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: // 群聊请求
    {
        m_cpdu->handleGroupChatRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST: // 创建文件夹请求
    {
        respdu = m_cpdu->handleCreateDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST: // 刷新文件夹请求
    {
        respdu = m_cpdu->handleFlushDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_REQUEST: // 删除文件请求
    {
        respdu = m_cpdu->handleDeleteDirorFileRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST: // 重命名文件请求
    {
        respdu = m_cpdu->handleRenameDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ENTRY_DIR_REQUEST: // 进入文件夹请求
    {
        respdu = m_cpdu->handleEntryDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_PRE_DIR_REQUEST: // 上一目录请求
    {
        respdu = m_cpdu->handlePreDirRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST: // 上传文件请求
    {
        respdu = m_cpdu->handleUploadFileOrDRequest(pdu, m_uploadFile);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST: // 下载文件请求
    {
        respdu = m_cpdu->handleDownloadFileRequest(pdu, m_pDownloadFile, m_pTimer);
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST: // 移动文件请求
    {
        respdu = m_cpdu->handleMoveFileRequest(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST: // 分享文件请求
    {
        respdu = m_cpdu->handleshareFileRequest(pdu, m_strName);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 分享文件通知响应处理
    {
        respdu = m_cpdu->handleShareFileNoteRespond(pdu);
        break;
    }
    default:
        break;
    }
    // 响应客户端
    if (NULL != respdu)
    {
        // qDebug() << resPdu -> uiMsgType << " " << resPdu ->caData;
        this->write((char *)respdu, respdu->uiPDULen);
        // 释放空间
        free(respdu);
        respdu = nullptr;
        return;
    }
    // 释放空间
    free(respdu);
    respdu = nullptr;
}
