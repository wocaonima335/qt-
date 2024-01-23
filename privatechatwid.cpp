#include "privatechatwid.h"
#include "ui_privatechatwid.h"
#include <QMessageBox>
#include "widget.h"
PrivateChatWid::PrivateChatWid(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::PrivateChatWid)
{
    ui->setupUi(this);
}

PrivateChatWid::~PrivateChatWid()
{
    delete ui;
}

QString PrivateChatWid::strChatName() const
{
    return m_strChatName;
}

QString PrivateChatWid::strLoginName() const
{
    return m_strLoginName;
}

void PrivateChatWid::setStrLoginName(const QString &strLoginName)
{
    m_strLoginName = strLoginName;
}

void PrivateChatWid::updateShowMsgTE(PDU *pdu)
{
    if (pdu == nullptr)
    {
        return;
    }
    char caSendName[32] = {'\0'};
    strncpy(caSendName, pdu->caData + 32, 32);
    QString strMsg = QString("%1 : %2").arg(caSendName).arg(QString::fromUtf8(reinterpret_cast<const char*>(pdu->caMsg)));

    ui->textEdit->append(strMsg);

}

void PrivateChatWid::setPriChatTitle(const char *caTitle)
{
    ui->label->setText(caTitle);
}

void PrivateChatWid::setStrChatName(const QString &strChatName)
{
    m_strChatName = strChatName;
}
void PrivateChatWid::on_sendMsg_pb_clicked()
{
    QString strSendMsg = ui->lineEdit->text();
    if (strSendMsg == nullptr)
    {
        QMessageBox::warning(this, "私聊", "发送消息不能为空");
        return;
    }
    ui->lineEdit->clear();
    ui->textEdit->append(QString("%1:%2").arg(m_strLoginName).arg(strSendMsg));

    PDU *pdu = mkPDU(strSendMsg.size() + 1);
    pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
    strncpy(pdu->caData, m_strChatName.toStdString().c_str(), 32);//目标用户名
    strncpy(pdu->caData + 32, m_strLoginName.toStdString().c_str(), 32);//请求用户名
    strncpy((char*)pdu -> caMsg, strSendMsg.toStdString().c_str(), strSendMsg.size());
    Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}
