#ifndef CPDU_H
#define CPDU_H
#include "protocol.h"
#include "dboperate.h"
#include <QTimer>
#include <QDateTime>
class Cpdu
{
public:
    Cpdu();
    ~Cpdu();
    PDU *hanndleRegistRequest(PDU *pdu);
    PDU *handleLoginRequest(PDU *pdu, QString &m_strName);
    PDU *handleOnlineUserRequest();
    PDU *handleSearchUserRequest(PDU *pdu);
    void handleAddFriendAgreeRequest(PDU *pdu);
    void handleAddFriendRejectRequest(PDU *pdu);
    PDU *handleAddFriendRequest(PDU *pdu);
    PDU *handleDeleFriendRequest(PDU *pdu);
    PDU *handlePrivateChatRequest(PDU *pdu);
    void handleGroupChatRequest(PDU *pdu);
    PDU *handleCreateDirRequest(PDU *pdu);
    PDU *handleDeleteDirorFileRequest(PDU *pdu);
    PDU *handleRenameDirRequest(PDU *pdu);
    PDU *handlePreDirRequest(PDU *pdu);
    PDU *handleUploadFileOrDRequest(PDU *pdu, TransFile *transfile);
    PDU *handleDownloadFileRequest(PDU *pdu, QFile *fDownLoadFile, QTimer *ptimer);
    PDU *handleMoveFileRequest(PDU *pdu);
    PDU *handleshareFileRequest(PDU *pdu, QString strSouName);
    bool copyDir(QString strOldPath, QString strNewPath);
    PDU *handleFlushFriendRequest(PDU *pdu);
    PDU *handleFlushDirRequest(PDU *pdu);
    PDU *handleShareFileNoteRespond(PDU *pdu);
    PDU *handleEntryDirRequest(PDU *pdu);

private:
    PDU *respdu;
};
#endif