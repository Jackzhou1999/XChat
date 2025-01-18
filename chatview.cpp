#include "chatview.h"
#include <QScrollBar>
#include <QTimer>
#include <QStyleOption>
#include <QPainter>
#include <QEvent>
#include <QApplication>
#include "tcpmgr.h"

ChatView::ChatView(QWidget *parent)
    : QWidget{parent}, isAppending(false)
{
    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    this->setLayout(pMainLayout);
    pMainLayout->setContentsMargins(0,0,0,0);

    m_pScrollArea = new CustomScrollArea();
    m_pScrollArea->setObjectName("chat_area");
    pMainLayout->addWidget(m_pScrollArea);

    QWidget *w = new QWidget(this);
    w->setObjectName("chat_bg");
    w->setAutoFillBackground(true);

    QVBoxLayout *PHLayout_1 = new QVBoxLayout();
    PHLayout_1->addWidget(new QWidget(), 100000);
    w->setLayout(PHLayout_1);
    m_pScrollArea->setWidget(w);

    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    connect(pVScrollBar, &QScrollBar::valueChanged, this, &ChatView::onVScrollBarValueChanged);
    QVBoxLayout *PHLayout_2 = new QVBoxLayout();
    PHLayout_2->addWidget(pVScrollBar, 0, Qt::AlignRight);
    PHLayout_2->setContentsMargins(0,0,0,0);
    m_pScrollArea->setLayout(PHLayout_2);
    pVScrollBar->setHidden(true);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->installEventFilter(this);
    initStyleSheet();
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_history_msg_arrived, this, &ChatView::slot_history_msg_arrived);
    connect(m_pScrollArea, &CustomScrollArea::requestLoadMore, this, &ChatView::slot_noBarNeedHistoryMsg);
}

void ChatView::appendChatItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    vl->insertWidget(vl->count()-1, item);
}

void ChatView::prependChatItem(QWidget *item)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    vl->insertWidget(0, item);
}

void ChatView::insertChatItem(QWidget *item, int location)
{
    QVBoxLayout *vl = qobject_cast<QVBoxLayout*>(m_pScrollArea->widget()->layout());
    vl->insertWidget(location, item);
}

void ChatView::removeAllItem()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(m_pScrollArea->widget()->layout());

    int count = layout->count();

    for (int i = 0; i < count - 1; ++i) {
        QLayoutItem *item = layout->takeAt(0); // 始终从第一个控件开始删除
        if (item) {
            if (QWidget *widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    }
}

bool ChatView::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::Enter && watched == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(m_pScrollArea->verticalScrollBar()->maximum() == 0);
    }
    else if(event->type() == QEvent::Leave && watched == m_pScrollArea)
    {
        m_pScrollArea->verticalScrollBar()->setHidden(true);
    }
    return QWidget::eventFilter(watched, event);
}

void ChatView::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void ChatView::onVScrollBarValueChanged(int value) {

    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    if (value == pVScrollBar->minimum()) {
        // 获取当前内容高度和滚动条位置
        int oldContentHeight = m_pScrollArea->widget()->height();
        int oldScrollValue = pVScrollBar->value();
        emit sig_load_history_msg(oldScrollValue, oldContentHeight);
    }
}

void ChatView::slot_noBarNeedHistoryMsg(){
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    int oldContentHeight = m_pScrollArea->widget()->height();
    int oldScrollValue = pVScrollBar->value();
    emit sig_load_history_msg(oldScrollValue, oldContentHeight);
}


void ChatView::initStyleSheet()
{

}

void ChatView::slot_history_msg_arrived(int uid)
{
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    int oldContentHeight = m_pScrollArea->widget()->height();
    int oldScrollValue = pVScrollBar->value();
    emit sig_history_msg_arrived(uid, oldScrollValue, oldContentHeight);

}

void ChatView::slot_load_history_msg_finish(int oldScrollValue, int oldContentHeight)
{
    QApplication::processEvents();
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    QWidget *contentWidget = m_pScrollArea->widget();
    contentWidget->adjustSize();
    int newContentHeight = contentWidget->height();

    // 恢复滚动条位置
    int heightDifference = newContentHeight - oldContentHeight;
    pVScrollBar->setValue(oldScrollValue + heightDifference);
}

void ChatView::slot_set_scrollbar_to_bottom()
{
    QApplication::processEvents();
    QWidget *contentWidget = m_pScrollArea->widget();
    contentWidget->adjustSize();
    QScrollBar *pVScrollBar = m_pScrollArea->verticalScrollBar();
    pVScrollBar->setValue(pVScrollBar->maximum());

}
