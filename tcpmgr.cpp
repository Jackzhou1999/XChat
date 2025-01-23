#include "tcpmgr.h"
#include <chrono>
#include <QJsonDocument>
#include "usermgr.h"
#include "sqlmgr.h"
#include "fileclient.h"
#include <QTimer>
#include <QtEndian>
#include <filesystem>

namespace fs = std::filesystem;
TcpMgr::TcpMgr(QObject *parent)
    : QObject{parent}
{
    _complete_recv_flag = true;
    _sonthread =  std::make_shared<WorkThread>();
    connect(&_socket, &QTcpSocket::connected, this,  [&]() {
           qDebug() << "Connected to server!";
            emit sig_connect_tcp_success(true);
    });

    connect(&_socket, &QTcpSocket::readyRead, this, &TcpMgr::slot_read_msg);

    connect(this, &TcpMgr::sig_readmsg_finished, this, &TcpMgr::slot_handle_msg);
    connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    connect(FileClient::GetInstance().get(), &FileClient::sig_downloadFinished, this, &TcpMgr::slot_new_msg_arrived);

    connect(&_socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
    });
    connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred), [&](QAbstractSocket::SocketError socketError) {
        Q_UNUSED(socketError)
        qDebug() << "Error:" << _socket.errorString();
    });
    initTcpHandlers();
    connect(FileClient::GetInstance().get(), &FileClient::sig_downloadFinished, this, &TcpMgr::slot_notifydownloadfinished);

}

