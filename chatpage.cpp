#include "chatpage.h"
#include "ui_chatpage.h"
#include <QStyleOption>
#include <QPainter>
#include <QJsonArray>
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"
#include <QJsonDocument>
#include <QVector>
#include "usermgr.h"
#include "sqlmgr.h"
#include "filebubble.h"
#include <iostream>
#include "tcpmgr.h"
#include <QUuid>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
    , _server_data_run_out(false)
{
    ui->setupUi(this);
    ui->send_btn->SetState("normal", "hover", "press");
    ui->emo_lb->SetState("normal", "hover", "press", "normal", "hover", "press");
    ui->file_lb->SetState("normal", "hover", "press", "normal", "hover", "press");
    _loadingScreen = new LoadingDialog(this);

    connect(ui->chat_data_list, &ChatView::sig_load_history_msg, this, &ChatPage::slot_load_history_msg);
    connect(this, &ChatPage::sig_load_history_msg_finish, ui->chat_data_list, &ChatView::slot_load_history_msg_finish);
    connect(this, &ChatPage::sig_set_scrollbar_to_bottom, ui->chat_data_list, &ChatView::slot_set_scrollbar_to_bottom);
    connect(ui->chat_data_list, &ChatView::sig_history_msg_arrived, this, &ChatPage::slot_history_msg_from_server_arrived);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_server_data_run_out, this, &ChatPage::slot_server_data_run_out);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::SetUserInfo(int uid)
{
    //数据库接口
    //查询好友关系表,对应uid对应的用户信息
    _peeruid = uid;
    _peerinfo = DatabaseManager::getUserInfoByUid(uid);
    _timestamp.clear();
    _server_data_run_out = false;
    //设置ui界面
    ui->title_lb->setText(_peerinfo.name);
    ui->chat_data_list->removeAllItem();

    //数据库接口
    //查询聊天记录表,和uid的聊天记录QVector<packet>
    int myid = UserMgr::GetInstance()->getUid_int();
    std::deque<ChatMessage> packets = DatabaseManager::getChatMessages(myid, uid, 20, _timestamp);
    if(packets.empty()){
        QJsonObject ret;
        ret["uid1"] = uid;
        ret["uid2"] = myid;
        ret["timestamp"] = _timestamp;
        QJsonDocument doc(ret);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        qDebug() << "ChatPage::SetUserInfo 发送的1017";

        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_HISTORY_MSG_REQ, jsonString);
        return;
    }

    int location = 0;
    std::vector<int> read_msg_ids;
    while(!packets.empty()){
        auto msg = packets.front();
        packets.pop_front();
        if(_chatuserwid->unread_msg_ids.find(msg.id) != _chatuserwid->unread_msg_ids.end()){
            read_msg_ids.push_back(msg.id);
            _chatuserwid->unread_msg_ids.remove(msg.id);
            DatabaseManager::setMessageStatusToRead(msg.id);
            _chatuserwid->SetUnreadCount(_chatuserwid->unread_msg_ids.size());
        }
        AppendChatMsg(msg, location);
        location++;
    }

    if(!read_msg_ids.empty()){
        qDebug() << "ChatPage::SetUserInfo:发送已读信息id";
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
    }
    emit sig_set_scrollbar_to_bottom();
}


void ChatPage::AppendChatMsg(const ChatMessage &msg, int location)
{
    QString name;
    QString icon;
    ChatRole role;
    if(msg.sender_id == _peeruid){
        name = _peerinfo.name;
        icon = _peerinfo.icon;
        role = ChatRole::Other;
    }else{
        name = UserMgr::GetInstance()->getname();
        icon = UserMgr::GetInstance()->getIcon();
        role = ChatRole::Self;
    }

    if(msg.content_type == 1){
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->setUserName(name);
        pChatItem->setUserIcon(QPixmap(icon));
        QWidget* pBubble = new TextBubble(role, msg.content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->insertChatItem(pChatItem, location);
    }else if(msg.content_type == 2){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.content.toUtf8());
        const QJsonObject obj = jsonDoc.object();
        QString filename = obj["filename"].toString();

        ImageBubble *imageBubble = new ImageBubble(name, QPixmap(icon), QPixmap(filename), (role==ChatRole::Self));
        connect(imageBubble, &ImageBubble::imageDoubleClicked, this, &ChatPage::slot_show_imageviewer);
        ui->chat_data_list->insertChatItem(imageBubble, location);
    }else{

        QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.content.toUtf8());
        const QJsonObject obj = jsonDoc.object();
        QString filename = obj["filename"].toString();
        int64_t filesize = obj["filesize"].toInt();
        QString filetype = obj["filetype"].toString();
        uint64_t fileid = obj["fileid"].toInt();
        QString filepath = UserMgr::GetInstance()->getDownloadDir() + "/" + filename;
        FileBubble* sentBubble = new FileBubble(role, fileid, filename, filesize, filetype, name , QPixmap(icon), filepath);
        ui->chat_data_list->insertChatItem(sentBubble, location);
    }
}

