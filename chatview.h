#ifndef CHATVIEW_H
#define CHATVIEW_H
#include <QVBoxLayout>
#include <QScrollArea>
#include "customscrollarea.h"

#include <QWidget>

class ChatView : public QWidget
{
    Q_OBJECT
public:
    explicit ChatView(QWidget *parent = nullptr);
    void appendChatItem(QWidget* item);
    void prependChatItem(QWidget* item);
    void insertChatItem(QWidget* item, int location);
    void removeAllItem();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    void paintEvent(QPaintEvent *event);
private:
    void onVScrollBarValueChanged(int value);
    void initStyleSheet();

private:
    QVBoxLayout* m_pVl;
    CustomScrollArea *m_pScrollArea;
    bool isAppending;
signals:
    void sig_load_history_msg(int, int);
    // 从服务器下载聊天历史记录完成
    void sig_history_msg_arrived(int uid, int oldScrollValue, int oldContentHeight);
public slots:

    void slot_history_msg_arrived(int uid);
    void slot_load_history_msg_finish(int oldScrollValue, int oldContentHeight);
    void slot_set_scrollbar_to_bottom();
    void slot_noBarNeedHistoryMsg();

};

#endif // CHATVIEW_H