void TcpMgr::initTcpHandlers()
{
    _handlers.insert(ReqId::ID_TCP_LOGIN, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);

        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            emit sig_login_tcp_failed(ErrorCode::ERROR_JSON);
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            qDebug() << "fail to parse QJsonDocument, no error element";
            emit sig_login_tcp_failed(ErrorCode::ERROR_JSON);
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCode::SUCCEE){
            emit sig_login_tcp_failed(err);
            return;
        }


        std::shared_ptr<semaphore> smp(new semaphore());

        if(jsonObj.contains("friend_requests")){
            QJsonArray friend_requests = jsonObj["friend_requests"].toArray();
            for (const QJsonValue &value : friend_requests) {
                auto apply_id = value["id"].toInt();
                auto sender_id = value["sender_id"].toInt();
                auto receiver_id = value["receiver_id"].toInt();
                auto message = value["message"].toString();
                auto status = value["status"].toInt();
                auto created_at = value["created_at"].toString();
                auto updated_at = value["updated_at"].toString();
                auto rejection_reason = value["rejection_reason"].toString();

                // 检查或格式化
                QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime.isValid()) {
                    qDebug() << "Invalid datetime format";
                } else {
                    created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
                }

                QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime2.isValid()) {
                    qDebug() << "Invalid datetime format";
                } else {
                    updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
                }
                DatabaseManager::insertFriendRequest(apply_id, sender_id, receiver_id, message, status, created_at, updated_at, rejection_reason, false);
            }
        }
        if(jsonObj.contains("friend_relationships")){
            QJsonArray friend_relationships = jsonObj["friend_relationships"].toArray();
            for(const QJsonValue &value : friend_relationships){
                auto user_id = value["user_id"].toInt();
                auto friend_id = value["friend_id"].toInt();
                auto created_at = value["created_at"].toString();
                auto updated_at = value["updated_at"].toString();

                // 检查或格式化
                QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime.isValid()) {
                    qDebug() << "Invalid datetime format";
                } else {
                    created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
                }

                QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime2.isValid()) {
                    qDebug() << "Invalid datetime format";
                } else {
                    updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
                }
                std::cout << "insert one Friend Relationship" << std::endl;
                DatabaseManager::insertFriendRelationship(user_id, friend_id, 1, created_at, updated_at);
            }
        }
        if(jsonObj.contains("chat_messages")){
            QJsonArray chat_messages = jsonObj["chat_messages"].toArray();
            for (const QJsonValue &value : chat_messages) {
                auto msg_id = value["id"].toInt();
                auto sender_id = value["sender_id"].toInt();
                auto receiver_id = value["receiver_id"].toInt();
                auto is_group = value["is_group"].toBool();
                auto content_type = value["content_type"].toInt();
                QString content = value["content"].toString();

                if(content_type == 2){
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
                    // 检查解析是否成功
                    if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                        qDebug() << "Invalid JSON format!";
                        return ;
                    }

                    QJsonObject jsonObj = jsonDoc.object();
                    // 访问 JSON 对象中的字段
                    uint64_t fileid = jsonObj.value("fileid").toInt();
                    QString filename = jsonObj.value("filename").toString();
                    uint64_t filesize = jsonObj.value("filesize").toInt();
                    QString filetype = jsonObj.value("filetype").toString();
                    QString filepath = UserMgr::GetInstance()->getUserHomeDir() + "/" + filename;
                    FileClient::GetInstance()->downloadFile(fileid, filepath);

                    _semaphores[fileid] = smp;
                    smp->registerTask(1);

                    QJsonObject msg;
                    msg["fileid"] = static_cast<int>(fileid);
                    msg["filename"] = filepath;
                    msg["filesize"] = static_cast<int>(filesize);
                    msg["filetype"] = filetype;
                    QJsonDocument doc(msg);
                    content = doc.toJson(QJsonDocument::Indented);

                }
                auto status = value["status"].toInt();
                auto created_at = value["created_at"].toString();
                auto updated_at = value["updated_at"].toString();

                // 检查或格式化
                QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime.isValid()) {
                    qDebug() << "Invalid datetime format";
                    continue;
                } else {
                    created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
                }

                QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime2.isValid()) {
                    qDebug() << "Invalid datetime format";
                    continue;
                } else {
                    updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
                }
                std::cout << "insert one Chat Message" << std::endl;
                DatabaseManager::insertChatMessage(msg_id, sender_id, receiver_id, is_group, content, content_type, status, created_at, updated_at);
            }
        }
        if(jsonObj.contains("user")){
            QJsonArray users = jsonObj["user"].toArray();
            for(const QJsonValue &value : users){
                auto uid = value["uid"].toInt();
                auto name = value["name"].toString();
                auto email = value["email"].toString();
                auto nick = value["nick"].toString();
                auto desc = value["desc"].toString();
                auto sex = value["sex"].toInt();


                QString icon = value["icon"].toString();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(icon.toUtf8());
                // 检查解析是否成功
                if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                    qDebug() << "Invalid JSON format!";
                    return;
                }

                QJsonObject jsonObj = jsonDoc.object();
                // 访问 JSON 对象中的字段
                uint64_t iconid = jsonObj.value("fileid").toInt();
                QString iconname = jsonObj.value("filename").toString();
                uint64_t iconsize = jsonObj.value("filesize").toInt();
                QString icontype = jsonObj.value("filetype").toString();
                qDebug() << "TcpMgr::initTcpHandlers ID_TCP_LOGIN " << iconid << " " << iconname;
                QString iconPath = UserMgr::GetInstance()->getUserHomeDir() + "/" + iconname;

                FileClient::GetInstance()->downloadFile(iconid, iconPath);
                _semaphores[iconid] = smp;
                smp->registerTask(1);
                DatabaseManager::insertUser(uid, name, email, nick, desc, sex, iconPath);
            }
        }

        _sonthread->PostTaskToQueue([this, smp](){
            smp->waitAllTaskFinish(std::chrono::milliseconds(5000));
            emit sig_login_tcp_success();
        });

        // QTimer::singleShot(3000, [this](){
        //     emit sig_login_tcp_success();
        // });

    });

    _handlers.insert(ReqId::ID_HEARTBEAT, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        std::cout << "_handlers.insert(ReqId::ID_HEARTBEAT" << std::endl;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            emit sig_login_tcp_failed(ErrorCode::ERROR_JSON);
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();


        auto now = std::chrono::system_clock::now();
        long long now_second = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        QJsonObject ret;
        ret["uid"] = UserMgr::GetInstance()->getUid_int();
        ret["timestamp"] = jsonObj["timestamp"].toInt();
        ret["timestamp_client"] = now_second;

        QJsonDocument doc(ret);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        emit sig_send_data(ReqId::ID_HEARTBEAT, jsonString);
    });


    // 处理新消息逻辑函数
    _handlers.insert(ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "new message coming!!!";
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();

        if(jsonObj["error"].toInt() == ErrorCode::SUCCEE){
            int id = jsonObj["msg_id"].toInt();
            int sender_id = jsonObj["sender_id"].toInt();
            int receiver_id = jsonObj["receiver_id"].toInt();
            int content_type = jsonObj["content_type"].toInt();
            QString content = jsonObj["content"].toString();
            QString url = jsonObj["url"].toString();

            if(content_type == 2){
                 QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
                // 检查解析是否成功
                if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                    qDebug() << "Invalid JSON format!";
                    return;
                }

                QJsonObject jsonObj = jsonDoc.object();
                // 访问 JSON 对象中的字段
                uint64_t fileid = jsonObj.value("fileid").toInt();
                QString filename = jsonObj.value("filename").toString();
                uint64_t filesize = jsonObj.value("filesize").toInt();
                QString filetype = jsonObj.value("filetype").toString();

                qDebug() << "_handlers.insert ReqId::ID_NOTIFY_TEXT_CHAT_MSG_REQ:"<< fileid << " " << filename;
                QString filepath = UserMgr::GetInstance()->getUserHomeDir() + "/" + filename;
                FileClient::GetInstance()->downloadFile(fileid, filepath);
                _file_to_id[fileid] = sender_id;

                QJsonObject msg;
                msg["fileid"] = static_cast<int>(fileid);
                msg["filename"] = filepath;
                msg["filesize"] = static_cast<int>(filesize);
                msg["filetype"] = filetype;
                QJsonDocument doc(msg);
                content = doc.toJson(QJsonDocument::Indented);
            }

            int created_at = jsonObj["created_at"].toInt();
            QDateTime dateTime = QDateTime::fromSecsSinceEpoch(created_at);
            QString created_at_str = dateTime.toString("yyyy-MM-dd HH:mm:ss");
            DatabaseManager::insertChatMessage(id, sender_id, receiver_id, false, content, content_type, 0, created_at_str, "");
            if(content_type != 2){
                emit sig_new_msg_arrived(sender_id);
            }
        }else{
            qDebug() << "_handlers ID_NOTIFY_TEXT_CHAT_MSG_REQ: wrong";
        }
    });


    _handlers.insert(ReqId::ID_MSG_BACK, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            qDebug() << "fail to create QJsonDocument, is null in _handlers.insert ReqId::ID_MSG_BACK";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();

        auto msg_id = jsonObj["id"].toInt();
        auto sender_id = jsonObj["sender_id"].toInt();
        auto receiver_id = jsonObj["receiver_id"].toInt();
        auto is_group = jsonObj["is_group"].toBool();
        QString content = jsonObj["content"].toString();
        int content_type = jsonObj["content_type"].toInt();

        if(content_type == 2){
            qDebug() << content;
            QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
            // 检查解析是否成功
            if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                qDebug() << "Invalid JSON format!";
                return;
            }

            QJsonObject contentObj = jsonDoc.object();
            // 访问 JSON 对象中的字段
            uint64_t fileid = contentObj["fileid"].toInt();
            QString filename = contentObj.value("filename").toString();
            uint64_t filesize = contentObj.value("filesize").toInt();
            QString filetype = contentObj.value("filetype").toString();
            QString filepath = jsonObj["filelocalpath"].toString();

            qDebug() << "_handlers.insert(ReqId::ID_MSG_BACK : " << filepath << " " << filename;
            FileClient::GetInstance()->uploadFile(fileid, filepath);

            QString destinationPath = UserMgr::GetInstance()->getUserHomeDir() + "/" + filename;      // 源文件路径
            QString sourcePath = filepath;   // 目标文件路径

            try {
                // 复制文件
                fs::copy(sourcePath.toStdString(), destinationPath.toStdString(), fs::copy_options::overwrite_existing);
            } catch (const fs::filesystem_error& e) {
                std::cerr << "文件复制失败: " << e.what() << std::endl;
            }


            QJsonObject msg;
            msg["fileid"] = static_cast<int>(fileid);
            msg["filename"] = destinationPath;
            msg["filesize"] = static_cast<int>(filesize);
            msg["filetype"] = filetype;
            QJsonDocument doc(msg);
            content = doc.toJson(QJsonDocument::Indented);
        }

        if(content_type == 3){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
            // 检查解析是否成功
            if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                qDebug() << "Invalid JSON format!";
                return;
            }

            QJsonObject contentObj = jsonDoc.object();
            // 访问 JSON 对象中的字段
            uint64_t fileid = contentObj["fileid"].toInt();
            QString filebubbleid = jsonObj["filebubbleid"].toString();
            emit sig_start_upload_file(fileid, filebubbleid);

        }
        auto status = jsonObj["status"].toInt();
        auto created_at = jsonObj["created_at"].toString();
        auto updated_at = jsonObj["updated_at"].toString();

        // 检查或格式化
        QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
        if (!dateTime.isValid()) {
            qDebug() << "Invalid datetime format";
            return;
        } else {
            created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
        }

        QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
        if (!dateTime2.isValid()) {
            qDebug() << "Invalid datetime format";
            return;
        } else {
            updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
        }
        DatabaseManager::insertChatMessage(msg_id, sender_id, receiver_id, is_group, content, content_type, status, created_at, updated_at);
    });


    _handlers.insert(ReqId::ID_HISTORY_MSG_RSP, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        qDebug() << "_handlers.insert(ReqId::ID_HISTORY_MSG_RSP : history message coming!!!";
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
           qDebug() << "fail to create QJsonDocument, is null";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("chat_messages")){
            qDebug() << "_handlers.insert(ReqId::ID_HISTORY_MSG_RSP : no history message";
            emit sig_server_data_run_out();
            return;
        }

        QJsonArray chat_messages = jsonObj["chat_messages"].toArray();
        int uid1 = jsonObj["uid1"].toInt();
        int uid2 = jsonObj["uid2"].toInt();
        int uid = (uid1 == UserMgr::GetInstance()->getUid_int()) ? uid2 : uid1;

        std::shared_ptr<semaphore> smp(new semaphore(0));

        if(chat_messages.size()){
            for (const QJsonValue &value : chat_messages) {
                auto msg_id = value["id"].toInt();
                auto sender_id = value["sender_id"].toInt();
                auto receiver_id = value["receiver_id"].toInt();
                auto is_group = value["is_group"].toBool();
                auto content = value["content"].toString();
                auto content_type = value["content_type"].toInt();
                if(content_type == 2){
                    qDebug() << content;
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(content.toUtf8());
                    // 检查解析是否成功
                    if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                        qDebug() << "Invalid JSON format!";
                        return;
                    }

                    QJsonObject jsonObj = jsonDoc.object();
                    // 访问 JSON 对象中的字段
                    uint64_t fileid = jsonObj.value("fileid").toInt();
                    QString filename = jsonObj.value("filename").toString();
                    uint64_t filesize = jsonObj.value("filesize").toInt();
                    QString filetype = jsonObj.value("filetype").toString();
                    QString filepath = UserMgr::GetInstance()->getUserHomeDir() + "/" + filename;

                    FileClient::GetInstance()->downloadFile(fileid, filepath);

                    _semaphores[fileid] = smp;
                    smp->registerTask(1);

                    QJsonObject msg;
                    msg["fileid"] = static_cast<int>(fileid);
                    msg["filename"] = filepath;
                    msg["filesize"] = static_cast<int>(filesize);
                    msg["filetype"] = filetype;
                    QJsonDocument doc(msg);
                    content = doc.toJson(QJsonDocument::Indented);
                }
                auto status = value["status"].toInt();
                auto created_at = value["created_at"].toString();
                auto updated_at = value["updated_at"].toString();

                // 检查或格式化
                QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime.isValid()) {
                    qDebug() << "Invalid datetime format";
                    continue;
                } else {
                    created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
                }

                QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
                if (!dateTime2.isValid()) {
                    qDebug() << "Invalid datetime format";
                    continue;
                } else {
                    updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
                }
                qDebug() << "_handlers.insert ReqId::ID_HISTORY_MSG_RSP 插入一条数据" << msg_id;
                DatabaseManager::insertChatMessage(msg_id, sender_id, receiver_id, is_group, content, content_type, status, created_at, updated_at);
            }

            _sonthread->PostTaskToQueue([this, smp, uid](){
                smp->waitAllTaskFinish(std::chrono::milliseconds(5000));
                emit sig_history_msg_arrived(uid);
            });

            // emit sig_history_msg_arrived(uid);
        }else{
            emit sig_server_data_run_out();
        }
    });

    _handlers.insert(ReqId::ID_USER_SEARCH, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            emit sig_login_tcp_failed(ErrorCode::ERROR_JSON);
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        QVector<DbUserInfo> infos;
        if(jsonObj.contains("user")){
            QJsonArray users = jsonObj["user"].toArray();
            int count = - users.size();
            std::cout << "tcpmgr ReqId::ID_USER_SEARCH:" << count << std::endl;
            std::shared_ptr<semaphore> smp(new semaphore(count));
            for(const QJsonValue &value : users){
                auto uid = value["uid"].toInt();
                auto name = value["name"].toString();
                auto email = value["email"].toString();
                auto nick = value["nick"].toString();
                auto desc = value["desc"].toString();
                auto sex = value["sex"].toInt();
                QString icon = value["icon"].toString();

                DbUserInfo info;
                info.uid = uid;
                info.name = name;
                info.email = email;
                info.nick = nick;
                QJsonDocument jsonDoc = QJsonDocument::fromJson(icon.toUtf8());
                // 检查解析是否成功
                if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
                    qDebug() << "Invalid JSON format!";
                    return;
                }

                QJsonObject jsonObj = jsonDoc.object();
                // 访问 JSON 对象中的字段
                uint64_t iconid = jsonObj.value("fileid").toInt();
                QString iconname = jsonObj.value("filename").toString();
                uint64_t iconsize = jsonObj.value("filesize").toInt();
                QString icontype = jsonObj.value("filetype").toString();
                QString iconPath = UserMgr::GetInstance()->getUserHomeDir() + "/" + iconname;
                info.icon = iconPath;
                infos.push_back(info);
                _semaphores[iconid] = smp;
                FileClient::GetInstance()->downloadFile(iconid, iconPath);
                DatabaseManager::insertUser(uid, name, email, nick, desc, sex, iconPath);
            }
            _sonthread->PostTaskToQueue([this, smp, infos](){
                smp->waitAllTaskFinish(std::chrono::milliseconds(1000));
                emit sig_searchfinished(infos);
            });

        }

    });

    _handlers.insert(ReqId::ID_NOTIFY_ADD_FRIEND_RSP, [this](ReqId id, uint16_t len, QByteArray data){
        Q_UNUSED(len);
        std::cout << "_handlers.insert(ReqId::ID_HEARTBEAT" << std::endl;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            emit sig_login_tcp_failed(ErrorCode::ERROR_JSON);
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }
        QJsonObject value = jsonDoc.object();
        auto apply_id = value["id"].toInt();
        auto sender_id = value["sender_id"].toInt();
        auto receiver_id = value["receiver_id"].toInt();
        auto message = value["message"].toString();
        auto status = value["status"].toInt();
        auto created_at = value["created_at"].toString();
        auto updated_at = value["updated_at"].toString();
        auto rejection_reason = "";

        // 检查或格式化
        QDateTime dateTime = QDateTime::fromString(created_at, "yyyy-MM-dd HH:mm:ss");
        if (!dateTime.isValid()) {
            qDebug() << "Invalid datetime format";
        } else {
            created_at = dateTime.toString("yyyy-MM-dd HH:mm:ss");
        }

        QDateTime dateTime2 = QDateTime::fromString(updated_at, "yyyy-MM-dd HH:mm:ss");
        if (!dateTime2.isValid()) {
            qDebug() << "Invalid datetime format";
        } else {
            updated_at = dateTime2.toString("yyyy-MM-dd HH:mm:ss");
        }
        DatabaseManager::insertFriendRequest(apply_id, sender_id, receiver_id, message, status, created_at, updated_at, rejection_reason, false);

        DbUserInfo receiver_info = DatabaseManager::getUserInfoByUid(receiver_id).value();

        MyApplyRspInfo info(receiver_id, receiver_info.name, receiver_info.icon, status);

        emit sig_updatefriendrequest(info);
    });
}

