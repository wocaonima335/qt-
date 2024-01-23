#include "Cpdu.h"
#include <QDir>
#include "mytcpserver.h"
Cpdu::Cpdu() : respdu(nullptr)
{
}

Cpdu::~Cpdu()
{
    if (respdu != nullptr)
    {
        free(respdu);
        respdu = nullptr;
    }
}

PDU *Cpdu::hanndleRegistRequest(PDU *pdu)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};

    strncpy(caName, pdu->caData, sizeof(caName));
    strncpy(caPwd, pdu->caData + sizeof(caName), sizeof(caPwd));

    bool ret = DBoperate::getInstacne().handleRegist(caName, caPwd);

    // 响应客户端
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
    if (ret)
    {
        strcpy(respdu->caData, REGIST_OK);
        QDir dir;
        ret = dir.mkdir(QString("%1/%2").arg(MyTcpServer::getInstance().getStrRootPath()).arg(caName));
        qDebug() << "创建新用户文件夹" << ret;
    }
    if (!ret)
    {
        strcpy(respdu->caData, REGIST_FAILED);
    }
    return respdu;
}

PDU *Cpdu::handleLoginRequest(PDU *pdu, QString &m_strName)
{
    char caName[32] = {'\0'};
    char caPwd[32] = {'\0'};

    strncpy(caName, pdu->caData, sizeof(caName));
    strncpy(caPwd, pdu->caData + sizeof(caName), sizeof(caPwd));
    bool ret = DBoperate::getInstacne().handleLogin(caName, caPwd);

    respdu = nullptr;

    if (ret)
    {
        QString strPath = QString("%1/%2")
                              .arg(MyTcpServer::getInstance().getStrRootPath())
                              .arg(caName); // 用户文件系统根目录
        qDebug() << "登入用户的路径" << strPath;
        respdu = mkPDU(strPath.size() + 1);
        memcpy(respdu->caData, LOGIN_OK, 32);
        memcpy(respdu->caData + 32, caName, 32);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        m_strName = caName;
        strncpy((char *)respdu->caMsg, strPath.toStdString().c_str(), strPath.size() + 1);
    }
    else
    {
        respdu = mkPDU(0);
        strcpy(respdu->caData, LOGIN_FAILED);
    }
    respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
    qDebug() << "登录处理：" << respdu->uiMsgType << " " << respdu->caData << " " << respdu->caData + 32;

    return respdu;
}

PDU *Cpdu::handleOnlineUserRequest()
{
    QStringList strList = DBoperate::getInstacne().handleOnlineUsers();
    uint uiMsgLen = strList.size() * 32 + 1;
    respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_RESPOND;
    qDebug() << "用户在线人数:" << strList.count();
    for (int i = 0; i < strList.count(); i++)
    {
        strncpy((char *)(respdu->caMsg) + 32 * i, strList[i].toStdString().c_str(), strList[i].size() + 1);
        qDebug() << "用户在线列表:" << (char *)(respdu->caMsg) + 32 * i;
    }
    return respdu;
}

PDU *Cpdu::handleSearchUserRequest(PDU *pdu)
{
    QString strName = (char *)pdu->caData;
    int ret = DBoperate::getInstacne().handleSearchUser(strName.toLocal8Bit().data());
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USERS_RESPOND;
    if (ret == 1)
    {
        strcpy(respdu->caData, SEARCH_USER_OK);
    }
    else if (ret == 0)
    {
        strcpy(respdu->caData, SEARCH_USER_OFFLINE);
    }
    else if (ret == 2)
    {
        strcpy(respdu->caData, SEARCH_USER_EMPTY);
    }
    return respdu;
}

void Cpdu::handleAddFriendAgreeRequest(PDU *pdu)
{
    char addName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    strncpy(addName, pdu->caData, 32);
    strncpy(sourceName, pdu->caData + 32, 32);
    DBoperate::getInstacne().handleAddFriendAgree(addName, sourceName);
    MyTcpServer::getInstance().forwardMsg(sourceName, pdu);
}

void Cpdu::handleAddFriendRejectRequest(PDU *pdu)
{
    char sourceName[32] = {'\0'};
    strncpy(sourceName, pdu->caData + 32, 32);
    MyTcpServer::getInstance().forwardMsg(sourceName, pdu);
}

