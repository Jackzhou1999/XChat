#include "filebubble.h"
#include <QGridLayout>


static QString getFileSize(qint64 size)
{
    QString Unit;
    double num;
    if(size < 1024){
        num = size;
        Unit = "B";
    }
    else if(size < 1024 * 1224){
        num = size / 1024.0;
        Unit = "KB";
    }
    else if(size <  1024 * 1024 * 1024){
        num = size / 1024.0 / 1024.0;
        Unit = "MB";
    }
    else{
        num = size / 1024.0 / 1024.0/ 1024.0;
        Unit = "GB";
    }
    return QString::number(num,'f',2) + " " + Unit;
}

FileBubble::FileBubble(ChatRole role, const uint64_t fileid, const QString &fileName, const uint64_t fileSize, const QString &fileType, const QString &username, const QPixmap &icon, const QString filePath, QWidget *parent)
    : QWidget(parent), m_role(role), m_fileid(fileid), m_filename(fileName), m_fileSize(fileSize){
    m_fileclient = FileClient::GetInstance();
    m_filepath = filePath;
    auto* mainLayout = new QHBoxLayout(this); // 主水平布局
    mainLayout->setContentsMargins(0, 10, 0, 10); // 调整主边距
    mainLayout->setSpacing(5);

    QLabel* avatar = new QLabel(); // 头像
    avatar->setScaledContents(true);
    avatar->setFixedSize(42, 42);
    avatar->setStyleSheet("border-radius: 5px; background-color: gray;");
    avatar->setPixmap(icon);

    // 用户名和图片的垂直布局
    auto* userContentLayout = new QVBoxLayout();
    userContentLayout->setContentsMargins(0, 0, 0, 0); // 去掉内部边距
    userContentLayout->setSpacing(0); // 减小间距为2像素

    QLabel* nameLabel = new QLabel(username); // 用户名
    nameLabel->setObjectName("chat_user_name");
    QFont font("ubuntu");
    font.setPointSize(9);
    nameLabel->setFont(font);
    nameLabel->setContentsMargins(0, 0, 0, 0); // 减小内部边距
    nameLabel->setFixedHeight(15);

    QString filesize = getFileSize(fileSize);
    m_pBubble = new Bubble(m_role, fileName, filesize, fileType);
    m_progressBar = new FileTransferWidget(m_role);
    m_progressBar->hide();

    userContentLayout->setStretch(0, 1);
    userContentLayout->setStretch(1, 90);
    userContentLayout->setStretch(2, 9);


    if(m_role == ChatRole::Self)
    {
        // 发送方布局：头像在右侧
        mainLayout->addStretch();
        userContentLayout->addWidget(nameLabel, 0, Qt::AlignRight); // 用户名紧贴顶部
        userContentLayout->addWidget(m_pBubble, 0, Qt::AlignRight); // 图片紧贴用户名
        userContentLayout->addWidget(m_progressBar, 0, Qt::AlignRight);
        mainLayout->addLayout(userContentLayout);
        mainLayout->addWidget(avatar, 0, Qt::AlignTop);
    } else {
        // 接收方布局：头像在左侧
        mainLayout->addWidget(avatar, 0, Qt::AlignTop);
        userContentLayout->addWidget(nameLabel, 0, Qt::AlignLeft); // 用户名紧贴顶部
        userContentLayout->addWidget(m_pBubble, 0, Qt::AlignCenter); // 图片紧贴用户名
        userContentLayout->addWidget(m_progressBar, 0, Qt::AlignLeft);
        mainLayout->addLayout(userContentLayout);
        mainLayout->addStretch();
    }
    connect(m_progressBar, &FileTransferWidget::sig_stop, m_pBubble, &Bubble::slot_stop_event);
    connect(m_progressBar, &FileTransferWidget::sig_continue, m_pBubble, &Bubble::slot_continue_event);
    connect(m_progressBar, &FileTransferWidget::sig_stop, this, &FileBubble::slot_stop_event);
    connect(m_progressBar, &FileTransferWidget::sig_continue, this, &FileBubble::slot_continue_event);

    connect(m_fileclient.get(), &FileClient::sig_bytes_transfered, this, &FileBubble::slot_caculateCurrentProgress);
    connect(m_fileclient.get(), &FileClient::sig_transferFinished, this, &FileBubble::slot_uploadFinished);
    connect(m_fileclient.get(), &FileClient::sig_downloadFinished, this, &FileBubble::slot_downloadFinished);

    connect(this, &FileBubble::sig_setCurrentProgress, m_progressBar, &FileTransferWidget::slot_setCurrentProgress);
    connect(this, &FileBubble::sig_transferFinished, m_pBubble, &Bubble::slot_transferFinished);
    connect(this, &FileBubble::sig_downloadFinished, m_pBubble, &Bubble::slot_downloadFinished);

    connect(this, &FileBubble::sig_stop_uploading_file, m_fileclient.get(), &FileClient::slot_stopuploadFile);
    connect(this, &FileBubble::sig_continue_uploading_file, m_fileclient.get(), &FileClient::uploadFile);

    connect(m_pBubble, &Bubble::sig_downloadfile_req, this, &FileBubble::slot_downloadfile_req);
    connect(this, &FileBubble::sig_downloadfile_req, m_fileclient.get(), &FileClient::downloadFile);
}

