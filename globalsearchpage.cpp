#include "globalsearchpage.h"
#include "ui_globalsearchpage.h"
#include "searchuserfrienditem.h"
#include "searchusernofrienditem.h"
#include "sqlmgr.h"
#include "tcpmgr.h"
#include <QJsonDocument>

GlobalSearchPage::GlobalSearchPage(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GlobalSearchPage)
{
    ui->setupUi(this);
    setWindowTitle(tr("全网搜索"));


    ui->global_search_edit->SetMaxLength(15);

    ui->all_btn->setStyleSheet(
        "QRadioButton::indicator {"
        "   width: 0px;"       // 隐藏圆形图标
        "   height: 0px;"
        "}"
        "QRadioButton {"
        "   color: black;"     // 默认字体颜色
        "   text-decoration: none;" // 默认无下划线
        "}"
        "QRadioButton:checked {"
        "   color: rgb(93, 173, 226);"      // 选中后字体颜色变为蓝色
        "   text-decoration: underline;" // 选中后显示下划线
        "}"
        );

    ui->user_btn->setStyleSheet(
        "QRadioButton::indicator {"
        "   width: 0px;"       // 隐藏圆形图标
        "   height: 0px;"
        "}"
        "QRadioButton {"
        "   color: black;"     // 默认字体颜色
        "   text-decoration: none;" // 默认无下划线
        "}"
        "QRadioButton:checked {"
        "   color: rgb(93, 173, 226);"      // 选中后字体颜色变为蓝色
        "   text-decoration: underline;" // 选中后显示下划线
        "}"
        );

    ui->group_btn->setStyleSheet(
        "QRadioButton::indicator {"
        "   width: 0px;"       // 隐藏圆形图标
        "   height: 0px;"
        "}"
        "QRadioButton {"
        "   color: black;"     // 默认字体颜色
        "   text-decoration: none;" // 默认无下划线
        "}"
        "QRadioButton:checked {"
        "   color: rgb(93, 173, 226);"      // 选中后字体颜色变为蓝色
        "   text-decoration: underline;" // 选中后显示下划线
        "}"
        );



    installEventFilter(this);
    QAction *searchAction = new QAction(ui->global_search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->global_search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->global_search_edit->setPlaceholderText(tr("输入搜索关键词"));
    ui->global_search_edit->setPlaceholderOffset(35, 3);

    QAction * clearAction = new QAction(ui->global_search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->global_search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    connect(ui->global_search_edit, &CustomLineEdit::textChanged, [this, clearAction](const QString& text){
        if(!text.isEmpty()){
            clearAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearAction, &QAction::triggered, [this, clearAction](){
        ui->global_search_edit->clear();
        clearAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->global_search_edit->clearFocus();
    });

    // 设置样式表
    ui->global_search_edit->setStyleSheet(
        "QLineEdit {"
        "   background-color: rgb(247,247,248);" // 设置背景颜色
        "   border: none;"                         // 移除边框
        "   padding: 5px;"
        "}"

        "QLineEdit:focus {"
        "   background-color: rgb(247,247,248);" // 设置背景颜色
        "   border: 1px solid blue;"
        "   padding: 5px;"
        "}"
        );
    connect(this, &GlobalSearchPage::sig_to_friendapply_page, this, &GlobalSearchPage::slot_to_friendapply_page);
    connect(ui->global_search_edit, &CustomLineEdit::textChanged, this, &GlobalSearchPage::slot_send_search_request);
}

GlobalSearchPage::~GlobalSearchPage()
{
    delete ui;
}

void GlobalSearchPage::addUserItem(const DbUserInfo &info)
{
    bool ismyfriend = DatabaseManager::doFriendsExist(info.uid);
    if(ismyfriend){
        auto *search_user_item = new SearchUserFriendItem();
        search_user_item->setInfo(info);
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(search_user_item->sizeHint());

        ui->global_search_list->addItem(item);
        ui->global_search_list->setItemWidget(item, search_user_item);
        connect(search_user_item, &SearchUserFriendItem::sig_to_chat_page, this, &GlobalSearchPage::sig_to_chat_page);
    }else{
        auto *search_user_item = new SearchUserNoFriendItem();
        search_user_item->setInfo(info);
        QListWidgetItem *item = new QListWidgetItem;
        item->setSizeHint(search_user_item->sizeHint());

        ui->global_search_list->addItem(item);
        ui->global_search_list->setItemWidget(item, search_user_item);
        connect(search_user_item, &SearchUserNoFriendItem::sig_to_friendapply_page, this, &GlobalSearchPage::sig_to_friendapply_page);
    }

}

void GlobalSearchPage::initEdit(const QString &text)
{
    ui->global_search_edit->setText(text);
}

void GlobalSearchPage::slot_to_friendapply_page(int uid)
{
    _applyfriendpage = std::make_shared<ApplyFriend>(this);
    std::optional<DbUserInfo> info_opt = DatabaseManager::getUserInfoByUid(uid);
    auto info = info_opt.value();
    _applyfriendpage->SetInfo(info);
    _applyfriendpage->show();
}

void GlobalSearchPage::slot_send_search_request(QString text)
{
    ui->global_search_list->clear();
    if(!text.isEmpty()){
        QJsonObject ret;
        QJsonArray jsonArray;

        QVector<qint64> uids;
        QMap<int, DbUserInfo> userinfos;

        bool isuid;
        int uid = text.toInt(&isuid);
        if(isuid){
            ret["uid"] = uid;
        }

        static QRegularExpression regex("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
        bool isemail = regex.match(text).hasMatch();
        if(isemail){
            ret["email"] = text;
        }

        ret["name"] = text;

        QJsonDocument doc(ret);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_USER_SEARCH, jsonString);
    }
}

void GlobalSearchPage::slot_recv_search_result(QVector<DbUserInfo> users)
{
    for(auto& info: users){
        addUserItem(info);
    }
}
