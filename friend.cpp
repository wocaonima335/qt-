#include "freind.h"
#include "widget.h"
#include <QInputDialog>
#include <QMessageBox>
#include "privatechatwid.h"

Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pFriendLW = new QListWidget;
    m_pGroupInputLE = new QLineEdit;
    m_pGroupShowMsgTE = new QTextEdit;
    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pSOrHOnlineUserPB = new QPushButton("在线用户");
    m_pSearchUserPB = new QPushButton("查找用户");
    m_pPrivateChatPB = new QPushButton("私聊");
    m_pGroupSendMsgPB = new QPushButton("群聊");
    m_pOnlineUserW = new onlineuserwid;

    QVBoxLayout *pLeftRightVBL = new QVBoxLayout;
    pLeftRightVBL->addWidget(m_pPrivateChatPB);
    pLeftRightVBL->addWidget(m_pDelFriendPB);
    pLeftRightVBL->addWidget(m_pFlushFriendPB);
    pLeftRightVBL->addWidget(m_pSOrHOnlineUserPB);
    pLeftRightVBL->addWidget(m_pSearchUserPB);
    QHBoxLayout *pRightDownHBL = new QHBoxLayout;
    pRightDownHBL->addWidget(m_pGroupInputLE);
    pRightDownHBL->addWidget(m_pGroupSendMsgPB);

    QVBoxLayout *pRightVBL = new QVBoxLayout;
    pRightVBL->addWidget(m_pGroupShowMsgTE);
    pRightVBL->addLayout(pRightDownHBL);
    QHBoxLayout *pMainHBL = new QHBoxLayout;
    pMainHBL->addWidget(m_pFriendLW);
    pMainHBL->addLayout(pLeftRightVBL);
    pMainHBL->addLayout(pRightVBL);

    m_pOnlineUserW->hide();
    setLayout(pMainHBL);

    connect(m_pSOrHOnlineUserPB, SIGNAL(clicked(bool)), this, SLOT(showOrHideOnlineUserW()));
    connect(m_pSearchUserPB, SIGNAL(clicked(bool)), this, SLOT(searchUser()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(flushFriendList()));
    connect(m_pDelFriendPB, SIGNAL(clicked(bool)), this, SLOT(deleteFriend()));
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool)), this, SLOT(privateChat()));
    connect(m_pGroupSendMsgPB, SIGNAL(clicked(bool)), this, SLOT(groupChatSendMsg()));
}

QString Friend::getStrSearchName() const
{
    return m_strSearchName;
}

void Friend::setStSeachName(const QString &getStrSearchName)
{
    m_strSearchName = getStrSearchName;
    return;
}

void Friend::setOnlineUsers(PDU *pdu)
{
    if (pdu == nullptr)
    {
        return;
    }
    m_pOnlineUserW->setOnlineUsers(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if (pdu == nullptr)
    {
        return;
    }
    uint uiSize = pdu->uiMsgLen / 36;
    char caName[32] = {'\0'};
    char caOnline[4] = {'\0'};

    m_pFriendLW->clear();
    for (uint i = 0; i < uiSize; i++)
    {
        memcpy(caName, (char *)(pdu->caMsg) + i * 36, 32);
        memcpy(caOnline, (char *)(pdu->caMsg) + 32 + i * 36, 4);
        qDebug() << "客户端好友" << caName << " " << caOnline;
        m_pFriendLW->addItem(QString("%1\t%2").arg(caName).arg(strcmp(caOnline, "1") == 0 ? "在线" : "离线"));
    }
}

QListWidget *Friend::getPFriend() const
{
    return m_pFriendLW;
}
void Friend::searchUser()
{
    m_strSearchName = QInputDialog::getText(this, "搜索", "用户名:");
    if (!m_strSearchName.isEmpty())
    {
        qDebug() << "查找" << m_strSearchName;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USERS_REQUEST;
        memcpy((char *)pdu->caData, m_strSearchName.toStdString().c_str(), m_strSearchName.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
    }
}
void Friend::flushFriendList()
{
    QString strName = Widget::getInstance().getStrName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLSUH_FRIEND_REQUEST;
    strncpy(pdu->caData, strName.toStdString().c_str(), strName.size());
    Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}
void Friend::deleteFriend()
{
    if (m_pFriendLW->currentItem() == nullptr)
    {
        return;
    }
    QString friName = m_pFriendLW->currentItem()->text();
    friName = friName.split("\t")[0];
    QString loginName = Widget::getInstance().getStrName();

    qDebug() << friName;
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    strncpy(pdu->caData, friName.toStdString().c_str(), 32);
    strncpy(pdu->caData + 32, loginName.toStdString().c_str(), 32);
    Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiMsgLen);
    free(pdu);
    pdu = nullptr;
}

void Friend::privateChat()
{
    if (m_pFriendLW->currentItem() == nullptr)
    {
        return;
    }
    QString friName = m_pFriendLW->currentItem()->text();
    friName = friName.split("\t")[0];
    QString loginName = Widget::getInstance().getStrName();
    PrivateChatWid *priChat = searchPriChatWid(friName.toStdString().c_str());
    if (priChat == nullptr)
    {
        priChat = new PrivateChatWid;
        priChat->setStrChatName(friName);
        priChat->setStrLoginName(loginName);
        priChat->setPriChatTitle(friName.toStdString().c_str());
        m_priChatWidList.push_back(priChat);
    }
    if (priChat->isHidden())
    {
        priChat->show();
    }
    if (priChat->isMinimized())
    {
        priChat->showNormal();
    }
}

PrivateChatWid *Friend::searchPriChatWid(const char *chatName)
{
    for (PrivateChatWid *ptr : m_priChatWidList)
    {
        if (chatName == ptr->strChatName())
        {
            return ptr;
        }
    }
    return nullptr;
}

void Friend::insertPriChatWidList(PrivateChatWid *priChat)
{
    m_priChatWidList.append(priChat);
}

void Friend::groupChatSendMsg()
{
    QString Msg = m_pGroupInputLE->text();
    if (Msg.isEmpty())
    {
        QMessageBox::warning(this, "群聊", "消息不能为空");
    }
    m_pGroupInputLE->clear();
    m_pGroupShowMsgTE->append(QString("%1 : %2").arg(Widget::getInstance().getStrName()).arg(Msg));
    PDU *pdu = mkPDU(Msg.size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
    strncpy(pdu->caData, Widget::getInstance().getStrName().toStdString().c_str(), 32);
    strncpy((char*)pdu->caMsg, Msg.toStdString().c_str(), Msg.size());
    Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

void Friend::updateGroupShowMsgTE(PDU *pdu)
{
    QString strMsg = QString("%1 : %2").arg(pdu->caData).arg((char*)pdu->caMsg);

    qDebug()<<"消息："<<strMsg;
    m_pGroupShowMsgTE->append(strMsg);
}

void Friend::showOrHideOnlineUserW()
{
    if (m_pOnlineUserW->isHidden())
    {
        m_pOnlineUserW->show();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USERS_REQUEST;
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
    }
    else
    {
        m_pOnlineUserW->hide();
    }
}
