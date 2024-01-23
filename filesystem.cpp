#include "filesystem.h"
#include <QInputDialog>
#include "widget.h"
#include <QMessageBox>
#include <qdialog.h>
#include <QFileDialog>
#include"operatewidget.h"
FileSystem::FileSystem(QWidget *parent) : QWidget(parent)
{
        m_strTryEntryDir.clear();
        m_strUploadFilePath.clear();
        m_pTimer = new QTimer;
        m_pDownloadFileInfo = new TransFile;
        m_pDownloadFileInfo->bTransForm = false;
        m_pFileListW = new QListWidget;

        m_ReturnPreBtn = new QPushButton("返回");
        m_pCreateDirBtn = new QPushButton("创建文件夹");
        m_pDelFileOrDirBtn = new QPushButton("删除");
        m_pFlushDirBtn = new QPushButton("刷新文件夹");
        m_pRenameFileBtn = new QPushButton("重命名");

        QHBoxLayout *pDirOpVBL = new QHBoxLayout;
        pDirOpVBL->addWidget(m_ReturnPreBtn);
        pDirOpVBL->addWidget(m_pCreateDirBtn);
        pDirOpVBL->addWidget(m_pDelFileOrDirBtn);
        pDirOpVBL->addWidget(m_pFlushDirBtn);
        pDirOpVBL->addWidget(m_pRenameFileBtn);

        m_pUploadFileBtn = new QPushButton("上传文件");
        m_pDownloadFileBtn = new QPushButton("下载文件");
        m_pShareFileBtn = new QPushButton("分享文件");
        m_pMoveFileBtn = new QPushButton("移动文件");
        m_pMoveDesDirBtn = new QPushButton("目标目录");
        m_pMoveDesDirBtn->setEnabled(false);

        QHBoxLayout *pFileOpVBL = new QHBoxLayout;
        pFileOpVBL->addWidget(m_pUploadFileBtn);
        pFileOpVBL->addWidget(m_pDownloadFileBtn);
        pFileOpVBL->addWidget(m_pShareFileBtn);
        pFileOpVBL->addWidget(m_pMoveFileBtn);
        pFileOpVBL->addWidget(m_pMoveDesDirBtn);

        QVBoxLayout *pMainVBL = new QVBoxLayout;
        pMainVBL->addLayout(pDirOpVBL);
        pMainVBL->addLayout(pFileOpVBL);
        pMainVBL->addWidget(m_pFileListW);

        setLayout(pMainVBL);

        m_pSharedFileFLW = new sharedFileFriendList;
        connect(m_pCreateDirBtn, SIGNAL(clicked(bool)),
                this, SLOT(createDir()));
        connect(m_pFlushDirBtn, SIGNAL(clicked(bool)),
                this, SLOT(flushDir()));
        connect(m_pDelFileOrDirBtn, SIGNAL(clicked(bool)),
                this, SLOT(delFileOrDir()));
        connect(m_pRenameFileBtn, SIGNAL(clicked(bool)),
                this, SLOT(renameFile()));
        connect(m_pFileListW, SIGNAL(doubleClicked(QModelIndex)),
                this, SLOT(entryDir(QModelIndex)));
        connect(m_ReturnPreBtn, SIGNAL(clicked(bool)),
                this, SLOT(returnPreDir()));
        connect(m_pUploadFileBtn, SIGNAL(clicked(bool)),
                this, SLOT(uploadFile()));
        connect(m_pTimer, SIGNAL(timeout()), // 时间间隔之后再上传文件，防止粘包
                this, SLOT(uploadFileData()));
        connect(m_pDownloadFileBtn, SIGNAL(clicked(bool)),
                this, SLOT(downloadFile()));
        connect(m_pMoveFileBtn, SIGNAL(clicked(bool)),
                this, SLOT(moveFile()));
        connect(m_pMoveDesDirBtn, SIGNAL(clicked(bool)),
                this, SLOT(moveDesDir()));
        connect(m_pShareFileBtn, SIGNAL(clicked(bool)),
                this, SLOT(shareFile()));
}

void FileSystem::updateFileList(PDU *pdu)
{
        if (pdu == nullptr)
        {
                return;
        }
        uint uiFileNum = pdu->uiMsgLen / sizeof(FileInfo);
        FileInfo *pFileInfo = nullptr;
        QListWidgetItem *pItem = nullptr;

        m_pFileListW->clear();
        for (uint i = 0; i < uiFileNum; i++)
        {
                pFileInfo = (FileInfo *)(pdu->caMsg) + i;
                if (strcmp(pFileInfo->caName, ".") == 0 || strcmp(pFileInfo->caName, "..") == 0)
                {
                        continue;
                }
                pItem = new QListWidgetItem;
                if (pFileInfo->bIsDir)
                {
                        pItem->setIcon(QIcon(":/images/open_folder.png"));
                }
                else
                {
                        pItem->setIcon(QIcon(":/images/file.jpeg"));
                }
                pItem->setText(QString("%1\t%2\t%3").arg(pFileInfo->caName).arg(pFileInfo->uisize).arg(pFileInfo->caTime));
                m_pFileListW->addItem(pItem);
        }
}

