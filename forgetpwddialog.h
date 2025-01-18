#ifndef FORGETPWDDIALOG_H
#define FORGETPWDDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ForgetPwdDialog;
}



class ForgetPwdDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ForgetPwdDialog(QWidget *parent = nullptr);
    ~ForgetPwdDialog();

private:
    void showTip(const QString&, bool);
    void initHttpHandlers();

    bool checkEmailFormat();
    bool checkUsernameValid();
    bool checkPasswdValid();
    bool checkVerifyCodeValid();

    void AddTipErr(TipErr e, QString tip);
    void DelTipErr(TipErr e);
    void ChangeTipPage();
    void timerCallback();
private slots:
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCode err);

    void on_getcode_btn_clicked();
    void on_cancel_btn_clicked();
    void on_confirm_btn_clicked();
    void on_passwdword_btn_clicked();

    void on_return_button_clicked();

signals:
    void sig_backtologinpage();

private:
    Ui::ForgetPwdDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    QTimer* _countdown_timer;
    size_t _duration;

};


#endif // FORGETPWDDIALOG_H
