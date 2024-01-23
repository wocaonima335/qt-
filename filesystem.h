#ifndef FILESYSTEM_H
#define FILESYSTEM_H


#include <QWidget>
#include <QListWidget>
#include<QListWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>
#include "sharedfilefriendlist.h"
class FileSystem : public QWidget
{
    Q_OBJECT
public:
    explicit FileSystem(QWidget *parent = nullptr);
    void updateFileList(PDU *pdu);
    QString strTryEntryDir() const;
    void setStrTryEntryDir(const QString &str);
    void clearStrTryEntryDir();

    QString getStrShareFilePath() const;
    QString getStrShareFileName() const;

public slots:
    void createDir();
    void flushDir();
    void delFileOrDir();
    void renameFile();
    void entryDir(const QModelIndex &index);
    void returnPreDir();
    void uploadFile();
    void startTimer();
    void uploadFileData(); // 上传文件实际数
    void downloadFile();
    void moveFile();
    void moveDesDir();
    void shareFile();
    TransFile *getDownloadFileInfo();
signals:
private:
    QListWidget *m_pFileListW;

    QPushButton *m_ReturnPreBtn;
    QPushButton *m_pCreateDirBtn;
    QPushButton *m_pFlushDirBtn;
    QPushButton *m_pDelFileOrDirBtn;

    QPushButton *m_pRenameFileBtn;
    QPushButton *m_pUploadFileBtn;
    QPushButton *m_pDownloadFileBtn;
    QPushButton *m_pShareFileBtn;

    QPushButton *m_pMoveFileBtn;
    QPushButton *m_pMoveDesDirBtn;

    QString m_strTryEntryDir;
    QString m_strUploadFilePath;
    QTimer *m_pTimer;

    TransFile *m_pDownloadFileInfo;

    QString m_strMoveFileName;
    QString m_strMoveOldDir;

    QString m_strSharedFilePath;
    QString m_strSharedFileName;
    sharedFileFriendList *m_pSharedFileFLW;

signals:
};

#endif // FILESYSTEM_H
