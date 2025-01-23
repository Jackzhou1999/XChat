#ifndef SEARCHUSERFRIENDITEM_H
#define SEARCHUSERFRIENDITEM_H

#include "userdata.h"
#include <QWidget>

namespace Ui {
class SearchUserFriendItem;
}

class SearchUserFriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit SearchUserFriendItem(QWidget *parent = nullptr);
    ~SearchUserFriendItem();
    void setInfo(DbUserInfo info);
    QSize sizeHint() const override{
        return QSize(450, 60);
    }
private:
    Ui::SearchUserFriendItem *ui;
    int _uid;

signals:
    void sig_to_chat_page(int uid);
private slots:
    void slot_to_chat_page();
};

#endif // SEARCHUSERFRIENDITEM_H
