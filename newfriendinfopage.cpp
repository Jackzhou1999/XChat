#include "newfriendinfopage.h"
#include "ui_newfriendinfopage.h"
#include "sqlmgr.h"
#include "cachemanager.h"
#include "usermgr.h"
#include "sqlmgr.h"

NewFriendInfoPage::NewFriendInfoPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewFriendInfoPage)
{
    ui->setupUi(this);

    ui->auth_friend_btn->SetState("normal", "hover", "press");
    QFont font;
    font.setPointSize(16);  // 设置字体大小为 20 点
    ui->peername_lb->setFont(font);
    // connect(ui->auth_friend_btn, &ClickButton::clicked, this, &NewFriendInfoPage::slot_auth_friend_apply);
}

NewFriendInfoPage::~NewFriendInfoPage()
{
    delete ui;
}

void NewFriendInfoPage::setNewFriendInfo(int uid)
{
    // select data from sqllite newfriend tabel
    std::optional<DbUserInfo> cacheinfo = CacheManager::getInstance().getUserInfo(uid);
    DbUserInfo info;
    if(cacheinfo.has_value()){
        qDebug() << "NewFriendInfoPage::setNewFriendInfo: use cache";
        info = cacheinfo.value();
    }else{
        auto info_opt = DatabaseManager::getUserInfoByUid(uid);
        if(info_opt.has_value()) info = info_opt.value();
    }
    int myid = UserMgr::GetInstance()->getUid_int();

    QString text = "无";
    std::optional<FriendRequest> msg = DatabaseManager::getFriendRequestBySenderAndReceiver(uid, myid);
    if(msg.has_value()){
        FriendRequest msgvalue = msg.value();
        text = msgvalue.message;
    }
    QPixmap head_pix(info.icon);
    head_pix = head_pix.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->icon_lb->setPixmap(head_pix);
    ui->peername_lb->setText(info.name);
    ui->uid_lb->setText(QString::number(info.uid));
    ui->msg_lb->setText(text);

}
