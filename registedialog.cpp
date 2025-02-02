#include "registedialog.h"
#include "ui_registedialog.h"
#include "global.h"
#include "httpmgr.h"

RegisteDialog::RegisteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisteDialog)
    , _duration(5)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("XChat"));
    ui->err_tip->setProperty("state", "normal");
    repolish(ui->err_tip);
    _countdown_timer = new QTimer(this);

    QAction *userAction = new QAction(ui->user_edit);
    QPixmap pixmap(":/res/user.png");
    userAction->setIcon(QIcon(pixmap.scaled(40, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui->user_edit->addAction(userAction, QLineEdit::LeadingPosition);
    ui->user_edit->setPlaceholderText(tr("用户名"));
    ui->user_edit->setPlaceholderOffset(35, 9);
    QAction *clearuserAction = new QAction(ui->user_edit);
    clearuserAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->user_edit->addAction(clearuserAction, QLineEdit::TrailingPosition);
    connect(ui->user_edit, &CustomLineEdit::textChanged, [this, clearuserAction](const QString& text){
        if(!text.isEmpty()){
            clearuserAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearuserAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearuserAction, &QAction::triggered, [this, clearuserAction](){
        ui->user_edit->clear();
        clearuserAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->user_edit->clearFocus();
    });

    QAction *emailAction = new QAction(ui->email_edit);
    QPixmap pixmap2(":/res/email.png");
    emailAction->setIcon(QIcon(pixmap2.scaled(40, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui->email_edit->addAction(emailAction, QLineEdit::LeadingPosition);
    ui->email_edit->setPlaceholderText(tr("邮箱"));
    ui->email_edit->setPlaceholderOffset(35, 9);
    QAction *clearemailAction = new QAction(ui->email_edit);
    clearemailAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->email_edit->addAction(clearemailAction, QLineEdit::TrailingPosition);
    connect(ui->email_edit, &CustomLineEdit::textChanged, [this, clearemailAction](const QString& text){
        if(!text.isEmpty()){
            clearemailAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearemailAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearemailAction, &QAction::triggered, [this, clearemailAction](){
        ui->email_edit->clear();
        clearemailAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->email_edit->clearFocus();
    });


    QAction *passwordAction = new QAction(ui->password_edit);
    QPixmap pixmap3(":/res/password.png");
    passwordAction->setIcon(QIcon(pixmap3.scaled(40, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui->password_edit->addAction(passwordAction, QLineEdit::LeadingPosition);
    ui->password_edit->setPlaceholderText(tr("密码须包含字母和数字且长度大于6"));
    ui->password_edit->setPlaceholderOffset(35, 9);
    QAction * clearpasswordAction = new QAction(ui->password_edit);
    clearpasswordAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->password_edit->addAction(clearpasswordAction, QLineEdit::TrailingPosition);
    connect(ui->password_edit, &CustomLineEdit::textChanged, [this, clearpasswordAction](const QString& text){
        if(!text.isEmpty()){
            clearpasswordAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearpasswordAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearpasswordAction, &QAction::triggered, [this, clearpasswordAction](){
        ui->password_edit->clear();
        clearpasswordAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->password_edit->clearFocus();
    });

    QAction *passwordconfirmAction = new QAction(ui->passwordconfirm_edit);
    QPixmap pixmap4(":/res/password.png");
    passwordconfirmAction->setIcon(QIcon(pixmap4.scaled(40, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui->passwordconfirm_edit->addAction(passwordconfirmAction, QLineEdit::LeadingPosition);
    ui->passwordconfirm_edit->setPlaceholderText(tr("确认密码"));
    ui->passwordconfirm_edit->setPlaceholderOffset(35, 9);
    QAction * clearpasswordconfirmAction = new QAction(ui->passwordconfirm_edit);
    clearpasswordconfirmAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->passwordconfirm_edit->addAction(clearpasswordconfirmAction, QLineEdit::TrailingPosition);
    connect(ui->passwordconfirm_edit, &CustomLineEdit::textChanged, [this, clearpasswordconfirmAction](const QString& text){
        if(!text.isEmpty()){
            clearpasswordconfirmAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearpasswordconfirmAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearpasswordconfirmAction, &QAction::triggered, [this, clearpasswordconfirmAction](){
        ui->passwordconfirm_edit->clear();
        clearpasswordconfirmAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->passwordconfirm_edit->clearFocus();
    });

    QAction *varifyAction = new QAction(ui->varify_edit);
    QPixmap pixmap5(":/res/varify.png");
    varifyAction->setIcon(QIcon(pixmap5.scaled(40, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    ui->varify_edit->addAction(varifyAction, QLineEdit::LeadingPosition);
    ui->varify_edit->setPlaceholderText(tr("验证码"));
    ui->varify_edit->setPlaceholderOffset(35, 9);
    QAction * clearvarifyActionAction = new QAction(ui->varify_edit);
    clearvarifyActionAction->setIcon(QIcon(":/res/close_transparent.png"));
    ui->varify_edit->addAction(clearvarifyActionAction, QLineEdit::TrailingPosition);
    connect(ui->varify_edit, &CustomLineEdit::textChanged, [this, clearvarifyActionAction](const QString& text){
        if(!text.isEmpty()){
            clearvarifyActionAction->setIcon(QIcon(":/res/search_close.png"));
        }else{
            clearvarifyActionAction->setIcon(QIcon(":/res/close_transparent.png"));
        }
    });
    connect(clearvarifyActionAction, &QAction::triggered, [this, clearvarifyActionAction](){
        ui->varify_edit->clear();
        clearvarifyActionAction->setIcon(QIcon(":/res/close_transparent.png"));
        ui->varify_edit->clearFocus();
    });


    ui->password_edit->setAlignment(Qt::AlignVCenter);
    ui->password_edit->setEchoMode(QLineEdit::Password);
    ui->passwordconfirm_edit->setEchoMode(QLineEdit::Password);
    ui->passwdword_btn->setCursor(Qt::PointingHandCursor);
    ui->passwordconform_btn->setCursor(Qt::PointingHandCursor);

    ui->confirm_btn->SetState("normal", "hover", "press");
    ui->cancel_btn->SetState("normal", "hover", "press");
    ui->getcode_btn->SetState("normal", "hover", "press");

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finished, this, &RegisteDialog::slot_reg_mod_finish);

    connect(ui->passwdword_btn, &EyeButton::clicked, this, &RegisteDialog::on_passwdword_btn_clicked);

    connect(ui->user_edit, &QLineEdit::editingFinished, this, &RegisteDialog::checkUsernameValid);
    connect(ui->email_edit, &QLineEdit::editingFinished, this, &RegisteDialog::checkEmailFormat);
    connect(ui->password_edit, &QLineEdit::editingFinished, this, &RegisteDialog::checkPasswdValid);
    connect(ui->passwordconfirm_edit, &QLineEdit::editingFinished, this, &RegisteDialog::checkPasswdConfirmValid);
    connect(ui->varify_edit, &QLineEdit::editingFinished, this, &RegisteDialog::checkVerifyCodeValid);

    connect(_countdown_timer, &QTimer::timeout, this, &RegisteDialog::timerCallback);
    initHttpHandlers();

}

RegisteDialog::~RegisteDialog()
{
    delete ui;
}

void RegisteDialog::showTip(const QString& s, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");

    }
    ui->err_tip->setText(s);
    repolish(ui->err_tip);
}

void RegisteDialog::initHttpHandlers()
{
    // 注册 获取验证码 回包逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj){

        int err = jsonObj["error"].toInt();
        if(err != ErrorCode::SUCCEE){
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱,注意查收"), true);
        qDebug()<<"email is "<<email<<Qt::endl;
    });

    // 注册 用户注册 回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](const QJsonObject& jsonObj){
        int err = jsonObj["error"].toInt();
        if(err != ErrorCode::SUCCEE){
            if(err == ErrorCode::ERROR_VERIFYCODEEXPIRE){
                showTip(tr("验证码已过期"), false);
                return;
            }
            if(err == ErrorCode::ERROR_VERIFYCODEUNMATCH){
                showTip(tr("验证码错误"), false);
                return;
            }
            if(err == ErrorCode::ERROR_USEREXIST){
                showTip(tr("该用户名已存在"), false);
                return;
            }
            if(err == ErrorCode::ERROR_EMAILHAVEBEENUSED){
                showTip(tr("该邮箱已注册过账号"), false);
                return;
            }
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        ChangeTipPage();
        qDebug()<<"email is "<<email<<Qt::endl;
    });
}

bool RegisteDialog::checkEmailFormat()
{
    auto email = ui->email_edit->text();
    static QRegularExpression regex("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
    bool valid = regex.match(email).hasMatch();
    if(email == "" || !valid){
        AddTipErr(TipErr::TIP_EMAIL_ERR, tr("邮箱格式错误"));
        return false;
    }
    DelTipErr(TipErr::TIP_EMAIL_ERR);
    return true;
}

bool RegisteDialog::checkUsernameValid()
{
    if(ui->user_edit->text() == ""){
        AddTipErr(TipErr::TIP_USER_ERR, tr("用户名不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisteDialog::checkPasswdValid()
{
    auto password = ui->password_edit->text();
    static QRegularExpression regex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).{6,}$");
    QRegularExpressionMatch match = regex.match(password);
    bool valid = match.hasMatch();
    if(password == "" || !valid){
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码不符合要求"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool RegisteDialog::checkPasswdConfirmValid()
{
    auto password = ui->password_edit->text();
    auto passwordConfirm = ui->passwordconfirm_edit->text();
    if(password != passwordConfirm){
        AddTipErr(TipErr::TIP_PWD_CONFIRM, tr("确认密码与密码不一致"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_CONFIRM);
    return true;
}

bool RegisteDialog::checkVerifyCodeValid()
{
    if(ui->varify_edit->text() == ""){
        AddTipErr(TipErr::TIP_VARIFY_ERR, tr("验证码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_VARIFY_ERR);
    return true;
}

void RegisteDialog::AddTipErr(TipErr e, QString tip)
{
    _tip_errs[e] = tip;
    showTip(tip, false);
}

void RegisteDialog::DelTipErr(TipErr e)
{
    ui->err_tip->clear();
    _tip_errs.remove(e);
    if(!_tip_errs.empty())
        showTip(_tip_errs.first(), false);
    return;
}

void RegisteDialog::ChangeTipPage()
{
    _countdown_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    _countdown_timer->start(1000);
}

void RegisteDialog::timerCallback()
{
    if(_duration <= 0){
        _duration = 5;
        _countdown_timer->stop();
        emit sig_backtologinpage();
        return;
    }
    --_duration;
    auto str = QString("注册成功，%1 s后返回登陆").arg(_duration);
    ui->tip_label->setText(str);
    return;
}
//反序列化HttpMgr
void RegisteDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCode err)
{

    if(err != ErrorCode::SUCCEE){
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析JSON字符串, res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"), false);
        return;
    }
    // json 解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析失败"), false);
        return;
    }

    _handlers[id](jsonDoc.object());
    return;
}

void RegisteDialog::on_getcode_btn_clicked()
{
    if(checkEmailFormat()){
        QJsonObject json_obj;
        json_obj["email"] = ui->email_edit->text();
        qDebug() << gate_url_prefix;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"), json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    }else{
        ui->email_edit->clear();
    }
}


void RegisteDialog::on_cancel_btn_clicked()
{
    emit sig_backtologinpage();
}


void RegisteDialog::on_confirm_btn_clicked()
{
    if(!checkUsernameValid()){
        return;
    }
    if(!checkEmailFormat()){
        return;
    }
    if(!checkPasswdValid()){
        return;
    }
    if(!checkPasswdConfirmValid()){
        return;
    }
    if(!checkVerifyCodeValid()){
        return;
    }

    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->password_edit->text();
    json_obj["verifycode"] = ui->varify_edit->text();
    qDebug() << "send meg";
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_register"), json_obj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
}

void RegisteDialog::on_passwdword_btn_clicked()
{
    if(ui->passwdword_btn->IsVisiable() == false){
        ui->password_edit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->password_edit->setEchoMode(QLineEdit::Password);
    }

}

void RegisteDialog::on_passwordconform_btn_clicked()
{
    if(ui->passwordconform_btn->IsVisiable() == false){
        ui->passwordconfirm_edit->setEchoMode(QLineEdit::Normal);
    }else{
        ui->passwordconfirm_edit->setEchoMode(QLineEdit::Password);
    }
}


void RegisteDialog::on_return_button_clicked()
{
    _duration = 5;
    _countdown_timer->stop();;
    emit sig_backtologinpage();
}

