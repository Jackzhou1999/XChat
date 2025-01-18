#ifndef FILEBUBBLE_H
#define FILEBUBBLE_H


#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QPainterPath>
#include "global.h"
#include "filetransferwidget.h"
#include "fileclient.h"

class Bubble : public QWidget {
    Q_OBJECT
public:
    enum class State{
        Start,
        End,
        Stop,
        Continue,
    };
    explicit Bubble(ChatRole role, const QString &fileName, const QString &fileSize, const QString &fileType, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void SetState(State state);

    ChatRole m_role;
    QString m_fileName;
    QString m_fileSize;
    QString m_fileType; // 文件类型（pdf, image, other）
    bool m_downloadState;
    QString m_text;
signals:
    void sig_downloadfile_req();
public slots:
    void slot_stop_event();
    void slot_continue_event();
    void slot_transferFinished();
    void slot_downloadFinished();
};



class FileBubble : public QWidget {
    Q_OBJECT
public:
    FileBubble(ChatRole role, const QString &fileName, const uint64_t fileSize, const QString &fileType, const QString &username, const QPixmap &icon, const QString filePath = "", QWidget *parent = nullptr);
    FileBubble(ChatRole role, const uint64_t fileid, const QString &fileName, const uint64_t fileSize, const QString &fileType, const QString &username, const QPixmap &icon, const QString filePath, QWidget *parent = nullptr);

    void setMyId(QString mid);
    QString myId();
private:
    QString m_id;
    ChatRole m_role;

    uint64_t m_fileid;
    QString m_filename;
    QString m_filepath;
    uint64_t m_fileSize;

    QLabel* m_pNameLabel;
    QLabel* m_pIconLabel;
    Bubble* m_pBubble;
    FileTransferWidget* m_progressBar;
    std::shared_ptr<FileClient> m_fileclient;

private slots:
    void slot_downloadfile_req();
    void slot_stop_event();
    void slot_continue_event();

    void slot_caculateCurrentProgress(int bytes);
    void slot_uploadFinished(uint64_t fileid);
    void slot_downloadFinished(uint64_t fileid);
public slots:
    void slot_startuploadFile(uint64_t fileid, QString filebubbleid);

signals:
    void sig_downloadfile_req(uint64_t fileid, QString filename);

    void sig_stop_uploading_file(uint64_t fileid);
    void sig_stop_downloading_file(uint64_t fileid);

    void sig_continue_uploading_file(uint64_t fileid, QString filepath);
    void sig_continue_downloading_file(uint64_t fileid, QString filename);

    void sig_setCurrentProgress(int);

    void sig_transferFinished();
    void sig_downloadFinished();
};

#endif // FILEBUBBLE_H
