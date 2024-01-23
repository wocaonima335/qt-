#ifndef FRIEND_H
#define FRIEND_H
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout> // 垂直布局
#include <QHBoxLayout> // 水平布局
#include "protocol.h"
#include "onlineuserwid.h"  // 所有在线用户
#include "privatechatwid.h" // 私聊窗口

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);

    QString getStrSearchName() const;
    void setStSeachName(const QString &getStrSearchName);

    void setOnlineUsers(PDU *pdu);
    void updateFriendList(PDU *pdu);

    QListWidget *getPFriend() const;
public slots:
    void showOrHideOnlineUserW();
    void searchUser();
    void flushFriendList();
    void deleteFriend();
    void privateChat();
    PrivateChatWid *searchPriChatWid(const char *chatName);
    void insertPriChatWidList(PrivateChatWid *priChat);
    void groupChatSendMsg();
    void updateGroupShowMsgTE(PDU *pdu);
signals:
private:
    QListWidget *m_pFriendLW;         // 好友列表
    QPushButton *m_pDelFriendPB;      // 删除好友
    QPushButton *m_pFlushFriendPB;    // 刷新好友列表
    QPushButton *m_pSOrHOnlineUserPB; // 显示/隐藏所有在线用户
    QPushButton *m_pSearchUserPB;     // 查找用户
    QLineEdit *m_pGroupInputLE;       // 群聊信息输入框
    QPushButton *m_pGroupSendMsgPB;   // 群聊发送消息
    QTextEdit *m_pGroupShowMsgTE;     // 显示群聊信息
    QPushButton *m_pPrivateChatPB;    // 私聊按钮，默认群聊

    onlineuserwid *m_pOnlineUserW; // 所有在线用户页面

    QString m_strSearchName; // 查找的用户的名字

    QList<PrivateChatWid *> m_priChatWidList; // 所有私聊的窗口
};
#endif