#include "searchuserfrienditem.h"
#include "ui_searchuserfrienditem.h"

SearchUserFriendItem::SearchUserFriendItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchUserFriendItem)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &SearchUserFriendItem::slot_to_chat_page);
}

SearchUserFriendItem::~SearchUserFriendItem()
{
    delete ui;
}

void SearchUserFriendItem::setInfo(DbUserInfo info)
{
    _uid = info.uid;
    QPixmap pixmap(info.icon);
    ui->search_user_icon_lb->setPixmap(pixmap.scaled(ui->search_user_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->search_user_icon_lb->setScaledContents(true);
    QString name_uid = info.name + " (uid:" + QString::number(info.uid) + ")";
    ui->search_user_name_uid_lb->setText(name_uid);
    ui->search_user_email_lb->setText(info.email);
}

void SearchUserFriendItem::slot_to_chat_page()
{
    emit sig_to_chat_page(_uid);
}
