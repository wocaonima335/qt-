#include "onlineuserwid.h"
#include "ui_onlineuserwid.h"
#include "widget.h"
#include <QDebug>
#include <QMessageBox>
#include<QListWidgetItem>
#include<QListWidget>
onlineuserwid::onlineuserwid(QWidget *parent) : QWidget(parent),
                                                ui(new Ui::onlineuserwid)
{
    ui->setupUi(this);
}

onlineuserwid::~onlineuserwid()
{
    delete ui;
}

void onlineuserwid::setOnlineUsers(PDU *pdu)
{
    uint uiSize = pdu->uiMsgLen / 32;
    char caTmp[32];
    ui->onlineuser_lw->clear();
    for (uint i = 0; i < uiSize; i++)
    {
        memcpy(caTmp, (char *)(pdu->caMsg) + 32 * i, 32);
        if (strcmp(caTmp, Widget::getInstance().getStrName().toStdString().c_str()) == 0)
        {
            continue;
        }
        ui->onlineuser_lw->addItem(caTmp);
    }
}

void onlineuserwid::on_addfriend_pb_clicked()
{
    QListWidgetItem *pItem = ui->onlineuser_lw->currentItem();
    if (NULL == pItem)
    {
        QMessageBox::warning(this, "添加好友", "请选择要添加的好友！");
        return;
    }
    QString strAddName = pItem->text();
    QString strLoginName = Widget::getInstance().getStrName();
    PDU *pdu = mkPDU(0);

    qDebug() << "on_addfriend_pb_clicked  " << strAddName << " " << strLoginName;
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, strAddName.toStdString().c_str(), strAddName.size());
    memcpy(pdu->caData + 32, strLoginName.toStdString().c_str(), strLoginName.size());
    Widget::getInstance().getTcpSocket().write((char*)pdu, pdu -> uiPDULen);
    free(pdu);
    pdu = NULL;
}
