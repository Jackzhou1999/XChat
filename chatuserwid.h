 #ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QWidget>
#include "listitembase.h"
#include <QLabel>
#include "userdata.h"
#include <QSet>

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    QSize sizeHint() const override{
        return QSize(0, 70);
    }

    void SetInfo(const DbUserInfo &info);
    void Refresh();
    void UpdateInfo(const ChatMessage& lastmsg);

    void SetUnreadCount(int count);
    int GetUid();
    bool hasMsg();
public:
    QSet<int> unread_msg_ids;
private:
    void updateRedDot();
private:
    Ui::ChatUserWid *ui;
    QLabel* _redDotLabel;

    int _unreadCount;
    int _uid;
    QString _name;
    QString _head;
    int _last_msg_type;
    QString _last_msg;
    QDateTime _last_msg_time;

    bool _set_msg;
};

#endif // CHATUSERWID_H
