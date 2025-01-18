#ifndef NEWFRIENDINFOPAGE_H
#define NEWFRIENDINFOPAGE_H

#include <QWidget>

namespace Ui {
class NewFriendInfoPage;
}

class NewFriendInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit NewFriendInfoPage(QWidget *parent = nullptr);
    ~NewFriendInfoPage();
    void setNewFriendInfo(int uid);
private:
    Ui::NewFriendInfoPage *ui;
};

#endif // NEWFRIENDINFOPAGE_H