void TcpMgr::slot_connect_tcp(UserInfo user)
{
    _host = user.host;
    _port = user.port.toInt();
    qDebug() << " in TcpMgr::slot_connect_tcp: " << _host << " " << _port;
    _socket.connectToHost(_host, _port);
    qDebug() << "start to connect host";
}

void TcpMgr::slot_read_msg()
{
    _buffer.append(_socket.readAll());
    while (true) {
        if(_complete_recv_flag == true){
            if(_buffer.size() < sizeof(quint16) * 2){
                return;
            }
            auto id = _buffer.mid(0, sizeof(quint16));
            _msg_id = static_cast<quint16>((static_cast<quint8>(id[0]) << 8) | static_cast<quint8>(id[1]));

            auto msg_len = _buffer.mid(sizeof(quint16), sizeof(quint16));
            _msg_lenght = static_cast<quint16>((static_cast<quint8>(msg_len[0]) << 8) | static_cast<quint8>(msg_len[1]));

            qDebug() << "头来:" << _msg_id <<" " << _msg_lenght;
            _buffer = _buffer.mid(sizeof(quint16) * 2);
        }
        if(_buffer.size() < _msg_lenght){
            qDebug() << _buffer;
            _complete_recv_flag = false;
            return;
        }
        _complete_recv_flag = true;
        QByteArray msg = _buffer.mid(0, _msg_lenght);
        _buffer = _buffer.mid(_msg_lenght);
        qDebug() << "REQID:" << _msg_id <<" "<<_msg_lenght<<QString(msg).size();
        emit sig_readmsg_finished(static_cast<ReqId>(_msg_id), _msg_lenght, msg);
    }
}

