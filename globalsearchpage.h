#ifndef GLOBALSEARCHPAGE_H
#define GLOBALSEARCHPAGE_H
#include <QDialog>
#include <QWidget>
#include "userdata.h"
#include "applyfriend.h"

namespace Ui {
class GlobalSearchPage;
}

class GlobalSearchPage : public QDialog
{
    Q_OBJECT

public:
    explicit GlobalSearchPage(QWidget *parent = nullptr);
    ~GlobalSearchPage();
    void addUserItem(const DbUserInfo& info);
    void initEdit(const QString& text);
private:
    Ui::GlobalSearchPage *ui;
    std::shared_ptr<ApplyFriend> _applyfriendpage;
signals:
    void sig_to_chat_page(int uid);
    void sig_to_friendapply_page(int uid);
private slots:
    void slot_to_friendapply_page(int uid);
    void slot_send_search_request(QString text);
public slots:
    void slot_recv_search_result(QVector<DbUserInfo> users);
};

#endif // GLOBALSEARCHPAGE_H
