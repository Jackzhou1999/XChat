#include "searchuseritem.h"
#include "ui_searchuseritem.h"

SearchUserItem::SearchUserItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::SearchUserItem)
{
    ui->setupUi(this);
    SetItemType(ListItemType::SEARCH_USER_ITEM);
    ui->s_btn->SetState("normal", "hover", "press");
    connect(ui->s_btn, &ClickButton::clicked, this, &SearchUserItem::slot_tofriendinfopage);

    QFont font;
    font.setPointSize(11);
    ui->name_uid_lb->setFont(font);
    QFont font2;
    font2.setPointSize(10);
    ui->email_lb->setFont(font2);
}

SearchUserItem::~SearchUserItem()
{
    delete ui;
}

void SearchUserItem::setInfo(DbUserInfo info)
{
    _uid = info.uid;
    QPixmap pixmap(info.icon);
    ui->search_icon_lb->setPixmap(pixmap.scaled(ui->search_icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->search_icon_lb->setScaledContents(true);
    QString name_uid = info.name + " (uid:" + QString::number(info.uid) + ")";
    ui->name_uid_lb->setText(name_uid);
    ui->email_lb->setText(info.email);
}

void SearchUserItem::slot_tofriendinfopage()
{
    emit sig_tofriendinfopage(_uid);
}