void ChatPage::AppendChatMsg(const ChatMessage &msg)
{
    QString name;
    QString icon;
    ChatRole role;
    if(msg.sender_id == _peeruid){
        name = _peerinfo.name;
        icon = _peerinfo.icon;
        role = ChatRole::Other;
    }else{
        name = UserMgr::GetInstance()->getname();
        icon = UserMgr::GetInstance()->getIcon();
        role = ChatRole::Self;
    }

    if(msg.content_type == 1){
        ChatItemBase* pChatItem = new ChatItemBase(role);

        pChatItem->setUserName(name);
        pChatItem->setUserIcon(QPixmap(icon));
        QWidget* pBubble = new TextBubble(role, msg.content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }else if(msg.content_type == 2){
        QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.content.toUtf8());
        const QJsonObject obj = jsonDoc.object();
        QString filename = obj["filename"].toString();

        ImageBubble *imageBubble = new ImageBubble(name, QPixmap(icon), QPixmap(filename), (role==ChatRole::Self));
        connect(imageBubble, &ImageBubble::imageDoubleClicked, this, &ChatPage::slot_show_imageviewer);
        ui->chat_data_list->appendChatItem(imageBubble);
    }else{
        QJsonDocument jsonDoc = QJsonDocument::fromJson(msg.content.toUtf8());
        const QJsonObject obj = jsonDoc.object();

        QString filename = obj["filename"].toString();
        int64_t filesize = obj["filesize"].toInt();
        QString filetype = obj["filetype"].toString();
        uint64_t fileid = obj["fileid"].toInt();
        QString filepath = UserMgr::GetInstance()->getDownloadDir() + "/" + filename;
        FileBubble* sentBubble = new FileBubble(role, fileid, filename, filesize, filetype, name , QPixmap(icon), filepath);
        ui->chat_data_list->appendChatItem(sentBubble);
    }
    emit sig_set_scrollbar_to_bottom();
}

void ChatPage::on_send_btn_clicked()
{

    auto pTextEdit = ui->chatedit;
    ChatRole role = ChatRole::Self;
    QString userName = UserMgr::GetInstance()->getname();
    QString userIcon = UserMgr::GetInstance()->getIcon();
    int myid = UserMgr::GetInstance()->getUid_int();

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;
    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));

        QWidget *textBubble = nullptr;
        ImageBubble *imageBubble = nullptr;
        FileBubble* fileBubble = nullptr;

        QJsonObject ret;
        ret["sender_id"] = myid;
        ret["receiver_id"] = _peeruid;
        ret["is_group"] = false;

        QDateTime currentDateTime = QDateTime::currentDateTime();
        ret["timestamp"] = currentDateTime.toSecsSinceEpoch();

        ChatMessage msg;
        msg.created_at = currentDateTime;

        if(type == "text")
        {
            qDebug() << "text:" << msgList[i].content;
            msg.content_type = 1;
            msg.content = msgList[i].content;

            ret["content_type"] = 1;
            ret["content"] = msgList[i].content;

            textBubble = new TextBubble(role, msgList[i].content);
        }
        else if(type == "image")
        {
            qDebug() << "image:" << msgList[i].content;
            ret["content_type"] = 2;

            QString filepath = QUrl::fromPercentEncoding(msgList[i].content.toUtf8());
            QFileInfo fileinfo(filepath);
            QString filename = fileinfo.fileName();
            QString filetype = fileinfo.suffix();
            qint64 filesize = fileinfo.size();

            msg.content_type = 2;
            msg.content = filename;

            imageBubble = new ImageBubble(userName, QPixmap(userIcon), QPixmap(msgList[i].content), true);
            connect(imageBubble, &ImageBubble::imageDoubleClicked, this, &ChatPage::slot_show_imageviewer);

            QFile file(filepath);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << "Could not open file:" << file.errorString();
                return;
            }

            QCryptographicHash hash(QCryptographicHash::Md5);
            if (!hash.addData(&file)) {
                qWarning() << "Failed to read data from file:" << filepath;
                return ;
            }

            QString filemd5 = hash.result().toHex();
            QJsonObject fileobj;
            fileobj["filename"] = filename;
            fileobj["filesize"] = filesize;
            fileobj["filetype"] = filetype;
            fileobj["filelocalpath"] = filepath;
            fileobj["hash"] = filemd5;

            ret["content"] = fileobj;
        }
        else if(type == "file")
        {
            qDebug() << "file:" << msgList[i].content;
            ret["content_type"] = 3;
            QString filepath = QUrl::fromPercentEncoding(msgList[i].content.toUtf8());
            QFileInfo fileinfo(filepath);
            QString filename = fileinfo.fileName();
            QString filetype = fileinfo.suffix();
            qint64 filesize = fileinfo.size();

            msg.content_type = 3;
            msg.content = filename;

            QUuid id = QUuid::createUuid();
            QString strId = id.toString();


            QFile file(filepath);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << "Could not open file:" << file.errorString();
                return;
            }

            QCryptographicHash hash(QCryptographicHash::Md5);
            if (!hash.addData(&file)) {
                qWarning() << "Failed to read data from file:" << filepath;
                return ;
            }
            QString filemd5 = hash.result().toHex();

            QJsonObject fileobj;
            fileobj["filename"] = filename;
            fileobj["filesize"] = filesize;
            fileobj["filetype"] = filetype;
            fileobj["filelocalpath"] = filepath;
            fileobj["filebubbleid"] = strId;
            fileobj["hash"] = filemd5;

            ret["content"] = fileobj;

            fileBubble = new FileBubble(ChatRole::Self, filename, filesize, filetype, userName, QPixmap(userIcon), filepath);
            fileBubble->setMyId(strId);
            connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_start_upload_file, fileBubble, &FileBubble::slot_startuploadFile);
        }
        QJsonDocument doc(ret);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ, jsonString);

        //发送消息
        if(textBubble != nullptr){
            pChatItem->setWidget(textBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
        if(imageBubble != nullptr){
            ui->chat_data_list->appendChatItem(imageBubble);
        }
        if(fileBubble != nullptr){
            ui->chat_data_list->appendChatItem(fileBubble);
        }
        emit sig_set_scrollbar_to_bottom();

        _chatuserwid->UpdateInfo(msg);
    }
}

