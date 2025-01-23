#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QRandomGenerator>
#include "chatuserwid.h"
#include "loadingdlg.h"
#include <unistd.h>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include "customizeedit.h"
#include <QMouseEvent>
#include <QMessageBox>
#include "sqlmgr.h"
#include "usermgr.h"
#include "tcpmgr.h"
#include <QJsonDocument>
#include <QFileDialog>
#include "fileclient.h"


ChatDialog::ChatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ChatDialog), _mode(ChatUiMode::ChatMode), _state(ChatUiMode::ChatMode), _b_loading(false), _prev_state(ChatUiMode::ChatMode),_global_search_dlg(nullptr)
{
    ui->setupUi(this);
    ui->add_btn->SetState("normal", "hover", "press");
    ui->download_file_btn->SetState("normal", "hover", "press");
    connect(ui->download_file_btn, &ClickButton::clicked, this, &ChatDialog::slot_to_downloadfile_dir);

    ui->search_edit->SetMaxLength(10);
    installEventFilter(this);
    QAction *searchAction = new QAction(ui->search_edit);
    searchAction->setIcon(QIcon(":/res/search.png"));
    ui->search_edit->addAction(searchAction, QLineEdit::LeadingPosition);
    ui->search_edit->setPlaceholderText(tr("搜索"));
    ui->search_edit->setPlaceholderOffset(35, 4);

    clearAction = new QAction(ui->search_edit);
    clearAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->search_edit->addAction(clearAction, QLineEdit::TrailingPosition);
    connect(ui->search_edit, &CustomLineEdit::textChanged, [this](const QString& text){
        if(!text.isEmpty()){
            clearAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearAction, &QAction::triggered, [this](){
        ui->search_edit->clear();
        clearAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->search_edit->clearFocus();
        _state = _prev_state;
        ShowSearch(false);
    });
    connect(ui->search_edit, &CustomLineEdit::sig_switch_search, this, &ChatDialog::slot_search_list_click);
    connect(ui->search_edit, &CustomLineEdit::textChanged, this, &ChatDialog::slot_search_list);
    ShowSearch(false);

    _loadingScreen = new LoadingDialog(this);
    connect(ui->chat_user_list, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);

    auto head = UserMgr::GetInstance()->getIcon();
    QPixmap pixmap(head);
    QPixmap scaledPixmap = pixmap.scaled(ui->side_head_lb->size(), Qt::KeepAspectRatio);
    ui->side_head_lb->setPixmap(scaledPixmap);
    ui->side_head_lb->setScaledContents(true);

    ui->side_chat_lb->setProperty("state", "normal");
    ui->side_chat_lb->SetState("normal", "hover", "selected");
    ui->side_contact_lb->SetState("normal", "hover", "selected");
    connect(ui->side_chat_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);
    connect(ui->side_contact_lb, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    _side_lb_list.append(ui->side_chat_lb);
    _side_lb_list.append(ui->side_contact_lb);
    addChatUserList();
    ui->stackedWidget->setCurrentWidget(ui->empty_page);

    connect(ui->chat_user_list, &QListWidget::itemClicked, this, &ChatDialog::slot_item_clicked);
    connect(ui->con_user_list, &ContactTreeWidget::to_newfriendpage, this, &ChatDialog::slot_tonewfriendpage);
    connect(ui->con_user_list, &ContactTreeWidget::to_friendinfopage, this, &ChatDialog::slot_tofriendinfopage);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_new_msg_arrived, this, &ChatDialog::slot_update_chat_user_item);
    connect(ui->contact_page, &contactinfopage::sig_to_chat_page, this, &ChatDialog::slot_to_chat_page);
    connect(ui->search_list, &SearchList::to_friendinfopage, this, &ChatDialog::slot_tofriendinfopage);
    connect(ui->search_list, &SearchList::to_globalsearchpage, this, &ChatDialog::slot_to_globalsearchpage);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_searchfinished, this, &ChatDialog::slot_searchfinished);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_updatefriendrequest, this, &ChatDialog::slot_updatemyapply);
}


ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::ShowSearch(bool bsearch)
{
    if(bsearch){
        ui->chat_user_list->hide();
        ui->con_user_list->hide();
        ui->search_list->show();
        _mode = ChatUiMode::SearchMode;
    }else if(_state == ChatUiMode::ChatMode){
        ui->chat_user_list->show();
        ui->con_user_list->hide();
        ui->search_list->hide();
        ui->search_edit->clear();
        _mode = ChatUiMode::ChatMode;
    }else if(_state == ChatUiMode::ContactMode){
        ui->chat_user_list->hide();
        ui->con_user_list->show();
        ui->search_list->hide();
        ui->search_edit->clear();
        _mode = ChatUiMode::ContactMode;
    }
}


