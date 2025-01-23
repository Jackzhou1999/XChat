#include "chatuserwid.h"
#include "ui_chatuserwid.h"
#include <QJsonDocument>
#include "sqlmgr.h"
#include "usermgr.h"
#include "tcpmgr.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);

    _redDotLabel = new QLabel(this);
    _redDotLabel->setFixedSize(12, 12);
    _redDotLabel->setStyleSheet(
        "background-color: red; "
        "color: white; "
        "border-radius: 6px; "
        "font-size: 11px;"
        "font-weight: bold;"
        "text-align: center;"
        );
    _redDotLabel->setAlignment(Qt::AlignCenter);
    _redDotLabel->hide(); // 初始时隐藏
    _set_msg = false;
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

void ChatUserWid::SetInfo(const DbUserInfo& info)
{
    _uid = info.uid;
    _name = info.name;
    _head = info.icon;
    qDebug() << "ChatUserWid::SetInfo:" << _head;
    QPixmap pixmap(_head);
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
    ui->user_name_lb->setText(_name);

    _last_msg.clear();
    _last_msg_type = 0;


    int myid = UserMgr::GetInstance()->getUid_int();
    ChatMessage lastmsg;
    std::optional<ChatMessage> msg = DatabaseManager::getLastChatMessage(myid, info.uid);
    if(msg.has_value()){
        lastmsg = msg.value();
        _last_msg = lastmsg.content;
        _last_msg_type = lastmsg.content_type;
        _last_msg_time = lastmsg.created_at;
        _set_msg = true;
        if(_last_msg_type == 1){
            if (_last_msg.length() > 20) {
                _last_msg = _last_msg.left(20) + "…"; // 截取前 maxLength 个字符并添加省略号
            }
        }
        int hour = _last_msg_time.time().hour();
        int minute = _last_msg_time.time().minute();

        // 格式化为字符串，例如 "14:23"
        QString formattedTime = QString("%1:%2")
                                    .arg(hour, 2, 10, QChar('0')) // 保证两位小时
                                    .arg(minute, 2, 10, QChar('0')); // 保证两位分钟

        if(_last_msg_type == 3){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(_last_msg.toUtf8());
            QJsonObject jsonObj = jsonDoc.object();
            QString filename = jsonObj["filename"].toString();
            _last_msg = "[文件]" + filename;
        }
        if(_last_msg_type == 2){
            _last_msg = "[图片]";
        }
        ui->user_chat_lb->setText(_last_msg);
        ui->time_lb->setText(formattedTime);
    }
}

void ChatUserWid::Refresh()
{
    std::cout << "run ChatUserWid::Update()" <<std::endl;
    int myid = UserMgr::GetInstance()->getUid_int();
    std::optional<ChatMessage> msg = DatabaseManager::getLastChatMessage(myid, _uid);
    if(msg.has_value()){
        std::cout << "run ChatUserWid::Update() has value" <<std::endl;

        ChatMessage lastmsg = msg.value();
        _last_msg = lastmsg.content;
        _last_msg_type = lastmsg.content_type;
        _last_msg_time = lastmsg.created_at;
        _set_msg = true;
        int hour = _last_msg_time.time().hour();
        int minute = _last_msg_time.time().minute();

        if(_last_msg_type == 0){
            if (_last_msg.length() > 20) {
                _last_msg = _last_msg.left(20) + "…"; // 截取前 maxLength 个字符并添加省略号
                qDebug() << "ChatUserWid::SetInfo:" << _last_msg;
            }
        }

        // 格式化为字符串，例如 "14:23"
        QString formattedTime = QString("%1:%2")
                                    .arg(hour, 2, 10, QChar('0')) // 保证两位小时
                                    .arg(minute, 2, 10, QChar('0')); // 保证两位分钟

        if(_last_msg_type == 3){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(_last_msg.toUtf8());
            QJsonObject jsonObj = jsonDoc.object();
            QString filename = jsonObj["filename"].toString();
            _last_msg = "[文件]" + filename;
        }

        if(_last_msg_type == 2){
            _last_msg = "[图片]";
        }

        ui->user_chat_lb->setText(_last_msg);
        ui->time_lb->setText(formattedTime);
    }
}


void ChatUserWid::UpdateInfo(const ChatMessage& lastmsg)
{
    _last_msg = lastmsg.content;
    _last_msg_type = lastmsg.content_type;
    _last_msg_time = lastmsg.created_at;
    _set_msg = true;
    int hour = _last_msg_time.time().hour();
    int minute = _last_msg_time.time().minute();

    if(_last_msg_type == 0){
        if (_last_msg.length() > 20) {
            _last_msg = _last_msg.left(20) + "…"; // 截取前 maxLength 个字符并添加省略号
            qDebug() << "ChatUserWid::SetInfo:" << _last_msg;
        }
    }

    // 格式化为字符串，例如 "14:23"
    QString formattedTime = QString("%1:%2")
                                .arg(hour, 2, 10, QChar('0')) // 保证两位小时
                                .arg(minute, 2, 10, QChar('0')); // 保证两位分钟

    if(_last_msg_type == 3){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(_last_msg.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        QString filename = jsonObj["filename"].toString();
        _last_msg = "[文件]" + filename;
    }

    if(_last_msg_type == 2){
        _last_msg = "[图片]";
    }

    ui->user_chat_lb->setText(_last_msg);
    ui->time_lb->setText(formattedTime);
}

void ChatUserWid::SetUnreadCount(int count)
{
    _unreadCount = count;
    updateRedDot();
}

int ChatUserWid::GetUid()
{
    return _uid;
}

bool ChatUserWid::hasMsg()
{
    return _set_msg;
}

void ChatUserWid::updateRedDot()
{
    if (_unreadCount > 0)
    {
        _redDotLabel->setText(QString::number(_unreadCount)); // 设置未读消息数量
        _redDotLabel->show();

        // 调整红点位置
        int x = ui->icon_lb->width() - 1; // 红点居右上角
        int y = 7;                         // 距离顶部
        _redDotLabel->move(x, y);
        _redDotLabel->raise();
    }
    else
    {
        _redDotLabel->hide(); // 隐藏红点
    }
}
