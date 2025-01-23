#include "applyfriend.h"
#include "ui_applyfriend.h"
#include "clicklabelplus.h"
#include "friendlabel.h"
#include <QScrollBar>
#include "usermgr.h"
#include "tcpmgr.h"
#include <QJsonDocument>
#include <QByteArray>

ApplyFriend::ApplyFriend(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ApplyFriend)
{
    ui->setupUi(this);
    // 隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFriend");
    this->setModal(true);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->apply_friend_sure_btn->SetState("normal","hover","press");
    ui->applyfriend_cancel_btn->SetState("normal","hover","press");
    // //连接确认和取消按钮的槽函数
    connect(ui->applyfriend_cancel_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplyCancel);
    connect(ui->apply_friend_sure_btn, &QPushButton::clicked, this, &ApplyFriend::SlotApplySure);
}

ApplyFriend::~ApplyFriend()
{
    qDebug()<< "ApplyFriend destruct";
    delete ui;
}

bool ApplyFriend::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->scrollArea && event->type() == QEvent::Enter)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(false);
    }
    else if (obj == ui->scrollArea && event->type() == QEvent::Leave)
    {
        ui->scrollArea->verticalScrollBar()->setHidden(true);
    }
    return QObject::eventFilter(obj, event);
}

void ApplyFriend::SetInfo(const DbUserInfo &info)
{
    _uid = info.uid;
    QPixmap pixmap(info.icon);
    ui->apply_friend_icon_lb->setPixmap(pixmap.scaled(ui->apply_friend_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->apply_friend_icon_lb->setScaledContents(true);
    QString name_uid = info.name + " (uid:" + QString::number(info.uid) + ")";
    ui->apply_friend_name_uid_lb->setText(name_uid);
    ui->apply_friend_email_lb->setText(info.email);
}

void ApplyFriend::SlotApplyCancel()
{
    qDebug() << "Slot Apply Cancel";
    hide();
}

void ApplyFriend::SlotApplySure()
{
    QJsonObject ret;
    ret["send_uid"] = UserMgr::GetInstance()->getUid_int();
    ret["recv_uid"] = static_cast<int>(_uid);

    QDateTime currentDateTime = QDateTime::currentDateTime();
    ret["timestamp"] = currentDateTime.toSecsSinceEpoch();
    ret["message"] = ui->name_ed->text();
    QJsonDocument doc(ret);
    QString jsonString = doc.toJson(QJsonDocument::Indented);
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_NOTIFY_ADD_FRIEND_REQ, jsonString);
    qDebug() << "Slot Apply Finished";
    hide();
}
