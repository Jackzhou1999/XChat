#ifndef REGISTEDIALOG_H
#define REGISTEDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class RegisteDialog;
}

class RegisteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisteDialog(QWidget *parent = nullptr);
    ~RegisteDialog();

private:
    void showTip(const QString&, bool);
    void initHttpHandlers();

    bool checkEmailFormat();
    bool checkUsernameValid();
    bool checkPasswdValid();
    bool checkPasswdConfirmValid();
    bool checkVerifyCodeValid();

    void AddTipErr(TipErr e, QString tip);
    void DelTipErr(TipErr e);
    void ChangeTipPage();
    void timerCallback();
private slots:
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCode err);

    void on_getcode_btn_clicked();
    void on_cancel_btn_clicked();
    void on_confirm_btn_clicked();
    void on_passwdword_btn_clicked();
    void on_passwordconform_btn_clicked();

    void on_return_button_clicked();

signals:
    void sig_backtologinpage();

private:
    Ui::RegisteDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    QMap<TipErr, QString> _tip_errs;
    QTimer* _countdown_timer;
    size_t _duration;

};

#endif // REGISTEDIALOG_H