QString FileSystem::strTryEntryDir() const
{
        return m_strTryEntryDir;
}

void FileSystem::setStrTryEntryDir(const QString &str)
{
        m_strTryEntryDir = str;
        return;
}

void FileSystem::clearStrTryEntryDir()
{
        m_strTryEntryDir.clear();
        return;
}

QString FileSystem::getStrShareFilePath() const
{
        return m_strSharedFilePath;
}

QString FileSystem::getStrShareFileName() const
{
        return m_strSharedFileName;
}
void FileSystem::flushDir()
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_REQUEST;
        memcpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
}
void FileSystem::delFileOrDir()
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        QListWidgetItem *qItem = m_pFileListW->currentItem();
        if (qItem == nullptr)
        {
                QMessageBox::warning(this, "警告", "请选择要删除的文件或文件夹！");
                return;
        }
        QString strFileName = qItem->text().split('\t')[0];
        QString strFilePath = QString("%1/%2").arg(strCurPath).arg(strFileName);
        PDU *pdu = mkPDU(strFilePath.size() + 1);

        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        memcpy((char *)pdu->caMsg, strFilePath.toStdString().c_str(), strFilePath.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
}
void FileSystem::renameFile()
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        QListWidgetItem *qItem = m_pFileListW->currentItem();
        if (qItem == nullptr)
        {
                QMessageBox::warning(this, "警告", "请选择要重命名的文件或文件夹！");
                return;
        }
        QString strOldName = qItem->text().split('\t')[0];
        QString strNewName = QInputDialog::getText(this, "重命名", "新文件名：");
        qDebug() << "重命名：" << strCurPath << " " << strOldName << " -> " << strNewName;
        if (strNewName.isEmpty())
        {
                QMessageBox::warning(this, "警告", "请输入新文件名！");
                return;
        }
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUEST;

        strncpy(pdu->caData, strOldName.toStdString().c_str(), strOldName.size());
        strncpy(pdu->caData + 32, strNewName.toStdString().c_str(), strNewName.size());
        memcpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
        return;
}
void FileSystem::entryDir(const QModelIndex &index)
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        QString strDirName = index.data().toString();
        strDirName = strDirName.split('\t')[0];
        QString strEnTryPath = QString("%1/%2").arg(strCurPath).arg(strDirName);
        qDebug() << "进入" << strEnTryPath;
        m_strTryEntryDir = strEnTryPath;
        PDU *pdu = mkPDU(strEnTryPath.size() + 1);

        pdu->uiMsgType = ENUM_MSG_TYPE_PRE_DIR_REQUEST;
        memcpy((char *)pdu->caMsg, strEnTryPath.toStdString().c_str(), strEnTryPath.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
}
void FileSystem::returnPreDir()
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        QString strRootPath = Widget::getInstance().getStrRootPath();
        if (strCurPath == strRootPath)
        {
                QMessageBox::warning(this, "返回上一目录", "已经是根目录");
                return;
        }
        int index = strCurPath.lastIndexOf('/');
        strCurPath = strCurPath.remove(index, strCurPath.size() - index);
        qDebug() << "返回到" << strCurPath;
        m_strTryEntryDir = strCurPath;

        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRE_DIR_REQUEST;
        memcpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = nullptr;
}
void FileSystem::uploadFile()
{
        QString strCurPath = Widget::getInstance().getStrCurPath();
        m_strUploadFilePath = QFileDialog::getOpenFileName();
        qDebug() << m_strUploadFilePath;
        if (m_strUploadFilePath.isEmpty())
        {
                QMessageBox::warning(this, "上传文件", "选择需要上传的文件！");
                return;
        }
        int index = m_strUploadFilePath.lastIndexOf('/');
        QString strFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index - 1);

        QFile file(m_strUploadFilePath);
        qint64 fileSize = file.size();
        qDebug() << "上传文件" << strFileName << " " << fileSize;
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        sprintf(pdu->caData, "%s %lld", strFileName.toStdString().c_str(), fileSize);

        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
}
void FileSystem::startTimer()
{
        m_pTimer->start(1000);
}
void FileSystem::uploadFileData()
{
    m_pTimer->stop(); // 关闭定时器，不然定时器会重新计时

    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)) // 只读形式打开文件
    {
        // 打开失败
        QMessageBox::warning(this, "打开文件", "打开文件失败！");
    }
    // 二进制形式传输文件
    char *pBuffer = new char[4096]; // 4096个字节读写效率更高
    qint64 iActualSize = 0;         // 实际读取文件内容大小

    while(true)
    {
        iActualSize = file.read(pBuffer, 4096); // 读数据，返回值是实际读取数据大小
        if (iActualSize > 0 && iActualSize <= 4096)
        {
            Widget::getInstance().getTcpSocket().write(pBuffer, iActualSize);
        }
        else if (iActualSize == 0)
        { // 发送完成
            break;
        }
        else
        {
            QMessageBox::warning(this, "上传文件", "上传失败！");
            break;
        }
    }
    file.close();
    delete [] pBuffer;
    pBuffer = NULL;
    m_strUploadFilePath.clear(); // 清除上传文件夹名，以免影响之后上传操作
}
void FileSystem::downloadFile()
{
        QListWidgetItem *pItem = m_pFileListW->currentItem();
        if (pItem == nullptr)
        {
                QMessageBox::warning(this, "下载文件", "请选择需要下载的文件！");
                return;
        }
        QString strDownloadFilePath = QFileDialog::getSaveFileName();
        if (strDownloadFilePath.isEmpty())
        {
                QMessageBox::warning(this, "下载文件", "请指定下载文件的位置");
                m_pDownloadFileInfo->file.setFileName("");
                return;
        }
        m_pDownloadFileInfo->file.setFileName(strDownloadFilePath);

        QString strCurPath = Widget::getInstance().getStrCurPath();
        QString strFileName = pItem->text().split('\t')[0];
        PDU *pdu = mkPDU(strCurPath.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        memcpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
        strncpy(pdu->caData, strFileName.toStdString().c_str(), strFileName.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        qDebug() << "下载文件" << pdu->caData;

        free (pdu);
        pdu = nullptr;
}
void FileSystem::moveFile()
{
        QListWidgetItem *pItem = m_pFileListW->currentItem();
        if (pItem == nullptr)
        {
                QMessageBox::warning(this, "移动文件", "请选择需要移动的文件！");
                return;
        }
        m_strMoveFileName = pItem->text().split('\t')[0];
        m_strMoveOldDir = Widget::getInstance().getStrCurPath();

        m_pMoveDesDirBtn->setEnabled(true);
        QMessageBox::information(this, "移动文件", "请跳转到需要移动到的目录，\n然后点击“目标目录”按钮。");
}

void FileSystem::moveDesDir()
{
        QString strDesDir = Widget::getInstance().getStrCurPath();
        QMessageBox::StandardButton sbMoveAffirm;
        QString strMoveAffirm = QString("你确认将%1 的 %2文件\n移动到%3目录下吗？").arg(m_strMoveOldDir).arg(m_strMoveFileName).arg(strDesDir);
        sbMoveAffirm = QMessageBox::question(this, "移动文件", strMoveAffirm);
        if (sbMoveAffirm == QMessageBox::No)
        {
                m_strMoveOldDir.clear();
                m_strMoveFileName.clear();
                m_pMoveDesDirBtn->setEnabled(false);
                return;
        }
        qDebug() << "移动文件：" << strMoveAffirm;
        PDU *pdu = mkPDU(strDesDir.size() + m_strMoveOldDir.size() + 5);
        pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf((char *)pdu->caMsg, "%s %s", strDesDir.toStdString().c_str(), m_strMoveOldDir.toStdString().c_str());
        sprintf(pdu->caData, "%s %d %d", m_strMoveFileName.toStdString().c_str(), strDesDir.size(), m_strMoveOldDir.size());
        Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
        free (pdu);
        pdu = nullptr;
        m_strMoveOldDir.clear();
        m_strMoveFileName.clear();
        m_pMoveDesDirBtn->setEnabled(false);
}

void FileSystem::shareFile()
{
        QListWidgetItem *pFileItem = m_pFileListW->currentItem();
        if (pFileItem == nullptr)
        {
                QMessageBox::warning(this, "分享文件", "请选择需要分享的文件");
                return;
        }
        m_strSharedFileName = pFileItem->text().split('\t')[0];
        m_strSharedFilePath = QString("%1/%2").arg(Widget::getInstance().getStrCurPath()).arg(m_strSharedFileName);
        qDebug() << "分享文件：" << m_strSharedFilePath;

        // 获得好友列表
        QListWidget *friendLW = OperateWidget::getInstance().getPFriend()->getPFriend();
        // 选择好友窗口展示
        m_pSharedFileFLW=new sharedFileFriendList();
        m_pSharedFileFLW->updateFriendList(friendLW);
        if (m_pSharedFileFLW->isHidden())
        {
                m_pSharedFileFLW->show();
        }
}

TransFile *FileSystem::getDownloadFileInfo()
{
        return m_pDownloadFileInfo;
}

void FileSystem::createDir()
{
        QString strDirName = QInputDialog::getText(this, "新建文件夹", "文件夹名：");
        QString strCurPath = Widget::getInstance().getStrCurPath();

        if (!strDirName.isEmpty())
        {
                PDU *pdu = mkPDU(strCurPath.size() + 1);
                pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
                strncpy(pdu->caData, strDirName.toStdString().c_str(), strDirName.size());
                memcpy((char *)pdu->caMsg, strCurPath.toStdString().c_str(), strCurPath.size());
                Widget::getInstance().getTcpSocket().write((char *)pdu, pdu->uiPDULen);
                free (pdu);
                pdu = NULL;
        }
}
