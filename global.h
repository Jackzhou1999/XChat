#ifndef GLOBAL_H
#define GLOBAL_H
#include <QWidget>
#include <functional>
#include "QStyle"
#include <QRegularExpression>

#include <mutex>
#include <iostream>
#include <memory.h>
#include <functional>
#include <QByteArray>
#include <QNetworkReply>
#include <QJsonObject>
#include <QDir>
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QSettings>

extern std::function<void(QWidget*)> repolish;
#define MAX_FILE_LEN 2048

enum ReqId:uint16_t{
    ID_GET_VARIFY_CODE = 1001,  //获取验证码
    ID_REG_USER = 1002, //注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_HTTP_LOGIN = 1004, // 登录GateServer
    ID_TCP_LOGIN = 1005, //登录TcpServer

    ID_SEEK_FILE_LOCATION = 1006,
    ID_UPLOAD_FILE_REQ = 1007,
    ID_UPLOAD_FILE_RSP = 1008,

    ID_DOWNLOAD_FILE_REQ = 1009,
    ID_DOWNLOAD_FILE_RSP = 1010,
    ID_DOWNLOAD_FILE_RSP_REPLY = 1011,

    ID_NOTIFY_ADD_FRIEND_REQ = 1012,
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1013,
    ID_NOTIFY_AUTH_FRIEND_REQ = 1014,

    ID_GET_MSG_REQ = 1015,
    ID_MSG_READ_RSP = 1016,
    ID_HISTORY_MSG_REQ = 1017,
    ID_HISTORY_MSG_RSP = 1018,
    ID_MSG_BACK = 1019,

    ID_CLOSE_FILESESSION = 1020,
    ID_USER_SEARCH = 1021,

    ID_NOTIFY_ADD_FRIEND_RSP = 1022,


    ID_HEARTBEAT = 1999,
};

enum Modules{
    REGISTERMOD = 0,
    LOGINMOD = 1,
    RESETMOD = 2
};

enum ErrorCode{
    SUCCEE = 0,
    ERR_JSON = 1,   // json解析失败
    ERR_NETWORK = 2,    //网络错误

    ERROR_JSON = 1001,
    RPCFAILED = 1002,
    ERROR_VERIFYCODEEXPIRE = 1003,
    ERROR_VERIFYCODEUNMATCH = 1004,
    ERROR_USEREXIST = 1005,
    ERROR_PASSWDUNMATCH = 1006,
    ERROR_EMAILHAVEBEENUSED = 1007,
    ERROR_EMAILNOREGISTE = 1008,

};

enum TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VARIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum ChatUiMode{
    SearchMode = 0,
    ChatMode,
    ContactMode
};

enum ClickLbState{
    Normal = 0,
    Selected = 1
};

//自定义QListWidgetItem的几种类型
enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
};

struct UserInfo{
    int uid;
    QString host;
    QString port;
    QString token;
};

enum ChatRole{
    Self, Other,
};

struct MsgInfo{
    QString msgFlag;//"text,image,file"
    QString content;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

const int MIN_APPLY_LABEL_ED_LEN = 40;
const QString add_prefix = "添加标签 ";
const int  tip_offset = 5;

extern QString gate_url_prefix;
#endif // GLOBAL_H
