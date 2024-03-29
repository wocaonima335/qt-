#ifndef DBOPERATE_H
#define DBOPERATE_H

#include <QObject>
#include <QSqlDatabase> //连接数据库
#include <QSqlQuery>    //数据库操作
#include <QStringList>
class DBoperate : public QObject
{
    Q_OBJECT
public:
    explicit DBoperate(QObject *parent = nullptr);
    static DBoperate &getInstacne();
    void init();
    ~DBoperate();

    bool handleRegist(const char *name, const char *pwd);                    // 处理注册操作
    bool handleLogin(const char *name, const char *pwd);                     // 处理登入操作
    bool handleoffline(const char *name);                                    // 处理用户下线
    int handleSearchUser(const char *name);                                 // 查找用户
    int haneleAddFriend(const char *addName, const char *sourceName);       // 处理添加用户
    bool handleAddFriendAgree(const char *addName, const char *sourceName);  // 处理同意好友申请
    int getIdByUserName(const char *name);                                   // 根据用户名获取用户id
    bool handleDeleteFriend(const char *deleteName, const char *sourceName); // 删除好友操作
    QStringList handleFlushFriendRequest(const char *name);                  // 刷新好友列表请求
    QStringList handleOnlineUsers();                                         // 处理查询所有在线用户

private:
    QSqlDatabase m_db;
};
#endif