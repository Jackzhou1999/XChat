#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include "global.h"
#include <QString>
#include <QObject>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>

class HttpMgr:public QObject, public Singleton<HttpMgr>, public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
    friend class Singleton<HttpMgr>;
public:
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);
    ~HttpMgr();
private:
    HttpMgr();
    QNetworkAccessManager _manage;
private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCode err, Modules mod);
signals:
    void sig_http_finished(ReqId id, QString res, ErrorCode err, Modules mod);
    void sig_reg_mod_finished(ReqId id, QString res, ErrorCode err);
    void sig_reset_mod_finished(ReqId id, QString res, ErrorCode err);
    void sig_log_mod_finished(ReqId id, QString res, ErrorCode err);

};

#endif // HTTPMGR_H
