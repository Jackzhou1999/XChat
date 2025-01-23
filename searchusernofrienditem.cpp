#include "searchusernofrienditem.h"
#include "ui_searchusernofrienditem.h"

SearchUserNoFriendItem::SearchUserNoFriendItem(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchUserNoFriendItem)
{
    ui->setupUi(this);
    connect(ui->pushButton, &QPushButton::clicked, this, &SearchUserNoFriendItem::slot_to_friendapply_page);
}

SearchUserNoFriendItem::~SearchUserNoFriendItem()
{
    delete ui;
}

void SearchUserNoFriendItem::setInfo(DbUserInfo info)
{
    _uid = info.uid;
    QPixmap pixmap(info.icon);
    ui->search_user_icon_lb->setPixmap(pixmap.scaled(ui->search_user_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->search_user_icon_lb->setScaledContents(true);
    QString name_uid = info.name + " (uid:" + QString::number(info.uid) + ")";
    ui->search_user_name_uid_lb->setText(name_uid);
    ui->search_user_email_lb->setText(info.email);
}

void SearchUserNoFriendItem::slot_to_friendapply_page()
{
    emit sig_to_friendapply_page(_uid);
}
