#include "operatewidget.h"
#include <qstackedwidget.h>
OperateWidget::OperateWidget(QWidget *parent)
{
    m_pUserLabel = new QLabel;
    m_pListWidget = new QListWidget(this);
    m_pListWidget->addItem("好友");
    m_pListWidget->addItem("文件");

    m_pFriend = new Friend;
    m_pFileSystem = new FileSystem;

    m_pStackedWidget = new QStackedWidget;
    m_pStackedWidget->addWidget(m_pFriend);
    m_pStackedWidget->addWidget(m_pFileSystem);

    QHBoxLayout *pLayout = new QHBoxLayout;
    pLayout->addWidget(m_pListWidget);
    pLayout->addWidget(m_pStackedWidget);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addWidget(m_pUserLabel);
    pMainVBL->addLayout(pLayout);
    setLayout(pMainVBL);

    connect(m_pListWidget, SIGNAL(currentRowChanged(int)), // 函数参数为改变后的行号
            m_pStackedWidget, SLOT(setCurrentIndex(int)));
}
OperateWidget &OperateWidget::getInstance()
{
    static OperateWidget instance;
    return instance;
}

Friend *OperateWidget::getPFriend() const
{
    return m_pFriend;
}

void OperateWidget::setUserLabel(const char *name)
{
    m_pUserLabel->setText(name);
}

FileSystem *OperateWidget::getPFileSystem() const
{
    return m_pFileSystem;
}