FileBubble::FileBubble(ChatRole role, const QString &fileName, const uint64_t fileSize, const QString &fileType, const QString &username, const QPixmap &icon, const QString filePath, QWidget *parent)
    : QWidget(parent), m_role(role), m_filename(fileName), m_fileSize(fileSize){
    m_fileclient = FileClient::GetInstance();
    m_filepath = filePath;
    auto* mainLayout = new QHBoxLayout(this); // 主水平布局
    mainLayout->setContentsMargins(0, 10, 0, 10); // 调整主边距
    mainLayout->setSpacing(5);

    QLabel* avatar = new QLabel(); // 头像
    avatar->setScaledContents(true);
    avatar->setFixedSize(42, 42);
    avatar->setStyleSheet("border-radius: 5px; background-color: gray;");
    avatar->setPixmap(icon);

    // 用户名和图片的垂直布局
    auto* userContentLayout = new QVBoxLayout();
    userContentLayout->setContentsMargins(0, 0, 0, 0); // 去掉内部边距
    userContentLayout->setSpacing(0); // 减小间距为2像素

    QLabel* nameLabel = new QLabel(username); // 用户名
    nameLabel->setObjectName("chat_user_name");
    QFont font("ubuntu");
    font.setPointSize(9);
    nameLabel->setFont(font);
    nameLabel->setContentsMargins(0, 0, 0, 0); // 减小内部边距
    nameLabel->setFixedHeight(15);

    QString filesize = getFileSize(fileSize);
    m_pBubble = new Bubble(m_role, fileName, filesize, fileType);
    m_progressBar = new FileTransferWidget(m_role);
    m_progressBar->hide();

    userContentLayout->setStretch(0, 1);
    userContentLayout->setStretch(1, 90);
    userContentLayout->setStretch(2, 9);


    if(m_role == ChatRole::Self)
    {
        // 发送方布局：头像在右侧
        mainLayout->addStretch();
        userContentLayout->addWidget(nameLabel, 0, Qt::AlignRight); // 用户名紧贴顶部
        userContentLayout->addWidget(m_pBubble, 0, Qt::AlignRight); // 图片紧贴用户名
        userContentLayout->addWidget(m_progressBar, 0, Qt::AlignRight);
        mainLayout->addLayout(userContentLayout);
        mainLayout->addWidget(avatar, 0, Qt::AlignTop);
    } else {
        // 接收方布局：头像在左侧
        mainLayout->addWidget(avatar, 0, Qt::AlignTop);
        userContentLayout->addWidget(nameLabel, 0, Qt::AlignLeft); // 用户名紧贴顶部
        userContentLayout->addWidget(m_pBubble, 0, Qt::AlignCenter); // 图片紧贴用户名
        userContentLayout->addWidget(m_progressBar, 0, Qt::AlignLeft);
        mainLayout->addLayout(userContentLayout);
        mainLayout->addStretch();
    }
    connect(m_progressBar, &FileTransferWidget::sig_stop, m_pBubble, &Bubble::slot_stop_event);
    connect(m_progressBar, &FileTransferWidget::sig_continue, m_pBubble, &Bubble::slot_continue_event);
    connect(m_progressBar, &FileTransferWidget::sig_stop, this, &FileBubble::slot_stop_event);
    connect(m_progressBar, &FileTransferWidget::sig_continue, this, &FileBubble::slot_continue_event);

    connect(m_fileclient.get(), &FileClient::sig_bytes_transfered, this, &FileBubble::slot_caculateCurrentProgress);
    connect(m_fileclient.get(), &FileClient::sig_transferFinished, this, &FileBubble::slot_uploadFinished);
    connect(m_fileclient.get(), &FileClient::sig_downloadFinished, this, &FileBubble::slot_downloadFinished);


    connect(this, &FileBubble::sig_setCurrentProgress, m_progressBar, &FileTransferWidget::slot_setCurrentProgress);
    connect(this, &FileBubble::sig_transferFinished, m_pBubble, &Bubble::slot_transferFinished);
    connect(this, &FileBubble::sig_downloadFinished, m_pBubble, &Bubble::slot_downloadFinished);

    connect(this, &FileBubble::sig_stop_uploading_file, m_fileclient.get(), &FileClient::slot_stopuploadFile);
    connect(this, &FileBubble::sig_continue_uploading_file, m_fileclient.get(), &FileClient::uploadFile);

    connect(m_pBubble, &Bubble::sig_downloadfile_req, this, &FileBubble::slot_downloadfile_req);
    connect(this, &FileBubble::sig_downloadfile_req, m_fileclient.get(), &FileClient::downloadFile);
}

