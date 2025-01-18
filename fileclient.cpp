#include "fileclient.h"
#include "utils.h"
#include <QJsonDocument>
#include <QFile>
#include <QFileInfo>

#include <chrono>
#include <thread>
#include <fstream>
#include "usermgr.h"

FileClient::FileClient(QObject *parent)
    : QObject{parent}, _threadNum(3)

{
    _host = "127.0.0.1";
    _port = 8100;
    initHandler();
    connect(this, &FileClient::sig_write_data, this, &FileClient::slot_write_data);
    connect(this, &FileClient::sig_close_session, this, &FileClient::slot_close_session);
    for(int i=0; i<_threadNum; i++){
        _threadPool.push_back(std::make_shared<WorkThread>());
    }
}


void FileClient::uploadFile(uint64_t fileid, const QString &filepath)
{
    std::shared_ptr<Session> session = std::make_shared<Session>(fileid, filepath);
    session->_send_thread_id = Index(_threadNum);
    session->_recv_thread_id = (session->_send_thread_id + 1) % _threadNum;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessionmap[fileid] = session;
    }
    session->_socket->connectToHost(_host, _port);
    std::weak_ptr<Session> weak_session(session);

    connect(session->_socket, &QTcpSocket::connected, [weak_session, this](){
        std::shared_ptr<Session> session = weak_session.lock();
        if(!session) return;
        onUploaded(session);
    });

    connect(session->_socket, &QTcpSocket::readyRead, [weak_session, this](){
        onRecved(weak_session);
    });

    connect(session->_socket, &QTcpSocket::disconnected, [weak_session, this](){
        std::shared_ptr<Session> session = weak_session.lock();
        if(!session) return;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if(_sessionmap.find(session->_fileid) != _sessionmap.end()){
                _sessionmap.remove(session->_fileid);
            }
        }
    });

    //连接 出错信号和槽函数
    connect(session->_socket, &QTcpSocket::errorOccurred,[weak_session, this](){
        std::shared_ptr<Session> session = weak_session.lock();
        if(!session) return;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if(_sessionmap.find(session->_fileid) != _sessionmap.end()){
                _sessionmap.remove(session->_fileid);
            }
        }
    });
}

void FileClient::slot_stopuploadFile(uint64_t fileid)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_sessionmap.find(fileid) != _sessionmap.end()){
            _sessionmap[fileid]->_stop = true;
        }
    }
}

void FileClient::downloadFile(uint64_t fileid, const QString &filename)
{
    std::shared_ptr<Session> session = std::make_shared<Session>(fileid, filename);
    session->_send_thread_id = Index(_threadNum);
    session->_recv_thread_id = (session->_send_thread_id + 1) % _threadNum;
    qDebug() << "下载文件:" << filename;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _sessionmap[fileid] = session;
    }
    std::weak_ptr<Session> weak_session(session);

    session->_socket->connectToHost(_host, _port);
    connect(session->_socket, &QTcpSocket::connected, [weak_session, this](){
        onDownloaded(weak_session);
    });

    connect(session->_socket, &QTcpSocket::readyRead, [weak_session, this](){
        onRecved(weak_session);
    });

    connect(session->_socket, &QTcpSocket::disconnected, [weak_session, this](){
        std::shared_ptr<Session> session = weak_session.lock();
        if(!session) return;
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if(_sessionmap.find(session->_fileid) != _sessionmap.end()){
                _sessionmap.remove(session->_fileid);
            }
        }
    });
}

void FileClient::allocater(ReqId reqid, std::shared_ptr<Session> session, QByteArray msg)
{
    switch (reqid) {
    case ReqId::ID_SEEK_FILE_LOCATION:
        _threadPool[session->_send_thread_id]->PostTaskToQueue(_handlers[ReqId::ID_SEEK_FILE_LOCATION], std::weak_ptr<Session>(session), msg);
        break;
    case ReqId::ID_UPLOAD_FILE_RSP:
        _threadPool[session->_recv_thread_id]->PostTaskToQueue(_handlers[ReqId::ID_UPLOAD_FILE_RSP], std::weak_ptr<Session>(session), msg);
        break;
    case ReqId::ID_DOWNLOAD_FILE_RSP:
        _threadPool[session->_send_thread_id]->PostTaskToQueue(_handlers[ReqId::ID_DOWNLOAD_FILE_RSP], std::weak_ptr<Session>(session), msg);
        break;
    case ReqId::ID_CLOSE_FILESESSION:
        _threadPool[session->_recv_thread_id]->PostTaskToQueue(_handlers[ReqId::ID_CLOSE_FILESESSION], std::weak_ptr<Session>(session), msg);
        break;
    default:
        qDebug() << "no coressponding handler";
        break;
    }
}


