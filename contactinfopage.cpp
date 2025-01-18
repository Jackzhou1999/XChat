#include "contactinfopage.h"
#include "ui_contactinfopage.h"
#include "sqlmgr.h"
#include "cachemanager.h"
#include "usermgr.h"
#include "sqlmgr.h"

contactinfopage::contactinfopage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::contactinfopage)
{
    ui->setupUi(this);
    ui->sendmsg_btn->SetState("normal", "hover", "press");
    QFont font;
    font.setPointSize(16);  // 设置字体大小为 20 点
    ui->contactname_lb->setFont(font);
    connect(ui->sendmsg_btn, &ClickButton::clicked, this, &contactinfopage::slot_to_chat_page);
}

contactinfopage::~contactinfopage()
{
    delete ui;
}

void contactinfopage::setContactInfo(int uid)
{
    std::optional<DbUserInfo> cacheinfo = CacheManager::getInstance().getUserInfo(uid);
    DbUserInfo info;
    if(cacheinfo.has_value()){
        qDebug() << "NewFriendInfoPage::setNewFriendInfo: use cache";
        info = cacheinfo.value();
    }else{
        info = DatabaseManager::getUserInfoByUid(uid);
    }
    int myid = UserMgr::GetInstance()->getUid_int();

    QPixmap head_pix(info.icon);
    head_pix = head_pix.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->icon_lb->setPixmap(head_pix);

    ui->contactname_lb->setText(info.name);
    ui->uid_lb->setText(QString::number(info.uid));
    ui->email_lb->setText(info.email);
    ui->nick_lb->setText(info.nick);
    _uid = info.uid;
}

void contactinfopage::slot_to_chat_page()
{
    emit sig_to_chat_page(_uid);
}
