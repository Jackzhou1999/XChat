#ifndef RESIZABLEWIDGET_H
#define RESIZABLEWIDGET_H

#include <QApplication>
#include <QWidget>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QLabel>

class ResizableWidget : public QWidget {
    Q_OBJECT

public:
    explicit ResizableWidget(QWidget *parent = nullptr)
        : QWidget(parent), resizing(false) {
        setMouseTracking(true);  // 启用鼠标跟踪
        setMinimumWidth(200);   // 设置最小宽度
        setMaximumWidth(350);
    }

protected:
    void mousePressEvent(QMouseEvent *event) override {
        // 判断鼠标是否在右边缘
        if (event->button() == Qt::LeftButton && isInResizeZone(event->pos())) {
            resizing = true;  // 开始拉伸
            lastMousePos = event->globalPos();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (resizing) {
            // 计算鼠标移动的距离
            int dx = event->globalPos().x() - lastMousePos.x();
            int newWidth = width() + dx;

            // 设置新的宽度
            if (newWidth >= minimumWidth()) {
                resize(newWidth, height());
                lastMousePos = event->globalPos();  // 更新鼠标位置
            }
        } else {
            // 改变鼠标光标形状
            if (isInResizeZone(event->pos())) {
                setCursor(Qt::SizeHorCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        Q_UNUSED(event);
        resizing = false;  // 停止拉伸
    }

private:
    bool isInResizeZone(const QPoint &pos) const {
        const int resizeMargin = 5;  // 边缘检测范围
        return pos.x() >= width() - resizeMargin;
    }

    bool resizing;
    QPoint lastMousePos;
};

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     // 主窗口
//     QWidget mainWindow;
//     mainWindow.setWindowTitle("Resizable Widget Example");

//     // 可拉伸的子窗口
//     ResizableWidget *resizableWidget = new ResizableWidget();
//     resizableWidget->setStyleSheet("background-color: lightblue;");

//     // 布局
//     QHBoxLayout *layout = new QHBoxLayout(&mainWindow);
//     layout->addWidget(new QLabel("Left Widget"));
//     layout->addWidget(resizableWidget);
//     layout->addWidget(new QLabel("Right Widget"));
//     layout->setStretch(1, 1);  // 设置拉伸因子，允许 resizableWidget 占据更多空间

//     mainWindow.setLayout(layout);
//     mainWindow.resize(800, 400);
//     mainWindow.show();

//     return app.exec();
// }

#endif // RESIZABLEWIDGET_H
