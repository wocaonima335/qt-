#include "dboperate.h"

#include <QMessageBox>
#include <QDebug>
#include <QSqlError>

DBoperate::DBoperate(QObject *parent) : QObject(parent)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

DBoperate &DBoperate::getInstacne()
{
    static DBoperate instance;
    return instance;
}

void DBoperate::init()
{
    m_db.setHostName("localhost");
    m_db.setDatabaseName("networkdiskdb.db");
    m_db.setUserName("root");
    m_db.setPassword("abc15497");
    if (m_db.open())
    {
        qDebug() << "数据库打开";
        QSqlQuery query;
        if (!query.exec("select * from userInfo"))
        {
            // 处理查询执行失败的情况
            qDebug() << "Query execution failed: " << query.lastError().text();
        }
        query.exec("select*from userInfo");
        while (query.next())
        {
            QString data = QString("%1, %2, %3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
            qDebug() << data;
        }
    }
    else
    {
        QMessageBox::critical(nullptr, "数据库打开", "数据库打开失败");
    }
}

DBoperate::~DBoperate()
{
    m_db.close();
}

bool DBoperate::handleRegist(const char *name, const char *pwd)
{
    if (name == nullptr || pwd == nullptr)
    {
        return false;
    }
    QString strQuery = QString("insert into userinfo(name, pwd,online) values(\'%1\', \'%2\',0)").arg(name).arg(pwd);
    QSqlQuery query;

    // qDebug() << strQuery;
    if (!query.exec(strQuery))
    {
        // 处理查询执行失败的情况
        qDebug() << "Query execution failed: " << query.lastError().text();
        return false;
    } // 数据库中name索引是unique，所以如果name重复会返回false，插入成功返回true
    return true;
}

bool DBoperate::handleLogin(const char *name, const char *pwd)
{

    if (name == nullptr || pwd == nullptr)
    {
        qDebug() << "name || pwd is NULL";
        return false;
    }
    QString strquery = QString("select * from userInfo where name= '%1' and pwd= '%2' and online= 0").arg(name).arg(pwd);
    QSqlQuery query;
    qDebug() << strquery;
    if (!query.exec(strquery))
    {
        qDebug() << "Query execution failed: " << query.lastError().text();
    }
    if (query.next())
    {
        strquery = QString("update userInfo set online=1 where name='%1'").arg(name);
        return query.exec(strquery);
    }
    else
    {
        return false;
    }
}

bool DBoperate::handleoffline(const char *name)
{
    if (name == nullptr)
    {
        qDebug() << "name == nullptr";
        return false;
    }
    QString strquery = QString("update userInfo set online = 0 where name = \'%1\' ").arg(name);
    QSqlQuery query;
    if (!query.exec(strquery))
    {
        // 处理查询执行失败的情况
        qDebug() << "Query execution failed: " << query.lastError().text();
        return false;
    }
    if (query.next())
    {
        return true;
    }
    else
    {
        return false;
    }
}

int DBoperate::handleSearchUser(const char *name)
{

    if (NULL == name)
    {
        return 2;
    }
    QString strQuery = QString("select online from userInfo where name = \'%1\' ").arg(name);
    QSqlQuery query;

    query.exec(strQuery);
    if (query.next())
    {
        return query.value(0).toInt(); // 存在并在线返回1，存在不在线返回0
    }
    else
    {
        return 2; // 不存在该用户
    }
}

int DBoperate::haneleAddFriend(const char *addName, const char *sourceName)
{
    if (addName == nullptr || sourceName == nullptr)
    {
        qDebug() << "addName == nullptr || sourceName == nullptr";
        return 4;
    }
    QString strquery = QString("select * from friendInfo where (id=(select id from userInfo where name = '%1') and friendId=(select id from userInfo where name = '%2'))");
    QSqlQuery query;
    if (!query.exec(strquery.arg(addName).arg(sourceName)))
    {
        qDebug() << "Query execution failed: " << query.lastError().text();
        return 4;
    }
    if (query.next())
    {
        return 3;
    }
    else
    {
        return handleSearchUser(addName);
    }
}

bool DBoperate::handleAddFriendAgree(const char *addName, const char *sourceName)
{
    if (addName == nullptr || sourceName == nullptr)
    {
        qDebug() << "addName == nullptr || sourceName == nullptr";
        return false;
    }
    int sourceId = getIdByUserName(sourceName);
    int addId = getIdByUserName(addName);
    QString strQuery = QString("insert into friendInfo values(%1, %2) ").arg(sourceId).arg(addId);
    QSqlQuery query;
    QString strQuery1 = QString("insert into friendInfo values(%1, %2) ").arg(addId).arg(sourceId);
    qDebug() << "handleAddFriendAgree " << strQuery;
    qDebug() << "handleAddFriendAgree " << strQuery1;
    return query.exec(strQuery) && query.exec(strQuery1);
}

int DBoperate::getIdByUserName(const char *name)
{
    if (name == nullptr)
    {
        return -1;
    }
    QString strquery = QString("select id from userInfo where name = \'%1\'").arg(name);
    QSqlQuery query;
    if (!query.exec(strquery))
    {
        qDebug() << "Query execution failed: " << query.lastError().text();
        return 4;
    }
    if (query.next())
    {
        return query.value(0).toInt();
    }
    else
    {
        return -1;
    }
}

bool DBoperate::handleDeleteFriend(const char *deleteName, const char *sourceName)
{
    if (deleteName == nullptr || sourceName == nullptr)
    {
        qDebug() << "deleteName == nullptr || sourceName == nullptr";
        return false;
    }
    int deleteId = getIdByUserName(deleteName);
    int sourceId = getIdByUserName(sourceName);
    QString strQuery = QString("delete from friendInfo where id1 = %1 and id2 = %2").arg(deleteId).arg(sourceId);
    QString strQuery1 = QString("delete from friendInfo where id1 = %1 and id2 = %2").arg(sourceId).arg(deleteId);
    QSqlQuery query;
    return query.exec(strQuery) && query.exec(strQuery1);
}

QStringList DBoperate::handleFlushFriendRequest(const char *name)
{
    QStringList strFriendList;
    if (name == nullptr)
    {
        return strFriendList;
    }
    int nameId = getIdByUserName(name);
    QString strQuery = QString("select name,online from userInfo where id in (select friendId from friendInfo where id = %1)").arg(nameId);
    QSqlQuery query;
    if (!query.exec(strQuery))
    {
        qDebug() << "Query execution failed: " << query.lastError().text();
        return strFriendList;
    }
    else
    {
        while (query.next())
        {
            char frinedName[32];
            char friendOnline[4];
            strncpy(frinedName, query.value(0).toString().toStdString().c_str(), 32);
            strncpy(friendOnline, query.value(1).toString().toStdString().c_str(), 4);
            strFriendList.append(frinedName);
            strFriendList.append(friendOnline);
            qDebug() << "好友信息：" << frinedName << " " << friendOnline;
            qDebug() << strFriendList;
        }
    }
    return strFriendList;
}

QStringList DBoperate::handleOnlineUsers()
{
    QStringList strOnlineUsers;
    QString stquery = QString("select name from userInfo where online = '1'");
    QSqlQuery query;
    if (!query.exec(stquery))
    {
        qDebug() << "Query execution failed: " << query.lastError().text();
        return strOnlineUsers;
    }
    else
    {
        while (query.next())
        {
            char name[32];
            strncpy(name, query.value(0).toString().toStdString().c_str(), 32);
            strOnlineUsers.append(name);
            qDebug() << "在线用户：" << name;
            qDebug() << strOnlineUsers;
        }
    }
    return strOnlineUsers;
}