void ChatDialog::addChatUserList()
{
    //数据库接口
    //查询好友关系表，获取10个好友uid
    // 向服务器端发送获取聊天记录请求,将获取的聊天记录存储与本地数据库
    // 查询本地数据库聊天记录表中uid=uid的状态为未读的信息，获取信息数量
    int myid = UserMgr::GetInstance()->getUid_int();
    QVector<qint64> ids = DatabaseManager::getAllUnreadSenderIds(myid);
    std::vector<DbUserInfo> userinfos = DatabaseManager::getUserInfoByUids(ids);
    qDebug() << "ChatDialog::addChatUserList" << " " << myid << ids.size();
    qDebug() << userinfos.size();
    for(auto& info: userinfos){
        QVector<qint64> unreadids;
        int unreadnum = DatabaseManager::getUnreadMessageCountFromUser(info.uid, myid, unreadids);
        qDebug() << "ChatDialog::addChatUserList unread meg count:" << unreadnum;

        auto* chatuserwid = new ChatUserWid();
        chatuserwid->SetInfo(info);
        chatuserwid->unread_msg_ids.unite(QSet<int>(unreadids.begin(), unreadids.end()));
        chatuserwid->SetUnreadCount(chatuserwid->unread_msg_ids.size());
        qDebug() << "now unread nums:" << chatuserwid->unread_msg_ids.size();
        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(chatuserwid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chatuserwid);

        ChatUserWidMgr.insert(info.uid, chatuserwid);
        save.insert(item, chatuserwid);
        save2.insert(chatuserwid, item);

    }
}

void ChatDialog::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = ui->chat_user_list->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM
        || itemType == ListItemType::GROUP_TIP_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }


    if(itemType == ListItemType::CHAT_USER_ITEM){
        // 创建对话框，提示用户
        qDebug()<< "contact user item clicked ";

        auto chat_wid = qobject_cast<ChatUserWid*>(customItem);
        auto uid = chat_wid->GetUid();
        //跳转到聊天界面
        ui->stackedWidget->setCurrentWidget(ui->chat_page);
        ui->chat_page->_chatuserwid = chat_wid;
        ui->chat_page->SetUserInfo(uid);
        _cur_chat_uid = uid;

        if(_state != ChatUiMode::SearchMode){
            _prev_state = _state;
        }
        _state = ChatUiMode::ChatMode;
        ShowSearch(false);
        ui->side_chat_lb->SetSelected(true);
        ui->side_contact_lb->ClearState();

        //设置当前chatpage对应的chatuseritem
        return;
    }
}

void ChatDialog::slot_tonewfriendpage(int uid)
{
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    ui->friend_apply_page->setNewFriendInfo(uid);
}

void ChatDialog::slot_tofriendinfopage(int uid)
{
    ui->stackedWidget->setCurrentWidget(ui->contact_page);
    ui->contact_page->setContactInfo(uid);
}

void ChatDialog::slot_tomyapplypage(int uid)
{

}


void ChatDialog::ClearLabelState(StateWidget *w)
{
    for(auto widget: _side_lb_list){
        if(widget == w) continue;
        widget->ClearState();
    }
}

void ChatDialog::slot_add_chat_user_item(int uid)
{
    //数据库接口
    //查询聊天记录表中uid=uid的状态为未读的信息，获取信息数量
    QVector<qint64> unreadids;
    int myid = UserMgr::GetInstance()->getUid_int();
    DatabaseManager::getUnreadMessageCountFromUser(uid, myid, unreadids);
    DbUserInfo info = DatabaseManager::getUserInfoByUid(uid).value();

    auto* chatuserwid = new ChatUserWid();
    chatuserwid->SetInfo(info);
    chatuserwid->unread_msg_ids.unite(QSet<int>(unreadids.begin(), unreadids.end()));
    chatuserwid->SetUnreadCount(chatuserwid->unread_msg_ids.size());

    QListWidgetItem* item = new QListWidgetItem;
    item->setSizeHint(chatuserwid->sizeHint());
    ui->chat_user_list->insertItem(0, item);
    ui->chat_user_list->setItemWidget(item, chatuserwid);

    save.insert(item, chatuserwid);
    save2.insert(chatuserwid, item);

    ChatUserWidMgr.insert(info.uid, chatuserwid);

}

