#ifndef CUSTOMSCROLLAREA_H
#define CUSTOMSCROLLAREA_H
#include <QScrollArea>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>

class CustomScrollArea : public QScrollArea {
    Q_OBJECT

public:
    explicit CustomScrollArea(QWidget *parent = nullptr) : QScrollArea(parent) {
        // 默认隐藏滚动条
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

protected:
    void wheelEvent(QWheelEvent *event) override {
        // 检查垂直滚动条是否可见
        if (!verticalScrollBar()->isVisible()) {
            // 滚动条不可见时处理滚轮事件
            if (event->angleDelta().y() > 0) {
                // 检测到上滑滚轮
                qDebug() << "Mouse wheel scrolled up (scrollbar is hidden)!";
                emit requestLoadMore(); // 自定义信号
            }
        } else {
            // 滚动条可见时保留默认行为
            QScrollArea::wheelEvent(event);
        }
    }

signals:
    void requestLoadMore(); // 用于通知上层逻辑加载更多数据
};

#endif // CUSTOMSCROLLAREA_H