void FileBubble::setMyId(QString mid){
    m_id = mid;
}

QString FileBubble::myId(){
    return m_id;
}


void FileBubble::slot_startuploadFile(uint64_t fileid, QString filebubbleid){
    if(m_role == Self && myId() == filebubbleid){
        m_progressBar->show();
        m_fileid = fileid;
        m_fileclient->uploadFile(m_fileid, m_filepath);
    }
}

void FileBubble::slot_downloadfile_req()
{
    m_progressBar->show();
    emit sig_downloadfile_req(m_fileid, m_filepath);
}

void FileBubble::slot_stop_event()
{
    if(m_role == Self){
        emit sig_stop_uploading_file(m_fileid);
    }else{
        emit sig_stop_downloading_file(m_fileid);
    }
}

void FileBubble::slot_continue_event()
{
    if(m_role == Self){
        emit sig_continue_uploading_file(m_fileid, m_filepath);
    }else{
        emit sig_continue_downloading_file(m_fileid, m_filename);
    }
}

void FileBubble::slot_caculateCurrentProgress(int bytes)
{
    int process = static_cast<int>(bytes*100/m_fileSize);
    emit sig_setCurrentProgress(process);
}

void FileBubble::slot_uploadFinished(uint64_t fileid)
{
    if(fileid == m_fileid){
        emit sig_transferFinished();
    }
}

void FileBubble::slot_downloadFinished(uint64_t fileid)
{
    if(fileid == m_fileid){
        emit sig_downloadFinished();
    }
}

Bubble::Bubble(ChatRole role, const QString &fileName, const QString &fileSize, const QString &fileType, QWidget *parent)
    : QWidget(parent), m_role(role), m_fileName(fileName), m_fileSize(fileSize), m_fileType(fileType), m_downloadState(false) {
    setMinimumHeight(80); // 设置初始高度
    setCursor(Qt::PointingHandCursor); // 鼠标悬停效果
    setMinimumWidth(300);
    setMaximumWidth(350);
    m_text = (role == Self)? m_fileSize : m_fileSize + "/" + "点击下载，7天后失效";
}