void FileClient::initHandler(){
    _handlers[ReqId::ID_SEEK_FILE_LOCATION] = [this](std::weak_ptr<Session> weak_session, QByteArray msg){
        std::shared_ptr<Session> session = weak_session.lock();
        if(session){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
            if(jsonDoc.isNull()){
                qDebug() << "fail to create QJsonDocument, is null";
                return;
            }

            QJsonObject jsonObj = jsonDoc.object();
            qint64 progress = jsonObj["progress"].toInt();
            qDebug() << "续传位置为:" << progress;
            qDebug() << session->_filename;
            session->_transfered_bytes = progress;
            QFile file(session->_filename);
            if (!file.open(QIODevice::ReadOnly)) {
                qWarning() << "Could not open file:" << file.errorString();
                return;
            }

            QCryptographicHash hash(QCryptographicHash::Md5);
            if (!hash.addData(&file)) {
                qWarning() << "Failed to read data from file:" << session->_filename;
                return ;
            }

            QString filemd5 = hash.result().toHex();

            file.seek(progress); // 定位到续传位置
            QByteArray buffer;
            int seq = 0;
            QFileInfo fileInfo(session->_filename); // 创建 QFileInfo 对象
            int total_size = fileInfo.size() - progress;
            QString filename = fileInfo.fileName();

            qDebug() << "剩余文件大小：" << total_size;
            int last_seq = 0;
            if(total_size % MAX_FILE_LEN){
                last_seq = (total_size/MAX_FILE_LEN)+1;
            }else{
                last_seq = total_size/MAX_FILE_LEN;
            }
            while (!file.atEnd() && !session->_stop) {
                //每次读取2048字节发送
                buffer = file.read(MAX_FILE_LEN);
                QJsonObject jsonObj;
                // 将文件内容转换为 Base64 编码（可选）
                QString base64Data = buffer.toBase64();
                ++seq;
                jsonObj["uid"] = UserMgr::GetInstance()->getUid_int();
                jsonObj["md5"] = filemd5;
                jsonObj["filename"] = filename;
                jsonObj["fileid"] = static_cast<int>(session->_fileid);
                jsonObj["seq"] = seq;
                jsonObj["trans_size"] = buffer.size() + (seq-1)*MAX_FILE_LEN;
                jsonObj["total_size"] = total_size;

                if(buffer.size() + (seq-1)*MAX_FILE_LEN == total_size){
                    jsonObj["last"] = 1;
                }else{
                    jsonObj["last"] = 0;
                }

                jsonObj["data"]= base64Data;
                jsonObj["last_seq"] = last_seq;
                QJsonDocument doc(jsonObj);
                QByteArray send_data = doc.toJson();
                emit sig_write_data(session, ReqId::ID_UPLOAD_FILE_REQ, send_data);
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
            if(session->_stop){
                emit sig_transferInterrupted(session->_fileid);
                emit sig_close_session(session);
            }
            //关闭文件
            file.close();
        }
    };

    _handlers[ReqId::ID_UPLOAD_FILE_RSP] = [this](std::weak_ptr<Session> weak_session, QByteArray msg){
        std::shared_ptr<Session> session = weak_session.lock();
        if(session){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
            if(jsonDoc.isNull()){
                qDebug() << "fail to create QJsonDocument, is null";
                return;
            }

            QJsonObject jsonObj = jsonDoc.object();
            int err = jsonObj["error"].toInt();
            if(err != ErrorCode::SUCCEE){
                emit sig_close_session(session);
            }

            int trans_size = jsonObj["trans_size"].toInt();
            int total_size = jsonObj["total_size"].toInt();
            qDebug() << "uploaded bytes:" << trans_size << "/" << total_size;
            emit sig_bytes_transfered(trans_size + session->_transfered_bytes);
            if(trans_size == total_size){
                qDebug() << "uploaded file: "<< session->_filename << " finished";
                emit sig_transferFinished(session->_fileid);
                emit sig_close_session(session);
            }
        }
    };

    _handlers[ReqId::ID_DOWNLOAD_FILE_RSP] = [this](std::weak_ptr<Session> weak_session, QByteArray msg){
        std::shared_ptr<Session> session = weak_session.lock();
        if(!session) return;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
        if(jsonDoc.isNull()){
            qDebug() << "fail to create QJsonDocument, is null";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        int err = jsonObj["error"].toInt();
        if(err != ErrorCode::SUCCEE){
            emit sig_close_session(session);
        }

        int uid = jsonObj["uid"].toInt();

        QByteArray data = QByteArray::fromBase64(jsonObj["data"].toString().toUtf8());
        QString filePath = session->_filename;
        qDebug() << filePath;
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            file.write(data);
            file.close();
        } else {
            qDebug() << "Failed to open file for writing.";
            emit sig_close_session(session);
        }


        int trans_size = jsonObj["trans_size"].toInt();
        int total_size = jsonObj["total_size"].toInt();
        qDebug() << "downloaded bytes:" << trans_size << "/" << total_size;

        QJsonObject reply;
        reply["reqid"] = ReqId::ID_DOWNLOAD_FILE_RSP_REPLY;
        reply["filepath"] = session->_filename;
        reply["fileid"] = static_cast<int>(session->_fileid);

        reply["uid"] = UserMgr::GetInstance()->getUid_int();
        reply["transferedBytes"] = trans_size;
        reply["totalBytes"] = total_size;
        QJsonDocument doc(reply);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        emit sig_write_data(session, ReqId::ID_DOWNLOAD_FILE_RSP_REPLY, jsonString);
        emit sig_bytes_transfered(trans_size);
        if(trans_size == total_size){
            qDebug() << "downloaded file: "<< session->_filename << " finished";
            emit sig_downloadFinished(session->_fileid);
        }
    };

    _handlers[ReqId::ID_CLOSE_FILESESSION] = [this](std::weak_ptr<Session> weak_session, QByteArray msg){
        std::shared_ptr<Session> session = weak_session.lock();
        if(session){
            QJsonDocument jsonDoc = QJsonDocument::fromJson(msg);
            if(jsonDoc.isNull()){
                qDebug() << "fail to create QJsonDocument, is null";
                return;
            }
            std::cout << "close session" << std::endl;
            emit sig_close_session(session);
        }
    };
}


void FileClient::onUploaded(std::weak_ptr<Session> weak_session)
{
    std::shared_ptr<Session> session = weak_session.lock();
    if(session){
        // 请求文件的上传进度
        QJsonObject jsonObj;
        jsonObj["reqid"] = ReqId::ID_SEEK_FILE_LOCATION;
        jsonObj["filename"] = session->_filename;
        jsonObj["fileid"] = static_cast<int>(session->_fileid);
        jsonObj["uid"] = UserMgr::GetInstance()->getUid_int();
        QJsonDocument doc(jsonObj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        emit sig_write_data(session, ReqId::ID_SEEK_FILE_LOCATION, jsonString);
    }
}

void FileClient::onRecved(std::weak_ptr<Session> weak_session)
{
    std::shared_ptr<Session> session = weak_session.lock();
    if(!session) return;

    session->_buffer.append(session->_socket->readAll());
    while (true) {
        if(session->_complete_recv_flag == true){
            if(session->_buffer.size() < sizeof(quint16) * 2){
                return;
            }

            auto id = session->_buffer.mid(0, sizeof(quint16));
            session->_msg_id = static_cast<quint16>((static_cast<quint8>(id[0]) << 8) | static_cast<quint8>(id[1]));

            auto msg_len = session->_buffer.mid(sizeof(quint16), sizeof(quint16));
            session->_msg_lenght = static_cast<quint16>((static_cast<quint8>(msg_len[0]) << 8) | static_cast<quint8>(msg_len[1]));

            session->_buffer = session->_buffer.mid(sizeof(quint16) * 2);
        }
        if(session->_buffer.size() < session->_msg_lenght){
            session->_complete_recv_flag = false;
            return;
        }
        session->_complete_recv_flag = true;
        QByteArray msg = session->_buffer.mid(0, session->_msg_lenght);
        session->_buffer = session->_buffer.mid(session->_msg_lenght);
        allocater(static_cast<ReqId>(session->_msg_id), session, msg);
    }

}

void FileClient::onDownloaded(std::weak_ptr<Session> weak_session)
{
    std::shared_ptr<Session> session = weak_session.lock();
    if(session){
        // 请求下载文件
        QJsonObject jsonObj;
        jsonObj["reqid"] = ReqId::ID_DOWNLOAD_FILE_REQ;
        jsonObj["filepath"] = session->_filename;
        jsonObj["fileid"] = static_cast<int>(session->_fileid);
        jsonObj["uid"] = UserMgr::GetInstance()->getUid_int();
        QJsonDocument doc(jsonObj);
        QByteArray jsonString = doc.toJson(QJsonDocument::Indented);
        emit sig_write_data(session, ReqId::ID_DOWNLOAD_FILE_REQ, jsonString);
    }
}

void FileClient::slot_write_data(std::weak_ptr<Session> weak_session, ReqId reqid, QByteArray dataBytes)
{
    std::shared_ptr<Session> session = weak_session.lock();
    if(!session) return;

    uint16_t id = reqid;
    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.size());
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
    session->_socket->write(block);
}

void FileClient::slot_close_session(std::weak_ptr<Session> weak_session)
{
    std::shared_ptr<Session> session = weak_session.lock();
    if(!session) return;
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if(_sessionmap.find(session->_fileid) != _sessionmap.end()){
            _sessionmap.remove(session->_fileid);
            std::cout << "FileClient::slot_close_session:" << session.use_count() << std::endl;
        }
    }
}

