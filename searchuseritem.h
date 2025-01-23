#ifndef SEARCHUSERITEM_H
#define SEARCHUSERITEM_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class SearchUserItem;
}

class SearchUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit SearchUserItem(QWidget *parent = nullptr);
    ~SearchUserItem();
    QSize sizeHint() const override{
        return QSize(250, 60);
    }
    void setInfo(DbUserInfo info);
private:
    Ui::SearchUserItem *ui;
    int _uid;
signals:
    void sig_tofriendinfopage(int uid);
private slots:
    void slot_tofriendinfopage();
};

#endif // SEARCHUSERITEM_H
