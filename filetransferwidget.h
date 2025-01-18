#ifndef FILETRANSFERWIDGET_H
#define FILETRANSFERWIDGET_H

#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include "progressbar.h"
#include "global.h"

class FileTransferWidget : public QWidget {
    Q_OBJECT

public:
    FileTransferWidget(ChatRole role, QWidget *parent = nullptr);
public slots:
    void slot_setCurrentProgress(int progress);

private slots:
    void onUploadProgress();
    void togglePauseResume();

private:
    ProgressBar *m_progressBar; // 自定义进度条
    QPushButton *m_button;      // 控制按钮
    QTimer *m_timer;            // 模拟上传定时器
    int m_uploadProgress;       // 上传进度
    bool m_paused;              // 暂停状态
    int m_currentProgress;
    ChatRole m_role;
signals:
    void sig_stop();
    void sig_continue();
};

#endif // FILETRANSFERWIDGET_H
