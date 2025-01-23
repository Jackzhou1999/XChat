#ifndef SEARCHUSERNOFRIENDITEM_H
#define SEARCHUSERNOFRIENDITEM_H

#include <QWidget>
#include "userdata.h"

namespace Ui {
class SearchUserNoFriendItem;
}

class SearchUserNoFriendItem : public QWidget
{
    Q_OBJECT

public:
    explicit SearchUserNoFriendItem(QWidget *parent = nullptr);
    ~SearchUserNoFriendItem();
    void setInfo(DbUserInfo info);
    QSize sizeHint() const override{
        return QSize(450, 60);
    }
private:
    Ui::SearchUserNoFriendItem *ui;
    int _uid;

signals:
    void sig_to_friendapply_page(int uid);
private slots:
    void slot_to_friendapply_page();
};

#endif // SEARCHUSERNOFRIENDITEM_H
