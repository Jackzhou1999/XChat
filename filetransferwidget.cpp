#include "filetransferwidget.h"

FileTransferWidget::FileTransferWidget(ChatRole role, QWidget *parent)
    : QWidget(parent), m_uploadProgress(0), m_currentProgress(0), m_paused(false), m_role(role){
    // 创建自定义进度条
    setFixedHeight(15);
    setMinimumWidth(350);
    setMaximumWidth(400);
    setContentsMargins(0,0,0,0);
    m_progressBar = new ProgressBar(this);
    // 创建按钮
    m_button = new QPushButton(this);
    m_button->setStyleSheet("border: none; border-image: url(:/res/stop.png);");
    m_button->setFixedSize(15, 15); // 设置按钮大小

    // 布局管理
    QHBoxLayout *layout = new QHBoxLayout(this);
    if (m_role == Self) {
        QSpacerItem *sp_right = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);
        QSpacerItem *sp_left = new QSpacerItem(30, 20, QSizePolicy::Expanding, QSizePolicy::Maximum);
        layout->addItem(sp_left);
        layout->addWidget(m_progressBar);
        layout->addWidget(m_button);
        layout->addItem(sp_right);
        layout->setSpacing(9);
        layout->setContentsMargins(0, 0, 0, 0);
    }else{
        QSpacerItem *sp_right = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);
        QSpacerItem *sp_left = new QSpacerItem(30, 20, QSizePolicy::Expanding, QSizePolicy::Maximum);
        layout->addItem(sp_left);
        layout->addWidget(m_progressBar);
        layout->addWidget(m_button);
        layout->addItem(sp_right);
        layout->setSpacing(9);
        layout->setContentsMargins(0, 0, 0, 0);
    }
    // 定时器模拟文件传输
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &FileTransferWidget::onUploadProgress);
    m_timer->start(1); // 每 100ms 更新一次

    // 按钮点击切换状态
    connect(m_button, &QPushButton::clicked, this, &FileTransferWidget::togglePauseResume);
}

void FileTransferWidget::slot_setCurrentProgress(int progress){
    if(progress > 100) progress = 100;
    if(progress < 0) progress = 0;
    m_currentProgress = progress;
}

void FileTransferWidget::onUploadProgress() {
    if (m_paused) return; // 暂停时不更新
    if(m_uploadProgress < m_currentProgress){
        m_uploadProgress += 2;
        if (m_uploadProgress > 100) m_uploadProgress = 100;
        m_progressBar->setProgress(m_uploadProgress);
    }

    if (m_uploadProgress == 100) {
        m_timer->stop();
        m_button->setStyleSheet("border: none");
        m_button->setDisabled(true);
    }
}

void FileTransferWidget::togglePauseResume() {
    if (m_paused) {
        emit sig_continue();
        m_paused = false;
        m_button->setStyleSheet("border: none; border-image: url(:/res/stop.png);");
    } else {
        emit sig_stop();
        m_paused = true;
        m_button->setStyleSheet("border: none; border-image: url(:/res/run.png);");
    }
}
