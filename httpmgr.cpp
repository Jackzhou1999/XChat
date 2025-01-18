#include "httpmgr.h"


void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    std::weak_ptr<HttpMgr> self = shared_from_this();
    QNetworkReply *reply = _manage.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [self, reply, req_id, mod](){
        auto strongself = self.lock();
        if(strongself != nullptr){
            if(reply->error() != QNetworkReply::NoError){
                qDebug() << reply->errorString();
                emit strongself->sig_http_finished(req_id, "", ErrorCode::ERR_NETWORK, mod);
                reply->deleteLater();
                return;
            }

            QString res = reply->readAll();
            emit strongself->sig_http_finished(req_id, res, ErrorCode::SUCCEE, mod);
            reply->deleteLater();
            return;
        }
    });
}

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr() {
    connect(this, &HttpMgr::sig_http_finished, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCode err, Modules mod)
{
    switch (mod) {
        case Modules::REGISTERMOD:
            emit sig_reg_mod_finished(id, res, err);
            break;
        case Modules::RESETMOD:
            emit sig_reset_mod_finished(id, res, err);
            break;
        case Modules::LOGINMOD:
            emit sig_log_mod_finished(id, res, err);
            break;

        default:
            break;
    }
}