PDU *Cpdu::handleAddFriendRequest(PDU *pdu)
{
    char addName[32] = {'\0'};
    char sourceName[32] = {'\0'};

    strncpy(addName, pdu->caData, 32);
    strncpy(sourceName, pdu->caData + 32, 32);

    qDebug() << "handleAddFriendRequest" << addName << " " << sourceName;
    int UserStatus = DBoperate::getInstacne().haneleAddFriend(addName, sourceName);
    respdu = nullptr;
    switch (UserStatus)
    {
    case 0:
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_OFFLINE);
        break;
    }
    case 1:
    {
        MyTcpServer::getInstance().forwardMsg(addName, pdu);
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_OK);
        break;
    }
    case 2:
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_EMPTY);
        break;
    }
    case3:
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, ADD_FRIEND_EXIST);
        break;
    }
    case 4:
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
        strcpy(respdu->caData, UNKNOWN_ERROR);
        break;
    }
    default:
        break;
    }
    return respdu;
}

PDU *Cpdu::handleDeleFriendRequest(PDU *pdu)
{
    char deleteName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    strncpy(deleteName, pdu->caData, 32);
    strncpy(sourceName, pdu->caData + 32, 32);
    int ret = DBoperate::getInstacne().handleDeleteFriend(deleteName, sourceName);
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
    if (ret)
    {
        strncpy(pdu->caData, DEL_FRIEND_OK, 32);
    }
    else
    {
        strncpy(pdu->caData, DEL_FRIEND_FAILED, 32);
    }
    MyTcpServer::getInstance().forwardMsg(deleteName, pdu);
    return respdu;
}

PDU *Cpdu::handlePrivateChatRequest(PDU *pdu)
{
    char chatName[32] = {'\0'};
    char sourceName[32] = {'\0'};
    strncpy(chatName, pdu->caData, 32);
    strncpy(sourceName, pdu->caData + 32, 32);
    qDebug() << "handlePrivateChatRequest" << chatName << " " << sourceName;

    respdu = nullptr;

    // 转发给对方消息，0对方存在不在线，1对方存在在线
    bool ret = MyTcpServer::getInstance().forwardMsg(chatName, pdu);

    if (!ret)
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND;
        strcpy(respdu->caData, PRIVATE_CHAT_OFFLINE);
    }
    return respdu;
}

void Cpdu::handleGroupChatRequest(PDU *pdu)
{
    QStringList strList = DBoperate::getInstacne().handleFlushFriendRequest(pdu->caData);
    qDebug() << "handleGroupChatRequest" << strList;
    for (QString strName : strList)
    {
        MyTcpServer::getInstance().forwardMsg(strName, pdu);
    }
}