void ChatDialog::slot_update_chat_user_item(int uid)
{
    if(ChatUserWidMgr.find(uid) == ChatUserWidMgr.end()){
        slot_add_chat_user_item(uid);
        return;
    }
    //数据库接口
    //查询聊天记录表中uid=uid的状态为未读的信息，获取信息数量
    int myid = UserMgr::GetInstance()->getUid_int();
    QVector<qint64> unreadids;

    DatabaseManager::getUnreadMessageCountFromUser(uid, myid, unreadids);
    DbUserInfo info = DatabaseManager::getUserInfoByUid(uid).value();
    ChatUserWidMgr[uid]->unread_msg_ids.unite(QSet<int>(unreadids.begin(), unreadids.end()));
    std::cout << "ChatDialog::slot_update_chat_user_item" <<std::endl;

    ChatUserWidMgr[uid]->Refresh();
    if(ui->chat_page->_peeruid == uid && ui->stackedWidget->currentWidget() == ui->chat_page){
        std::optional<ChatMessage> msg = DatabaseManager::getLastChatMessage(myid, uid);
        ChatMessage lastmsg = msg.value();
        ui->chat_page->AppendChatMsg(lastmsg);
        if(ChatUserWidMgr[uid]->unread_msg_ids.find(lastmsg.id) != ChatUserWidMgr[uid]->unread_msg_ids.end()){
            ChatUserWidMgr[uid]->unread_msg_ids.remove(lastmsg.id);
        }
        std::vector<qint64> read_msg_ids;
        read_msg_ids.push_back(lastmsg.id);
        DatabaseManager::setMessageStatusToRead(lastmsg.id);

        QJsonObject ret;
        QJsonArray jsonArray;
        for(auto& id: read_msg_ids){
            jsonArray.append(id);
        }
        ret["msg_id"] = jsonArray;
        QJsonDocument doc(ret);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_MSG_READ_RSP, jsonString);

    }else{
        ChatUserWidMgr[uid]->SetUnreadCount(ChatUserWidMgr[uid]->unread_msg_ids.size());
    }

}

void ChatDialog::slot_loading_chat_user()
{
    if(_b_loading) return;
    _b_loading = true;
    _loadingScreen->showCentered(this); // 在子界面中心显示加载界面
    QTimer::singleShot(200, this, &ChatDialog::hideLoadingDialog); // 模拟加载0.2秒后隐藏
    // addChatUserList();

    _b_loading = false;
}

void ChatDialog::slot_side_chat()
{
    ClearLabelState(ui->side_chat_lb);
    ui->stackedWidget->setCurrentWidget(ui->empty_page);
    _prev_state = _state;
    _state = ChatUiMode::ChatMode;
    ShowSearch(false);
}

void ChatDialog::slot_side_contact()
{
    ClearLabelState(ui->side_contact_lb);
    ui->stackedWidget->setCurrentWidget(ui->empty_page);
    _prev_state = _state;
    _state = ChatUiMode::ContactMode;
    ShowSearch(false);
}

void ChatDialog::slot_search_list(const QString &text)
{
    if(!text.isEmpty()){
        if(_state != ChatUiMode::SearchMode){
            _prev_state = _state;
        }
        _state = ChatUiMode::SearchMode;
        ShowSearch(true);
        ui->search_list->ClearItem();
        QVector<qint64> uids;
        QMap<int, DbUserInfo> userinfos;

        bool isuid;
        int uid = text.toInt(&isuid);
        if(isuid){
            std::optional<DbUserInfo> user = DatabaseManager::getUserInfoByUid(uid);
            if(user.has_value()){
                auto userinfo = user.value();
                if(userinfos.find(userinfo.uid) == userinfos.end()){
                    uids.push_back(userinfo.uid);
                    userinfos[userinfo.uid] = userinfo;
                }
            }
        }

        static QRegularExpression regex("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
        bool isemail = regex.match(text).hasMatch();
        if(isemail){
            std::optional<DbUserInfo> user = DatabaseManager::getUserInfoByEmail(text);
            if(user.has_value()){
                auto userinfo = user.value();
                if(userinfos.find(userinfo.uid) == userinfos.end()){
                    uids.push_back(userinfo.uid);
                    userinfos[userinfo.uid] = userinfo;
                }
            }
        }

        std::vector<DbUserInfo> users = DatabaseManager::getUserInfoByName(text);
        for(auto& userinfo: users){
            if(userinfos.find(userinfo.uid) == userinfos.end()){
                uids.push_back(userinfo.uid);
                userinfos[userinfo.uid] = userinfo;
            }
        }
        std::cout << "uids size:" << uids.size() <<std::endl;
        std::vector<bool> isFriends = DatabaseManager::doFriendsExist(uids);
        for(int i=0; i<isFriends.size(); i++){
            if(isFriends[i]){
                ui->search_list->addUserItem(userinfos[uids[i]]);
            }
        }

    }else{
        _state = _prev_state;
        ShowSearch(false);
    }
}

void ChatDialog::slot_search_list_click()
{
    if(_state != ChatUiMode::SearchMode){
        _prev_state = _state;
    }
    clearAction->setIcon(QIcon(":/res/search_close.png"));
    _state = ChatUiMode::SearchMode;
    ui->stackedWidget->setCurrentWidget(ui->empty_page);

    ShowSearch(true);

}
void ChatDialog::hideLoadingDialog() {
    _loadingScreen->hide();
}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        qDebug() << "in ChatDialog::eventFilter";
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMousePress(mouseEvent);
    }

    return QDialog::eventFilter(watched, event);
}

