#ifndef PRIVATECHATWID_H
#define PRIVATECHATWID_H

#include <QWidget>
#include "protocol.h"
namespace Ui
{
    class PrivateChatWid;
}

class PrivateChatWid : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChatWid(QWidget *parent = nullptr);
    ~PrivateChatWid();
    QString strChatName() const;
    void setStrChatName(const QString &strChatName);

    QString strLoginName() const;
    void setStrLoginName(const QString &m_strLoginName);

    void updateShowMsgTE(PDU *pdu);
    void setPriChatTitle(const char *caTitle);

private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChatWid *ui;
    QString m_strChatName;
    QString m_strLoginName;
};

#endif // PRIVATECHATWID_H
