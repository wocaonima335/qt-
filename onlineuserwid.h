#ifndef ONLINEUSERWID_H
#define ONLINEUSERWID_H

#include <QWidget>
#include "protocol.h"

namespace Ui
{
    class onlineuserwid;
}

class onlineuserwid : public QWidget
{
    Q_OBJECT

public:
    explicit onlineuserwid(QWidget *parent = nullptr);
    ~onlineuserwid();
    void setOnlineUsers(PDU *pdu);
private slots:
    void on_addfriend_pb_clicked();

private:
    Ui::onlineuserwid *ui;
};

#endif // ONLINEUSERWID_H
