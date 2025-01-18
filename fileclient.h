#ifndef FILECLIENT_H
#define FILECLIENT_H

#include "singleton.h"
#include <QString>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <functional>
#include <mutex>
#include "global.h"
#include <vector>
#include "workthread.h"

class Session
{
public:
    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;

    explicit Session(const QString& filename):_filename(filename), _socket(new QTcpSocket()), _complete_recv_flag(true), _stop(false){
        _buffer.reserve(100*1024*1024);
    }
    explicit Session(uint64_t fileid, const QString& filename):_fileid(fileid), _filename(filename), _socket(new QTcpSocket()), _complete_recv_flag(true), _stop(false){
        _buffer.reserve(100*1024*1024);
    }

    ~Session(){
        qDebug() << "release file session :" << _filename;
        _socket->deleteLater();
    }
    QTcpSocket* _socket;
    std::mutex _mutex_for_buffer;
    QByteArray _buffer;
    uint16_t _msg_id;
    uint16_t _msg_lenght;
    uint64_t _transfered_bytes;
    bool _complete_recv_flag;
    std::atomic<bool> _stop;
    uint _send_thread_id;
    uint _recv_thread_id;

    QString _filename;
    uint64_t _fileid;
};

class FileClient : public QObject, public Singleton<FileClient>
{
    Q_OBJECT
    friend class Singleton<FileClient>;
public:
    explicit FileClient(QObject *parent = nullptr);

public slots:
    void uploadFile(uint64_t fileid, const QString& filepath);
    void slot_stopuploadFile(uint64_t fileid);
    void downloadFile(uint64_t fileid, const QString& filename);
private:
    void allocater(ReqId reqid, std::shared_ptr<Session> session, QByteArray msg);
    void initHandler();
    void onRecved(std::weak_ptr<Session> weak_session);

    void onUploaded(std::weak_ptr<Session> weak_session);
    void onDownloaded(std::weak_ptr<Session> weak_session);

    QString _host;
    uint16_t _port;
    // ThreadPool _threadpool;
    std::vector<std::shared_ptr<WorkThread>> _threadPool;
    uint8_t _threadNum;
    std::mutex _mutex;
    QMap<uint64_t, std::shared_ptr<Session>> _sessionmap;
    QMap<ReqId, std::function<void(std::weak_ptr<Session> weak_session, QByteArray)>> _handlers;
signals:
    void sig_write_data(std::weak_ptr<Session> weak_session, ReqId reqid, QByteArray dataBytes);
    void sig_close_session(std::weak_ptr<Session> weak_session);
    void sig_bytes_transfered(int bytes);
    void sig_bytes_downloaded(int bytes);
    void sig_transferFinished(uint64_t fileid);
    void sig_transferInterrupted(uint64_t fileid);
    void sig_downloadFinished(uint64_t fileid);

private slots:
    void slot_write_data(std::weak_ptr<Session> weak_session, ReqId reqid, QByteArray dataBytes);
    void slot_close_session(std::weak_ptr<Session> weak_session);
};


#endif // FILECLIENT_H