void ChatPage::slot_show_imageviewer(const QPixmap& image){
    qDebug() << "show iamge";
    _imageviewer = new ImageViewer();
    _imageviewer->setImage(image);
    _imageviewer->show();
}

void ChatPage::slot_load_history_msg(int oldScrollValue, int oldContentHeight)
{
    int myid = UserMgr::GetInstance()->getUid_int();
    qDebug() << "ChatPage::slot_load_history_msg 加载数据...," << _timestamp;
    std::deque<ChatMessage> packets = DatabaseManager::getChatMessages(myid, _peeruid, 20, _timestamp);
    std::vector<int> read_msg_ids;
    int location = 0;
    if(!packets.empty()){
        qDebug() << "load mas number:" << packets.size();
        if(!_chatuserwid->hasMsg()){
            _chatuserwid->Refresh();
        }
        while(!packets.empty()){
            auto msg = packets.front();
            packets.pop_front();
            if(_chatuserwid->unread_msg_ids.find(msg.id) != _chatuserwid->unread_msg_ids.end()){
                read_msg_ids.push_back(msg.id);
                _chatuserwid->unread_msg_ids.remove(msg.id);
                DatabaseManager::setMessageStatusToRead(msg.id);
                _chatuserwid->SetUnreadCount(_chatuserwid->unread_msg_ids.size());
            }
            AppendChatMsg(msg, location);
            location++;
        }
        emit sig_load_history_msg_finish(oldScrollValue,oldContentHeight);

        // 向服务器设置消息为已读
        if(!read_msg_ids.empty()){
            qDebug() << "ChatPage::slot_load_history_msg:发送已读信息id: ";
            QJsonObject ret;

            QJsonArray jsonArray;
            for(auto& id: read_msg_ids){
                std::cout << id << " ";
                jsonArray.append(id);
            }
            std::cout << std::endl;
            ret["msg_id"] = jsonArray;
            QJsonDocument doc(ret);
            QString jsonString = doc.toJson(QJsonDocument::Indented);
            //发送tcp请求给chat server
            emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_MSG_READ_RSP, jsonString);
        }

    }else{
        if(!_server_data_run_out){
            QJsonObject ret;
            ret["uid1"] = _peeruid;
            ret["uid2"] = myid;
            ret["timestamp"] = _timestamp;
            QJsonDocument doc(ret);
            QString jsonString = doc.toJson(QJsonDocument::Indented);
            //发送tcp请求给chat server
            qDebug() << "ChatPage::slot_load_history_msg 发送的1017";
            emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_HISTORY_MSG_REQ, jsonString);

            _loadingScreen->showCentered(this); // 在子界面中心显示加载界面
            QTimer::singleShot(300, this, &ChatPage::hideLoadingDialog); // 模拟加载0.2秒后隐藏
            return;
        }
    }
}

void ChatPage::slot_history_msg_from_server_arrived(int uid, int oldScrollValue, int oldContentHeight)
{
    if(uid != _peeruid) return;
    slot_load_history_msg(oldScrollValue, oldContentHeight);
}

void ChatPage::hideLoadingDialog() {
    _loadingScreen->hide();
}

void ChatPage::slot_server_data_run_out()
{
    _server_data_run_out = true;
}