void TcpMgr::slot_handle_msg(ReqId msg_id, uint16_t msg_len, QByteArray msg)
{

    auto iter = _handlers.find(msg_id);
    if(iter == _handlers.end()){
        qDebug() << "cant find responding handler:" << msg_id;
        return;
    }
    iter.value()(msg_id, msg_len, msg);
}

void TcpMgr::slot_send_data(ReqId reqId, QString data)
{
    uint16_t id = reqId;

    // 将字符串转换为UTF-8编码的字节数组
    QByteArray dataBytes = data.toUtf8();
    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.size());
    qDebug() << id << " " << len << " " << data;

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);
    // 写入ID和长度
    out << id << len;
    // 添加字符串数据
    block.append(dataBytes);
    // 发送数据
    _socket.write(block);

}

void TcpMgr::slot_new_msg_arrived(uint64_t fileid){
    std::cout << "TcpMgr::slot_new_msg_arrived" << std::endl;
    if(_file_to_id.find(fileid) != _file_to_id.end()){
        emit sig_new_msg_arrived(_file_to_id[fileid]);
        _file_to_id.remove(fileid);
    }
}

void TcpMgr::slot_notifydownloadfinished(int fileid)
{
    if(_semaphores.find(fileid) != _semaphores.end()){
        _semaphores[fileid]->finishTask(1);
        _semaphores.remove(fileid);
    }
}
