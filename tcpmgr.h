#ifndef TCPMGR_H
#define TCPMGR_H

#include <QObject>
#include <singleton.h>
#include "global.h"
#include "userdata.h"
#include "Semaphore.h"
#include "workthread.h"
#include "contacttreewidget.h"

class TcpMgr : public QObject, public Singleton<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;
public:
    explicit TcpMgr(QObject *parent = nullptr);
private:
    QString _host;
    uint16_t _port;
    QTcpSocket _socket;
    QByteArray _buffer;
    bool _complete_recv_flag;
    uint16_t _msg_id;
    uint16_t _msg_lenght;
    QMap<uint64_t, int> _file_to_id;
    QMap<ReqId, std::function<void(ReqId, uint16_t, QByteArray)>> _handlers;

    std::shared_ptr<WorkThread> _sonthread;
    QMap<uint64_t, std::shared_ptr<semaphore>> _semaphores;

    void initTcpHandlers();

signals:
    void sig_connect_tcp_success(bool);
    void sig_login_tcp_success();
    void sig_login_tcp_failed(int);

    void sig_readable_event();
    void sig_readmsg_finished(ReqId, uint16_t, QByteArray);
    void sig_send_data(ReqId , QString);
    void sig_user_search(std::shared_ptr<SearchInfo>);

    void sig_new_msg_arrived(int uid);
    void sig_server_data_run_out();
    void sig_history_msg_arrived(int uid);

    void sig_start_upload_file(uint64_t fileid, QString filebubbleid);
    void sig_searchfinished(QVector<DbUserInfo>);
    void sig_updatefriendrequest(MyApplyRspInfo);
public slots:
    void slot_connect_tcp(UserInfo);
    void slot_read_msg();
    void slot_handle_msg(ReqId, uint16_t, QByteArray);
    void slot_send_data(ReqId , QString);
    void slot_new_msg_arrived(uint64_t fileid);

    void slot_notifydownloadfinished(int fileid);

};

#endif // TCPMGR_H