void Bubble::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    // 设置气泡框和三角形参数
    QRect bubbleRect;
    QPoint trianglePoints[3];
    QRect iconRect;
    if (m_role == Self) {
        // 发送气泡，靠右
        bubbleRect = QRect(50, 10, width() - 60, height() - 20);
        trianglePoints[0] = QPoint(width() - 20, 20);
        trianglePoints[1] = QPoint(width() - 3 , 25);
        trianglePoints[2] = QPoint(width() - 20, 42);
        painter.setBrush(QColor("#444"));
        iconRect= QRect(65, 17, 37, 46);
    } else {
        // 接收气泡，靠左
        bubbleRect = QRect(10, 10, width() - 60, height() - 20);
        trianglePoints[0] = QPoint(20, 20);
        trianglePoints[1] = QPoint(0, 25);
        trianglePoints[2] = QPoint(20, 42);
        painter.setBrush(QColor("#444"));
        iconRect= QRect(width()-100, 17, 37, 46);

    }

    // 绘制圆角矩形
    QPainterPath path;
    path.addRoundedRect(bubbleRect, 10, 10);
    painter.drawPath(path);

    // 绘制三角形
    painter.drawPolygon(trianglePoints, 3);
    QFont font("ubuntu", 10);
    painter.setFont(font);
    QFontMetrics fontMetrics(font);
    if (m_role == Self) {
        // 绘制文件信息
        painter.setPen(Qt::white);
        int textWidth = fontMetrics.horizontalAdvance(m_fileName);
        qDebug() << textWidth << " " << width()-textWidth;
        painter.drawText(bubbleRect.adjusted(width()-textWidth-70, 10, 100, 30), m_fileName, QTextOption(Qt::AlignLeft | Qt::AlignTop));
        painter.setPen(Qt::lightGray);
        painter.drawText(bubbleRect.adjusted(150, 35, -10, -8), m_text, QTextOption(Qt::AlignRight | Qt::AlignBottom));
    }else{
        // 绘制文件信息
        painter.setPen(Qt::white);
        painter.drawText(bubbleRect.adjusted(20, 10, -10, 30), m_fileName, QTextOption(Qt::AlignLeft | Qt::AlignTop));
        painter.setPen(Qt::lightGray);
        painter.drawText(bubbleRect.adjusted(20, 35, -10, -8), m_text, QTextOption(Qt::AlignLeft | Qt::AlignBottom));
    }
    // 绘制文件类型图标
    painter.setBrush(Qt::NoBrush);
    if (m_fileType == "pdf") {
        painter.drawPixmap(iconRect, QPixmap(":/res/pdf_icon.png"));
    } else if (m_fileType == "txt") {
        painter.drawPixmap(iconRect, QPixmap(":/res/txt_icon.png"));
    } else {
        painter.drawPixmap(iconRect, QPixmap(":/icons/file_icon.png"));
    }
}

QSize Bubble::sizeHint() const {
    return QSize(width(), 80); // 动态调整宽度
}

void Bubble::mousePressEvent(QMouseEvent *event) {
    if(!m_downloadState){
        m_downloadState = true;
        if (event->button() == Qt::LeftButton) {
            if (m_role == Self) {

            }else{
                emit sig_downloadfile_req();
                SetState(State::Start);
            }
        }
    }
}

void Bubble::slot_stop_event()
{
    SetState(State::Stop);
}

void Bubble::slot_continue_event()
{
    SetState(State::Continue);
}

void Bubble::slot_transferFinished()
{
    SetState(State::End);
}

void Bubble::slot_downloadFinished()
{
    SetState(State::End);
}

void Bubble::SetState(State state){
    if(m_role == Self){
        switch (state) {
        case State::Start:
            m_text = m_fileSize + "/" + "发送中...";
            break;
        case State::Continue:
            m_text = m_fileSize + "/" + "发送中...";
            break;
        case State::Stop:
            m_text = m_fileSize + "/" + "发送暂停";
            break;
        case State::End:
            m_text = m_fileSize + "/" + "发送完成";
            break;
        }
    }else{
        switch (state) {
        case State::Start:
            m_text = m_fileSize + "/" + "下载中...";
            break;
        case State::Continue:
            m_text = m_fileSize + "/" + "下载中...";
            break;
        case State::Stop:
            m_text = m_fileSize + "/" + "下载暂停";
            break;
        case State::End:
            m_text = m_fileSize + "/" + "下载完成";
            break;
        }
    }
    update();
}

