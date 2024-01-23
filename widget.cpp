#include "widget.h"
#include "./ui_widget.h"
#include "protocol.h"
#include <QByteArray>
#include <QDebug>      // 调试
#include <QMessageBox> // 消息提示框
#include <QHostAddress>
#include "operatewidget.h"
Widget::Widget(QWidget *parent)
    : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    loadCofig();
    connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(showConncet()));
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(receiveMsg()));

    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::loadCofig()
{
    QFile file(":/client.config");
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll();
        QString strData = baData.toStdString().c_str();
        file.close();

        strData.replace("\r\n", " ");
        QStringList strList = strData.split(" ");
        m_strIP = strList[0];
        m_usPort = strList[1].toUShort();
        qDebug() << "IP:" << m_strIP << "Port:" << m_usPort;
    }
    else
    {

        QMessageBox::critical(this, "警告", "配置文件不存在");
    }
}

Widget &Widget::getInstance()
{
    static Widget instance;
    return instance;
}

QTcpSocket &Widget::getTcpSocket()
{
    return m_tcpSocket;
}

void Widget::setStrName(const QString &strName)
{
    m_strName = strName;
}

QString Widget::getStrName() const
{
    return m_strName;
}

QString Widget::getStrCurPath() const
{
    return m_strCurPath;
}

void Widget::setStrCurPath(const QString &strCurPath)
{
    m_strCurPath = strCurPath;
    return;
}

QString Widget::getStrRootPath() const
{
    return m_strRootPath;
}

