#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void loadCofig();
    static Widget &getInstance();
    QTcpSocket &getTcpSocket();

    void setStrName(const QString &strName);
    QString getStrName() const;

    QString getStrCurPath() const;
    void setStrCurPath(const QString &strCurPath);

    QString getStrRootPath() const;
    void setStrRootPath(const QString &strRootPath);
private slots:
    void on_login_pb_clicked();
    void on_regist_pb_clicked();
    void on_logout_pb_clicked();
public slots:
    void showConncet();
    void receiveMsg();

private:
    Ui::Widget *ui;
    QString m_strIP;        // 存储配置文件读取到的IP地址
    quint16 m_usPort;       // 无符号16位整型 存储配置文件的端口号
    QTcpSocket m_tcpSocket; // TCPSocket用来和服务器连接与交互

    QString m_strName;     // 该客户端用户名
    QString m_strRootPath; // 用户根目录
    QString m_strCurPath;  // 当前目录
};
#endif // WIDGET_H
