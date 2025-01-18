#include "logindialog.h"
#include "ui_logindialog.h"
#include "httpmgr.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include "fileclient.h"

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    ,_duration(5)
{
    ui->setupUi(this);
    _countdown_timer = new QTimer(this);
    connect(ui->regist_pushButton, &QPushButton::clicked, this, &LoginDialog::switch_registion_page);
    connect(ui->forgetpasswd_label, &ClickLabel::clicked, this, &LoginDialog::slot_forget_passwd);
    connect(_countdown_timer, &QTimer::timeout, this, &LoginDialog::timerCallback);

    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_log_mod_finished, this, &LoginDialog::slot_log_mod_finish);
    connect(this, &LoginDialog::sig_connect_tcp, TcpMgr::GetInstance().get(), &TcpMgr::slot_connect_tcp);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_connect_tcp_success, this, &LoginDialog::slot_connect_tcp_success);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_tcp_success, this, &LoginDialog::slot_login_tcp_success);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_tcp_failed, this, &LoginDialog::slot_login_tcp_failed);

    initHttpHandlers();
    ui->password_edit->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    this->setWindowTitle(tr("XChat"));

}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::initHttpHandlers()
{
    // 注册 用户登录 回包逻辑
    _handlers.insert(ReqId::ID_HTTP_LOGIN, [this](const QJsonObject& jsonObj){
        int err = jsonObj["error"].toInt();
        if(err != ErrorCode::SUCCEE){
            if(err == ErrorCode::ERROR_EMAILNOREGISTE){
                showTip(tr("该邮箱未注册过账号"), false);
                ui->login_pushButton->setEnabled(true);
                return;
            }
            if(err == ErrorCode::ERROR_PASSWDUNMATCH){
                showTip(tr("密码错误"), false);
                ui->login_pushButton->setEnabled(true);
                return;
            }
            showTip(tr("参数错误"), false);
            ui->login_pushButton->setEnabled(true);
            return;
        }

        UserInfo user;
        user.uid = jsonObj["uid"].toInt();
        user.host = jsonObj["host"].toString();
        user.port = jsonObj["port"].toString();
        user.token = jsonObj["token"].toString();
        QJsonObject allinfo = jsonObj.value("info").toObject();
        QString iconstr = allinfo.value("icon").toString();
        qDebug()  << iconstr;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(iconstr.toUtf8());
        // 检查解析是否成功
        if (jsonDoc.isNull() || (!jsonDoc.isObject() && !jsonDoc.isArray())) {
            qDebug() << "Invalid JSON format!";
            return ;
        }

        QJsonObject icon = jsonDoc.object();

        // 访问 JSON 对象中的字段
        uint64_t iconid = icon.value("fileid").toInt();
        QString iconname = icon.value("filename").toString();
        uint64_t iconsize = icon.value("filesize").toInt();
        QString icontype = icon.value("filetype").toString();

        qDebug() << "In LoginDialog::initHttpHandlers: " << user.port.toInt();
        showTip(tr("身份验证成功, 开始连接聊天服务器..."), true);
        emit sig_connect_tcp(user);
        _uid = user.uid;
        _token = user.token;

        qDebug()<<"uid is "<<user.uid<<Qt::endl;
        qDebug()<<"token is "<<user.token<<Qt::endl;
        qDebug()<<"host is "<<user.host<<Qt::endl;
        qDebug()<<"port is "<<user.port<<Qt::endl;
        UserMgr::GetInstance()->setUid(user.uid);
        UserMgr::GetInstance()->setToken(user.token);
        UserMgr::GetInstance()->setEmail(jsonObj["email"].toString());
        UserMgr::GetInstance()->setname(jsonObj["name"].toString());
        const QString download_path = "/home/jackzhou/Desktop/XchatSave/downloaddir";
        UserMgr::GetInstance()->setDownloadDir(download_path);

        QString homepath = UserMgr::GetInstance()->getUserHomeDir();
        QString iconpath = homepath + "/" + iconname;
        FileClient::GetInstance()->downloadFile(iconid, iconpath);
        UserMgr::GetInstance()->setIcon(iconpath);



        ui->login_pushButton->setEnabled(true);
    });
}

void LoginDialog::slot_forget_passwd()
{
    qDebug() << "slot forget pwd";
    emit switch_forgetpwd_page();
}

void LoginDialog::slot_log_mod_finish(ReqId id, QString res, ErrorCode err)
{
    if(err != ErrorCode::SUCCEE){
        showTip(tr("网络请求错误"), false);
        ui->login_pushButton->setEnabled(true);
        return;
    }
    //解析JSON字符串, res转化为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("json解析失败"), false);
        ui->login_pushButton->setEnabled(true);
        return;
    }
    // json 解析错误
    if(!jsonDoc.isObject()){
        showTip(tr("json解析失败"), false);
        ui->login_pushButton->setEnabled(true);
        return;
    }
    _handlers[id](jsonDoc.object());    
    return;
}

void LoginDialog::on_login_pushButton_clicked()
{
    if(!checkEmailFormat()){
        return;
    }
    if(!checkPasswdValid()){
        return;
    }

    QJsonObject json_obj;
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->password_edit->text();
    qDebug() << "send login msg";
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/user_login"), json_obj, ReqId::ID_HTTP_LOGIN, Modules::LOGINMOD);
    ui->login_pushButton->setEnabled(false);

}

void LoginDialog::slot_connect_tcp_success(bool success)
{
    if(success){
        showTip(tr("聊天服务连接成功，正在登录..."),true);
        QJsonObject jsonObj;
        jsonObj["uid"] = _uid;
        jsonObj["token"] = _token;
        QJsonDocument doc(jsonObj);
        QString jsonString = doc.toJson(QJsonDocument::Indented);
        //发送tcp请求给chat server
        emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TCP_LOGIN, jsonString);
    }else{
        showTip(tr("网络异常"),false);
        ui->login_pushButton->setEnabled(true);
    }
}

void LoginDialog::slot_login_tcp_success()
{
    showTip(tr("登录成功"),true);
    emit switch_chat_page();
}



void LoginDialog::slot_login_tcp_failed(int err)
{
    QString result = QString("登录失败, err is %1")
                         .arg(err);
    showTip(result,false);
    ui->login_pushButton->setEnabled(true);
}

bool LoginDialog::checkEmailFormat()
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

bool LoginDialog::checkPasswdValid()
{
    auto password = ui->password_edit->text();
    if(password == ""){
        AddTipErr(TipErr::TIP_PWD_ERR, tr("密码不能为空"));
        return false;
    }
    DelTipErr(TipErr::TIP_PWD_ERR);
    return true;
}

void LoginDialog::timerCallback()
{
    showTip(tr("welcome"), true);
    _countdown_timer->stop();
}

void LoginDialog::AddTipErr(TipErr e, QString tip)
{
    _tip_errs[e] = tip;
    showTip(tip, false);
    _countdown_timer->start(3000);
}

void LoginDialog::DelTipErr(TipErr e)
{
    ui->err_tip->clear();
    _tip_errs.remove(e);
    if(!_tip_errs.empty()){
        showTip(_tip_errs.first(), false);
        _countdown_timer->start(3000);
    }
    return;
}

void LoginDialog::showTip(const QString& s, bool b_ok)
{
    if(b_ok){
        ui->err_tip->setProperty("state", "normal");
    }else{
        ui->err_tip->setProperty("state", "err");

    }
    ui->err_tip->setText(s);
    repolish(ui->err_tip);
}

