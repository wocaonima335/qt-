#include "sharedfilefriendlist.h"
#include <QDebug>
#include "widget.h"
#include "protocol.h"
#include"operatewidget.h"
sharedFileFriendList::sharedFileFriendList(QWidget *parent)
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancleSelectPB = new QPushButton("清空");

    m_pAffirmPB = new QPushButton("确认");
    m_pCanclePB = new QPushButton("取消");

    m_pFriendsSA = new QScrollArea;
    m_pFriendsWid = new QWidget;
    m_pFriendsVBL = new QVBoxLayout(m_pFriendsWid);
    m_pFriendsBG = new QButtonGroup(m_pFriendsWid);
    m_pFriendsBG->setExclusive(false);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addStretch();
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancleSelectPB);

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pAffirmPB);
    pDownHBL->addWidget(m_pCanclePB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pFriendsSA); // SA中放置Wid，Wid是BG的父类
    pMainVBL->addLayout(pDownHBL);
    setLayout(pMainVBL);

    connect(m_pSelectAllPB, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
    connect(m_pCancleSelectPB, SIGNAL(clicked(bool)), this, SLOT(cancelSelect()));
    connect(m_pAffirmPB, SIGNAL(clicked(bool)), this, SLOT(affirmShare()));
    connect(m_pCanclePB, SIGNAL(clicked(bool)), this, SLOT(cancelSelect()));
}

void sharedFileFriendList::updateFriendList(QListWidget *pFriendList)
{
    if (pFriendList == nullptr)
    {
        return;
    }
    QList<QAbstractButton *> preFriendList = m_pFriendsBG->buttons();
    for (QAbstractButton *pItem : preFriendList)
    {
        m_pFriendsVBL->removeWidget(pItem);
        m_pFriendsBG->removeButton(pItem);
        delete pItem;
        pItem = nullptr;
    }
    QCheckBox *pCB = nullptr;
    for (int i = 0; i < pFriendList->count(); i++)
    {
        qDebug() << "好友：" << pFriendList->item(i)->text();
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendsVBL->addWidget(pCB);
        m_pFriendsBG->addButton(pCB);
    }
    m_pFriendsSA->setWidget(m_pFriendsWid);
}
void sharedFileFriendList::cancelSelect()
{
    QList<QAbstractButton *> friendsButtons = m_pFriendsBG->buttons();

    for (QAbstractButton *pItem : friendsButtons)
    {
        pItem->setChecked(false);
    }
}
void sharedFileFriendList::affirmShare()
{
    QString strFileName = OperateWidget ::getInstance().getPFileSystem()->getStrShareFileName();
    QString strFilePath = OperateWidget::getInstance().getPFileSystem()->getStrShareFilePath();

    QList<QAbstractButton *> abList = m_pFriendsBG->buttons();
    QList<QString> userList;
    for (int i = 0; i < abList.count(); i++)
    {
        if (abList[i]->isChecked())
        {
            userList.append(abList[i]->text().split('\t')[0]);
        }
    }
    int iUserNum = userList.count();
    qDebug() << "分享好友：" << userList << " " << iUserNum;
    PDU *pdu = mkPDU(strFilePath.size() + userList.count() * 32 + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    for (int i = 0; i < iUserNum; i++)
    {
        strncpy((char *)(pdu->caMsg) + 32 * i, userList[i].toStdString().c_str(), 32);
    }
    memcpy((char *)(pdu->caMsg) + 32 * iUserNum, strFilePath.toStdString().c_str(), strFilePath.size());
       sprintf(pdu -> caData, "%s %d", strFileName.toStdString().c_str(), iUserNum);
    Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
    this->hide();
}
void sharedFileFriendList::cancleShare()
{
    this->hide();
}
void sharedFileFriendList::selectAll()
{
    QList<QAbstractButton *> friendButtons = m_pFriendsBG->buttons();
    for (QAbstractButton *pItem : friendButtons)
    {
        pItem->setChecked(true);
    }
}
