#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QAction>
#include "global.h"
#include "loadingdlg.h"
#include "statewidget.h"
#include "customitemdelegate.h"
#include "chatuserwid.h"
#include <QListWidgetItem>

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void ShowSearch(bool bsearch = false);
    void addChatUserList();
    void ClearLabelState(StateWidget*);
public slots:
    //当有新消息到达时应发送信号触发该曹函数
    void slot_add_chat_user_item(int uid);
    void slot_update_chat_user_item(int uid);
private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_search_list(const QString& text);
    void slot_search_list_click();
    void hideLoadingDialog();

    void slot_item_clicked(QListWidgetItem *item);
    void slot_tonewfriendpage(int uid);
    void slot_tofriendinfopage(int uid);
    void slot_tomyapplypage(int uid);
    void slot_to_chat_page(int uid);
    void slot_to_downloadfile_dir();
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void handleGlobalMousePress(QMouseEvent *event);
signals:
    void sig_loading_chat_user_finished();
private:
    Ui::ChatDialog *ui;
    ChatUiMode _mode;
    ChatUiMode _state;
    ChatUiMode _prev_state;

    QAction * clearAction;
    bool _b_loading;
    LoadingDialog* _loadingScreen;
    QVector<StateWidget*> _side_lb_list;

    int _cur_chat_uid;
    QMap<int, ChatUserWid*> ChatUserWidMgr;
    QMap<QListWidgetItem* ,ChatUserWid*> save;
    QMap<ChatUserWid*, QListWidgetItem*> save2;

};

#endif // CHATDIALOG_H
