#ifndef OPERATEWIDGET_H
#define OPERATEWIDGET_H
#include <QWidget>
#include <QListWidget>
#include "freind.h"
#include "filesystem.h"
#include <QStackedWidget>
#include <QLabel>

class OperateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OperateWidget(QWidget *parent = nullptr);
    static OperateWidget &getInstance();
    Friend *getPFriend() const;
    void setUserLabel(const char *name);
    FileSystem *getPFileSystem() const;

signals:

private:
    QLabel *m_pUserLabel;
    QListWidget *m_pListWidget;
    Friend *m_pFriend;
    FileSystem *m_pFileSystem;
    QStackedWidget *m_pStackedWidget;
};
#endif