void Widget::setStrRootPath(const QString &strRootPath)
{
    m_strRootPath = strRootPath;
    return;
}
void Widget::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        memcpy(pdu->caData, strName.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char *)pdu, pdu->uiPDULen);
       free(pdu);
        pdu = nullptr;
    }
    else
    {
        QMessageBox::critical(this, "警告", "用户名或密码不能为空");
    }
}
void Widget::on_logout_pb_clicked()
{
}
void Widget::showConncet()
{
    qDebug() << "连接服务器成功";
}
void Widget::receiveMsg()
{
    TransFile *transfile = OperateWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
    if (transfile->bTransForm)
    {
        QByteArray baBuffer = m_tcpSocket.readAll();
        transfile->file.write(baBuffer);

        transfile->iReceivedSize += baBuffer.size();
        if (transfile->iReceivedSize == transfile->iTotalSize)
        {
            QMessageBox::information(this, "下载文件", "下载文件成功！");
            transfile->file.close();
            transfile->file.setFileName("");
            transfile->bTransForm = false;
            transfile->iTotalSize = 0;
            transfile->iReceivedSize = 0;
        }
        else if (transfile->iReceivedSize > transfile->iTotalSize)
        {
            QMessageBox::warning(this, "下载文件", "下载文件失败！");
            transfile->file.close();
            transfile->file.setFileName("");
            transfile->bTransForm = false;
            transfile->iTotalSize = 0;
            transfile->iReceivedSize = 0;
        }
        return;
    }
    uint uiPDULen = 0;
    m_tcpSocket.read((char *)&uiPDULen, sizeof(uint));
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    m_tcpSocket.read((char *)pdu + sizeof(uint), uiPDULen - sizeof(uint));
    switch (pdu->uiMsgType)
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if (strcmp(pdu->caData, REGIST_OK) == 0)
        {
            QMessageBox::information(this, "注册", REGIST_OK);
        }
        else if (strcmp(pdu->caData, REGIST_FAILED) == 0)
        {
            QMessageBox::warning(this, "注册", REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
    {
        if (strcmp(pdu->caData, LOGIN_OK) == 0)
        {
            char caName[32] = {'\0'};
            strncpy(caName, pdu->caData + 32, 32);

            m_strRootPath = QString((char *)pdu->caMsg);
            qDebug() << "用户根目录" << m_strRootPath;

            Widget::getInstance().setStrCurPath(m_strRootPath);
            Widget::getInstance().setStrName(caName);
            setStrName(caName);
            qDebug() << "用户已登录：" << caName << " strName：" << m_strName;

           // OperateWidget::getInstance().setUserLabel(caName);
            OperateWidget::getInstance().show();

           // OperateWidget::getInstance().getPFileSystem()->flushDir();
            OperateWidget::getInstance().getPFriend()->flushFriendList();

            this->hide();
        }
        else if (strcmp(pdu->caData, LOGIN_FAILED) == 0)
        {
            QMessageBox::warning(this, "登录", LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USERS_RESPOND:
    {
        OperateWidget::getInstance().getPFriend()->setOnlineUsers(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USERS_RESPOND:
    {
        if (0 == strcmp(SEARCH_USER_OK, pdu->caData))
        {
            QMessageBox::information(this, "查找", OperateWidget::getInstance().getPFriend()->getStrSearchName() + SEARCH_USER_OK);
        }
        else if (0 == strcmp(SEARCH_USER_OFFLINE, pdu->caData))
        {
            QMessageBox::information(this, "查找", OperateWidget::getInstance().getPFriend()->getStrSearchName() + SEARCH_USER_OFFLINE);
        }
        else if (0 == strcmp(SEARCH_USER_EMPTY, pdu->caData))
        {
            QMessageBox::warning(this, "查找", OperateWidget::getInstance().getPFriend()->getStrSearchName() + SEARCH_USER_EMPTY);
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this, "添加好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char sourceName[32];
        strncpy(sourceName, pdu->caData + 32, 32);
        int ret = QMessageBox::information(this, "好友申请", QString("%1 想添加你为好友，是否同意？").arg(sourceName), QMessageBox::Yes | QMessageBox::No);
        PDU *respdu = mkPDU(0);
        strncpy(respdu->caData, pdu->caData, 32);
        strncpy(respdu->caData + 32, pdu->caData + 32, 32);
        if (ret == QMessageBox::Yes)
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_AGREE;
        }
        else
        {
            respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REJECT;
        }
        Widget::getInstance().getTcpSocket().write((char *)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE: // 对方同意加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已同意您的好友申请！").arg(pdu->caData));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REJECT: // 对方拒绝加好友
    {
        QMessageBox::information(this, "添加好友", QString("%1 已拒绝您的好友申请！").arg(pdu->caData));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND: // 刷新好友响应
    {
        OperateWidget::getInstance().getPFriend()->updateFriendList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND: // 删除好友响应
    {
        QMessageBox::information(this, "删除好友", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST: // 处理服务器转发过来的删除好友请求
    {
        char sourceName[32]; // 获取发送方用户名
        strncpy(sourceName, pdu->caData + 32, 32);
        QMessageBox::information(this, "删除好友", QString("%1 已删除与您的好友关系！").arg(sourceName));
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND: // 私聊好友消息响应（发送消息是否成功）
    {
        if (strcmp(PRIVATE_CHAT_OFFLINE, pdu->caData) == 0) // 发送消息失败
        {
            QMessageBox::information(this, "私聊", PRIVATE_CHAT_OFFLINE);
        }
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        char sourceName[32];
        strncpy(sourceName, pdu->caData + 32, 32);
        PrivateChatWid *priChatW = OperateWidget::getInstance().getPFriend()->searchPriChatWid(sourceName);
        if (priChatW == nullptr)
        {
            priChatW = new PrivateChatWid;
            priChatW->setStrChatName(sourceName);
            priChatW->setStrLoginName(m_strName);
            priChatW->setPriChatTitle(sourceName);
            OperateWidget::getInstance().getPFriend()->insertPriChatWidList(priChatW);
        }
        priChatW->updateShowMsgTE(pdu);
        priChatW->show();
        if (priChatW->isMinimized())
        {
            priChatW->showNormal();
        }
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST: // 群发好友信息请求（接收消息）
    {
        OperateWidget::getInstance().getPFriend()->updateGroupShowMsgTE(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND: // 创建文件夹响应
    {
        QMessageBox::information(this, "创建文件夹", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_DIR_RESPOND: // 刷新文件夹响应
    {
        OperateWidget::getInstance().getPFileSystem()->updateFileList(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND: // 删除文件或文件夹响应
    {
        QMessageBox::information(this, "删除文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND: // 重命名文件或文件夹响应
    {
        QMessageBox::information(this, "重命名文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_ENTRY_DIR_RESPOND:
    {
        qDebug() << "进入文件夹响应：" << pdu->caData;
        if (strcmp(ENTRY_DIR_OK, pdu->caData) == 0)
        {
            OperateWidget::getInstance().getPFileSystem()->updateFileList(pdu);
            QString entryPath = OperateWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if (!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OperateWidget::getInstance().getPFileSystem()->clearStrTryEntryDir();
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "进入文件夹", pdu->caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_PRE_DIR_RESPOND:
    {
        qDebug() << "上一文件夹" << pdu->caData;
        if (strcmp(PRE_DIR_OK, pdu->caData) == 0)
        {
            OperateWidget::getInstance().getPFileSystem()->updateFileList(pdu);
            QString entryPath = OperateWidget::getInstance().getPFileSystem()->strTryEntryDir();
            if (!entryPath.isEmpty())
            {
                m_strCurPath = entryPath;
                OperateWidget::getInstance().getPFileSystem()->clearStrTryEntryDir();
                qDebug() << "当前路径：" << m_strCurPath;
            }
        }
        else
        {
            QMessageBox::warning(this, "上一文件夹", pdu->caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND: // 上传文件响应
    {
        if(strcmp(UPLOAD_FILE_START, pdu -> caData) == 0) // 开始上传文件数据内容
        {
            OperateWidget::getInstance().getPFileSystem()->startTimer();
        }
        else if(strcmp(UPLOAD_FILE_OK, pdu -> caData) == 0) // 上传文件成功
        {
            QMessageBox::information(this, "上传文件", pdu -> caData);
        }
        else if(strcmp(UPLOAD_FILE_FAILED, pdu -> caData) == 0) // 上传失败
        {
            QMessageBox::warning(this, "上传文件", pdu -> caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND: // 下载文件响应
    {
        if (strcmp(DOWNLOAD_FILE_START, pdu->caData) == 0) // 开始下载文件数据内容
        {
            // TransFile *transFile = OperateWidget::getInstance().getPFileSystem()->getDownloadFileInfo();
            qint64 ifileSize = 0;
            char strFileName[32];
            sscanf((char *)pdu->caMsg, "%s %lld", strFileName, &ifileSize);
            qDebug() << "下载文件中：" << strFileName << ifileSize;

            if (strlen(strFileName) > 0 && transfile->file.open(QIODevice::WriteOnly))
            {
                transfile->bTransForm = true;
                transfile->iTotalSize = ifileSize;
                transfile->iReceivedSize = 0;
            }
            else
            {
                QMessageBox::warning(this, "下载文件", "下载文件失败！");
            }
        }
        else if (strcmp(DOWNLOAD_FILE_OK, pdu->caData) == 0) // 下载文件成功
        {
            QMessageBox::information(this, "下载文件", pdu->caData);
        }
        else if (strcmp(DOWNLOAD_FILE_FAILED, pdu->caData) == 0) // 下载失败
        {
            QMessageBox::warning(this, "下载文件", pdu->caData);
        }
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND: // 移动文件响应
    {
        QMessageBox::information(this, "移动文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND: // 分享文件响应
    {
        QMessageBox::information(this, "分享文件", pdu->caData);
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE: // 被分享文件提醒
    {
        char caFileName[32]; // 文件名
        char caSouName[32];  // 用户名
        int iFilePathLen = pdu->uiMsgLen;
        char caFilePath[iFilePathLen]; // 文件路径

        memcpy(caSouName, pdu->caData, 32);
        memcpy(caFileName, pdu->caData + 32, 32);
        QString strShareNote = QString("%1 想要分享 %2 文件给您，\n是否接收？").arg(caSouName).arg(caFileName);
        QMessageBox::StandardButton sbShareNote = QMessageBox::question(this, "分享文件", strShareNote);
        if (sbShareNote == QMessageBox::No)
        { // 拒绝接收
            break;
        }

        // 同意接收
        qDebug() << "接收文件：" << caSouName << " " << caFileName;
        memcpy(caFilePath, (char *)pdu->caMsg, iFilePathLen);
        QString strRootDir = m_strRootPath; // 用户根目录
        PDU *resPdu = mkPDU(iFilePathLen + strRootDir.size() + 1);
        resPdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
        sprintf(resPdu->caData, "%d %d", iFilePathLen, strRootDir.size());
        sprintf((char *)resPdu->caMsg, "%s %s", caFilePath, strRootDir.toStdString().c_str());
        qDebug() << (char *)resPdu->caMsg;
        m_tcpSocket.write((char *)resPdu, resPdu->uiPDULen);
        delete (resPdu);
        resPdu = NULL;

        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND: // 被分享文件通知响应的处理结果
    {
        QMessageBox::information(this, "分享文件", pdu->caData);
        break;
    }
    default:
        break;
    }
    // 释放空间
    free(pdu);
    pdu = NULL;
}
void Widget::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if (!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        memcpy(pdu->caData, strName.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char *)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
    }
    else
    {
        QMessageBox::critical(this, "登录", "登录失败：用户名或密码为空！");
    }
}
