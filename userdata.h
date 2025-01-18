#ifndef USERDATA_H
#define USERDATA_H
#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>

struct packet
{
    packet(QString from_uid, QString to_uid, QString content, int content_type)
        :_from_uid(from_uid), _to_uid(to_uid), _content(content),_content_type(content_type) {}
    packet() = default;
    QString _from_uid;
    QString _to_uid;
    QString _content;
    int _content_type;
};

class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
    QString _icon;
};

struct AuthInfo {
    AuthInfo(int uid, QString name,
             QString nick, QString icon, int sex):
        _uid(uid), _name(name), _nick(nick), _icon(icon),
        _sex(sex){}
    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name,
            QString peer_nick, QString peer_icon, int peer_sex)
        :_uid(peer_uid),_name(peer_name),_nick(peer_nick),
        _icon(peer_icon),_sex(peer_sex)
    {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

// 好友关系信息结构体
struct FriendRelationship {
    int id;
    qint64 user_id;
    qint64 friend_id;
    int status;
    QDateTime created_at;
    QDateTime updated_at;
};

// 聊天消息信息结构体
struct ChatMessage {
    int id;
    qint64 sender_id;
    qint64 receiver_id;
    bool is_group;
    QString content;
    int content_type;
    int status;
    QDateTime created_at;
    QDateTime updated_at;
};

// 用户信息结构体
struct DbUserInfo {
    int id;
    qint64 uid;
    QString name;
    QString email;
    QString nick;
    QString desc;
    int sex;
    QString icon;
};


struct FriendRequest {
    qint64 id;                 // 申请记录的唯一标识 ID
    qint64 sender_id;          // 发送者用户 ID
    qint64 receiver_id;        // 接收者用户 ID
    QString message;           // 申请附带消息
    int status;                // 申请状态 (0: 待处理, 1: 已接受, 2: 已拒绝)
    QString created_at;        // 创建时间 (申请时间)
    QString updated_at;        // 更新时间 (处理时间)
    QString rejection_reason;  // 拒绝理由 (如果状态为拒绝)
    bool is_sender_read;       // 发送者是否已读 (标志)
};

#endif // USERDATA_H