PDU *Cpdu::handleCreateDirRequest(PDU *pdu)
{
    char caDirName[32];
    char caCurPath[pdu->uiMsgLen];
    strncpy(caDirName, pdu->caData, 32);
    strncpy(caCurPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    QString strDir = QString("%1/%2").arg(caCurPath).arg(caDirName);
    qDebug() << "handleCreateDirRequest" << strDir;
    QDir dir;
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_PRE_DIR_RESPOND;
    if (dir.exists(caCurPath))
    {
        if (dir.exists(strDir))
        {
            strncpy(respdu->caData, CREATE_DIR_EXIST, 32);
        }
        else
        {
            dir.mkdir(strDir);
            strncpy(respdu->caData, CREATE_DIR_OK, 32);
        }
    }
    else
    {
        strncpy(respdu->caData, PATH_NOT_EXIST, 32);
    }
    return respdu;
}

PDU *Cpdu::handleDeleteDirorFileRequest(PDU *pdu)
{
    respdu = mkPDU(0);
    char strDelPath[pdu->uiMsgLen];
    memcpy(strDelPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    qDebug() << "handleDeleteDirorFileRequest" << strDelPath;
    QDir dir;
    respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
    if (!dir.exists(strDelPath))
    {
        strncpy(respdu->caData, PATH_NOT_EXIST, 32);
    }
    else
    {
        bool ret = false;
        QFileInfo fileinfo(strDelPath);
        if (fileinfo.isDir())
        {
            dir.setPath(strDelPath);
            ret = dir.removeRecursively();
        }
        else if (fileinfo.isFile())
        {
            ret = dir.remove(strDelPath);
        }
        if (ret)
        {
            strncpy(respdu->caData, DELETE_FILE_OK, 32);
        }
        else
        {
            strncpy(respdu->caData, DELETE_FILE_FAILED, 32);
        }
        qDebug() << respdu->caData;
        return respdu;
    }
}

PDU *Cpdu::handleRenameDirRequest(PDU *pdu)
{
    PDU *respdu = mkPDU(0);
    char strCurPath[pdu->uiMsgLen];
    char caOldName[32];
    char caNewName[32];
    memcpy(strCurPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    strncpy(caOldName, pdu->caData, 32);
    strncpy(caNewName, pdu->caData + 32, 32);
    qDebug() << "重命名文件：" << strCurPath << " " << caOldName << " -> " << caNewName;
    QDir dir;
    respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
    dir.setPath(strCurPath);
    if (dir.rename(caOldName, caNewName))
    {
        strncpy(respdu->caData, RENAME_FILE_OK, 32);
    }
    else
    {
        strncpy(respdu->caData, RENAME_FILE_FAILED, 32);
    }
    qDebug() << respdu->caData;
    return respdu;
}

PDU *Cpdu::handlePreDirRequest(PDU *pdu)
{
    char strPrePath[pdu->uiMsgLen];
    memcpy(strPrePath, (char *)pdu->caMsg, pdu->uiMsgLen);
    qDebug() << "上一目录：" << strPrePath;
    respdu = nullptr;
    QDir dir(strPrePath);
    if (!dir.exists())
    {
        respdu = mkPDU(0);
        strncpy(respdu->caData, PRE_DIR_FAILED, 32);
    }
    else
    {
        respdu = handleFlushDirRequest(pdu); // 通过该函数获取文件夹下内容
    }
    respdu->uiMsgType = ENUM_MSG_TYPE_PRE_DIR_RESPOND;
    qDebug() << "1 respdu->caData:" << respdu->caData;
    if (strcmp(respdu->caData, FLUSH_DIR_OK) == 0)
    {
        strncpy(respdu->caData, PRE_DIR_OK, 32);
        qDebug() << "2 respdu->caData:" << respdu->caData;
    }
    else
    {
        strncpy(respdu->caData, PRE_DIR_FAILED, 32);
        qDebug() << "3 respdu->caData:" << respdu->caData;
    }
    return respdu;
}

PDU *Cpdu::handleUploadFileOrDRequest(PDU *pdu, TransFile *transfile)
{
    char caCurPath[pdu->uiMsgLen];
    char caFileName[32] = {'\0'};
    qint64 fileSize = 0;
    strncpy(caCurPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    sscanf(pdu->caData, "%s %lld", caFileName, &fileSize);
    QString strFileName = QString("%1/%2").arg(caCurPath).arg(caFileName);
    qDebug() << "handleUploadFileOrDRequest" << strFileName << fileSize;

    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
    transfile->file.setFileName(strFileName);
    if (transfile->file.open(QIODevice::WriteOnly))
    {
        transfile->bTransForm = true;
        transfile->iTotalSize = fileSize;
        transfile->iReceivedSize = 0;
        memcpy(respdu->caData, UPLOAD_FILE_START, 32);
    }
    else
    {
        memcpy(respdu->caData, UPLOAD_FILE_FAILED, 32);
    }
    return respdu;
}

PDU *Cpdu::handleDownloadFileRequest(PDU *pdu, QFile *fDownLoadFile, QTimer *ptimer)
{
    char caFileName[32] = {'\0'};
    char caCurPath[pdu->uiMsgLen];
    memcpy(caCurPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    memcpy(caFileName, (char *)respdu->caData, 32);
    QString DownFile = QString("%1/%2").arg(caCurPath).arg(caFileName);
    fDownLoadFile->setFileName(DownFile);
    qint64 filesize = fDownLoadFile->size();
    respdu = nullptr;
    if (fDownLoadFile->open(QIODevice::ReadOnly))
    {
        respdu = mkPDU(32 + sizeof(qint64) + 5);
        respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strncpy(respdu->caData, DOWNLOAD_FILE_START, 32);
        sprintf((char *)respdu->caMsg, "%s %lld", caFileName, filesize);
        ptimer->start(1000);
        qDebug() << (char *)respdu->caMsg;
    }
    else
    {
        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        strncpy(respdu->caData, DOWNLOAD_FILE_FAILED, 32);
    }
    return respdu;
}

PDU *Cpdu::handleMoveFileRequest(PDU *pdu)
{
    char caMoveFileName[32];
    int iOldDirSize = 0;
    int iDesDirSize = 0;
    sscanf(pdu->caData, "%s %d %d", caMoveFileName, &iDesDirSize, &iOldDirSize);
    char caOldDir[iOldDirSize + 33];
    char caDesDir[iDesDirSize + 33];
    sscanf((char *)pdu->caMsg, "%s %s", caDesDir, caOldDir);
    qDebug() << "移动文件：" << caMoveFileName << "从" << caOldDir << "到" << caDesDir;

    QFileInfo fileinfo(caDesDir);
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
    if (!fileinfo.isDir())
    {
        strncpy(respdu->caData, MOVE_FILE_OK, 32);
        return respdu;
    }
    strcat(caOldDir, "/");
    strcat(caOldDir, caMoveFileName);
    strcat(caDesDir, "/");
    strcat(caDesDir, caMoveFileName);
    if (QFile::rename(caOldDir, caDesDir))
    {
        strncpy(respdu->caData, MOVE_FILE_OK, 32);
    }
    else
    {
        strncpy(respdu->caData, MOVE_FILE_FAILED, 32);
    }
    return respdu;
}

PDU *Cpdu::handleshareFileRequest(PDU *pdu, QString strSouName)
{
    int iUserNum = 0;
    char caFileName[32];
    sscanf(pdu->caData, "%s %d", caFileName, &iUserNum);
    qDebug() << "handleshareFileRequest" << caFileName << iUserNum;

    const int iFilePathLen = pdu->uiMsgLen - iUserNum * 32;
    char caFilePath[iFilePathLen];
    respdu = mkPDU(iFilePathLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
    memcpy(respdu->caData, strSouName.toStdString().c_str(), strSouName.size());
    memcpy(respdu->caData + 32, caFileName, 32);
    memcpy(caFilePath, (char *)(pdu->caMsg) + 32 * iUserNum, iFilePathLen);
    memcpy((char *)respdu->caMsg, caFilePath, iFilePathLen); // 发送文件路径

    char caDesName[32];
    for (int i = 0; i < iUserNum; i++)
    {
        memcpy(caDesName, (char *)(pdu->caMsg) + 32 * i, 32);
        MyTcpServer::getInstance().forwardMsg(caDesName, respdu);
        qDebug() << caDesName;
    }
    free(respdu);
    respdu = nullptr;

    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    strncpy(respdu->caData, SHARE_FILE_OK, 32);
    return respdu;
}

bool Cpdu::copyDir(QString strOldPath, QString strNewPath)
{
    int ret = true;
    QDir dir;
    qDebug() << "分享目录：" << strOldPath << "  " << strNewPath;
    dir.setPath(strOldPath);
    QFileInfoList fileList = dir.entryInfoList();
    QString strOldFile;
    QString strNewFile;
    for (QFileInfo fileInfo : fileList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
        {
            continue;
        }
        strOldFile = QString("%1/%2").arg(strOldPath).arg(fileInfo.fileName());
        strNewFile = QString("%1/%2").arg(strNewPath).arg(fileInfo.fileName());
        if (fileInfo.isFile())
        {
            ret = ret && QFile::copy(strOldFile, strNewFile);
        }
        else if (fileInfo.isDir())
        {
            ret = ret && copyDir(strOldFile, strNewFile);
        }
        qDebug() << strOldFile << " ->" << strNewFile;
    }
    return ret;
}

PDU *Cpdu::handleShareFileNoteRespond(PDU *pdu)
{
    int iOldPathLen = 0;
    int iNewPathLen = 0;
    sscanf(pdu->caData, "%d %d", &iOldPathLen, &iNewPathLen);
    char caOldPath[iOldPathLen];
    char caNewPath[iNewPathLen];
    sscanf((char *)pdu->caMsg, "%s %s", caOldPath, caNewPath);
    qDebug() << "handleShareFileNoteRequest" << caOldPath << "  " << caNewPath;
    // 获得文件的新路径
    char *pIndex = strrchr(caOldPath, '/');
    QString strNewPath = QString("%1/%2").arg(caNewPath).arg(pIndex + 1);
    qDebug() << "同意分享文件：" << caOldPath << " " << strNewPath;
    QFileInfo fileinfo(caOldPath);
    bool ret = false;
    if (fileinfo.isFile())
    {
        ret = QFile::copy(caOldPath, strNewPath);
    }
    else if (fileinfo.isDir())
    {
        ret = copyDir(caOldPath, strNewPath);
    }
    else
    {
        ret = false;
    }
    respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
    if (ret)
    {
        memcpy(respdu->caData, SHARE_FILE_OK, 32);
    }
    else
    {
        memcpy(respdu->caData, SHARE_FILE_FAILED, 32);
    }
    return respdu;
}

PDU *Cpdu::handleEntryDirRequest(PDU *pdu)
{
    char strEntryPath[pdu->uiMsgLen]; // 进入文件夹路径
    memcpy(strEntryPath, (char *)pdu->caMsg, pdu->uiMsgLen);
    qDebug() << "进入 " << strEntryPath;
    respdu = NULL;
    QDir dir(strEntryPath);

    if (!dir.exists()) // 请求文件夹不存在
    {
        respdu = mkPDU(0);
        strncpy(respdu->caData, PATH_NOT_EXIST, 32);
    }
    else
    {
        QFileInfo fileInfo(strEntryPath);
        if (!fileInfo.isDir()) // 不是文件夹
        {
            respdu = mkPDU(0);
            strncpy(respdu->caData, ENTRY_DIR_FAILED, 32);
        }
        else
        {
            respdu = handleFlushDirRequest(pdu); //  通过该函数获取文件夹下的内容
        }
    }
    respdu->uiMsgType = ENUM_MSG_TYPE_ENTRY_DIR_RESPOND;
    qDebug() << "1 resPdu -> caData ：" << respdu->caData;
    if (strcmp(respdu->caData, FLUSH_DIR_OK) == 0)
    {
        strncpy(respdu->caData, ENTRY_DIR_OK, 32);
        qDebug() << "2 resPdu -> caData ：" << respdu->caData;
    }
    else
    {
        strncpy(respdu->caData, ENTRY_DIR_FAILED, 32);
        qDebug() << "2 resPdu -> caData ：" << respdu->caData;
    }
    return respdu;
}

PDU *Cpdu::handleFlushFriendRequest(PDU *pdu)
{
    char strName[32] = {'\0'};
    strncpy(strName, pdu->caData, 32);
    qDebug() << "handleFlushFriendRequest" << strName;
    QStringList strList = DBoperate::getInstacne().handleFlushFriendRequest(strName);
    uint MsgLen = strList.size() / 2 * 36;
    respdu = mkPDU(MsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
    for (int i = 0; i * 2 < strList.size(); i++)
    {
        strncpy((char *)(respdu->caMsg) + i * 36, strList.at(i * 2).toStdString().c_str(), 32);
        strncpy((char *)(respdu->caMsg) + 32 + i * 36, strList.at(i * 2 + 1).toStdString().c_str(), 4);
    }
    return respdu;
}

PDU *Cpdu::handleFlushDirRequest(PDU *pdu)
{
    char caCurDir[pdu->uiMsgLen];

    memcpy(caCurDir, (char *)pdu->caMsg, pdu->uiMsgLen);
    qDebug() << "handleFlushDirRequest" << caCurDir;
    QDir dir;
    respdu = nullptr;
    if (!dir.exists(caCurDir))
    {
        respdu = mkPDU(0);
        strncpy(respdu->caData, PATH_NOT_EXIST, 32);
    }
    else
    {
        dir.setPath(caCurDir);
        QFileInfoList fileInfoList = dir.entryInfoList();
        int iFileNum = fileInfoList.size();

        respdu = mkPDU(sizeof(FileInfo) * iFileNum);
        FileInfo *pfileInfo = nullptr;
        strncpy(respdu->caData, FLUSH_DIR_OK, 32);
        for (int i = 0; i < iFileNum; i++)
        {
            /*
             * 首先创建了一个FileInfo结构体的指针，然后每次通过这个指针指向resPdu → caMsg的地址空间，
             * 由于resPdu → caMsg被强转为FileInfo*类型，所以每次移动也是以FileInfo大小为单位移动，
             * 然后后面我们直接通过FileInfo指针来操作PDU的caMsg的空间，操作更为方便。
             */

            pfileInfo = (FileInfo *)(respdu->caMsg) + i;
            memcpy(pfileInfo->caName, fileInfoList[i].fileName().toStdString().c_str(), fileInfoList[i].fileName().size());
            pfileInfo->bIsDir = fileInfoList[i].isDir();
            pfileInfo->uisize = fileInfoList[i].size();
            QDateTime dtLastTime = fileInfoList[i].lastModified(); // 获取文件最后修改时间
            QString strLastTime = dtLastTime.toString("yyyy-MM-dd hh:mm:ss");
            memcpy(pfileInfo->caTime, strLastTime.toStdString().c_str(), strLastTime.size());
            qDebug() << "handleFlushDirRequest" << pfileInfo->caName << pfileInfo->bIsDir << pfileInfo->uisize << pfileInfo->caTime;
        }
    }
    respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
    return respdu;
}