void ChatDialog::handleGlobalMousePress(QMouseEvent *event)
{
    if(_state != ChatUiMode::SearchMode){
        return;
    }

    QPoint posInSearchList = ui->search_list->mapFromGlobal(event->globalPos());
    if(!ui->search_list->rect().contains(posInSearchList)){
        ui->search_edit->clear();
        ShowSearch(false);
    }
}

void ChatDialog::slot_to_chat_page(int uid){

    if(ChatUserWidMgr.find(uid) == ChatUserWidMgr.end()){
        DbUserInfo info = DatabaseManager::getUserInfoByUid(uid).value();
        int myid = UserMgr::GetInstance()->getUid_int();
        QVector<qint64> unreadids;
        DatabaseManager::getUnreadMessageCountFromUser(info.uid, myid, unreadids);

        auto* chatuserwid = new ChatUserWid();

        chatuserwid->SetInfo(info);
        chatuserwid->unread_msg_ids.unite(QSet<int>(unreadids.begin(), unreadids.end()));
        chatuserwid->SetUnreadCount(chatuserwid->unread_msg_ids.size());

        QListWidgetItem* item = new QListWidgetItem;
        item->setSizeHint(chatuserwid->sizeHint());
        ui->chat_user_list->addItem(item);
        ui->chat_user_list->setItemWidget(item, chatuserwid);

        ChatUserWidMgr.insert(info.uid, chatuserwid);
        save.insert(item, chatuserwid);
        save2.insert(chatuserwid, item);
    }

    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    ui->chat_page->_chatuserwid = ChatUserWidMgr[uid];
    ui->chat_user_list->setCurrentItem(save2[ChatUserWidMgr[uid]]);
    ui->chat_page->SetUserInfo(uid);
    std::cout << "ChatDialog::slot_to_chat_page" <<std::endl;
    ChatUserWidMgr[uid]->Refresh();
    //设置当前chatpage对应的chatuseritem
    _cur_chat_uid = uid;

    if(_state != ChatUiMode::SearchMode){
        _prev_state = _state;
    }
    _state = ChatUiMode::ChatMode;
    ShowSearch(false);
    ui->side_chat_lb->SetSelected(true);
    ui->side_contact_lb->ClearState();
}

void ChatDialog::slot_to_globalsearchpage()
{
    _global_search_dlg = std::make_shared<GlobalSearchPage>(this);
    QString text = ui->search_edit->text();
    if(!text.isEmpty()){
        _global_search_dlg->initEdit(text);
    }
    _global_search_dlg->show();
}

void ChatDialog::slot_to_downloadfile_dir()
{
    // 指定download默认路径
    QString defaultDir = "/home/jackzhou/Desktop/XchatSave/downloaddir";
    QString filePath = QFileDialog::getOpenFileName(
        this,                                  // 父窗口
        "选择文件",                              // 对话框标题
        defaultDir,                            // 默认路径
        "所有文件 (*.*);;文本文件 (*.txt)"         // 文件过滤器
        );
    if (!filePath.isEmpty()) {
        qDebug() << "选择的文件:" << filePath;
    }
}

void ChatDialog::slot_searchfinished(QVector<DbUserInfo> infos)
{
    for(auto& info: infos){
        _global_search_dlg->addUserItem(info);
    }
}

void ChatDialog::slot_updatemyapply(MyApplyRspInfo info)
{
    ui->side_contact_lb->ShowRedPoint();
    ui->con_user_list->addMyApplysSubItem(info);
}

