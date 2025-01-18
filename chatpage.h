#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "imageviewer.h"
#include "userdata.h"
#include "chatuserwid.h"
#include "loadingdlg.h"

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void paintEvent(QPaintEvent *event) override;
    void SetUserInfo(int uid);
    void AppendChatMsg(const ChatMessage &msg, int location);
    void AppendChatMsg(const ChatMessage &msg);

    ChatUserWid* _chatuserwid;
    int _peeruid;

private slots:
    void on_send_btn_clicked();
    void slot_show_imageviewer(const QPixmap& image);
    void slot_load_history_msg(int oldScrollValue, int oldContentHeight);
    void slot_history_msg_from_server_arrived(int uid, int oldScrollValue, int oldContentHeight);
    void hideLoadingDialog();
    void slot_server_data_run_out();
signals:
    void sig_load_history_msg_finish(int oldScrollValue, int oldContentHeight);
    void sig_set_scrollbar_to_bottom();
private:    
    DbUserInfo _peerinfo;
    QString _timestamp;
    bool _server_data_run_out;

    LoadingDialog* _loadingScreen;
    Ui::ChatPage *ui;
    ImageViewer* _imageviewer;
};

#endif // CHATPAGE_H
