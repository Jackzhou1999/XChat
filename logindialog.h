#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "global.h"
#include <QTimer>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
private:
    void initHttpHandlers();
    bool checkEmailFormat();
    bool checkPasswdValid();
    void timerCallback();
    void AddTipErr(TipErr e, QString tip);
    void DelTipErr(TipErr e);
    void showTip(const QString&, bool);

    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    Ui::LoginDialog *ui;
    QTimer* _countdown_timer;
    size_t _duration;

    int _uid;
    QString _token;
private slots:
    void slot_forget_passwd();
    void slot_log_mod_finish(ReqId id, QString res, ErrorCode err);
    void on_login_pushButton_clicked();

    void slot_connect_tcp_success(bool);
    void slot_login_tcp_success();
    void slot_login_tcp_failed(int);

signals:
    void switch_registion_page();
    void switch_forgetpwd_page();
    void switch_chat_page();
    void sig_connect_tcp(UserInfo user);
};

#endif // LOGINDIALOG_H
