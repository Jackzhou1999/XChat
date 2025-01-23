#ifndef SEARCHUSERLIST_H
#define SEARCHUSERLIST_H

#include <QListWidget>

class SearchUserList : public QListWidget
{
    Q_OBJECT
public:
    SearchUserList(QWidget* parent = nullptr);
protected:
    bool eventFilter(QObject *object, QEvent *event);
signals:
    void sig_loading_more_user();
};

#endif // SEARCHUSERLIST_H
