#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimer>

// 加载界面类
class LoadingScreen : public QWidget {
    Q_OBJECT

public:
    explicit LoadingScreen(QWidget *parent = nullptr) : QWidget(parent) {
        // 设置窗口样式和大小
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(200, 200);

        // 创建 QLabel 并设置 GIF 动画
        QLabel *loadingLabel = new QLabel(this);
        QMovie *movie = new QMovie(":res/loading.gif"); // 确保 GIF 文件在资源中
        loadingLabel->setMovie(movie);
        loadingLabel->setAlignment(Qt::AlignCenter);

        // 启动 GIF 动画
        movie->start();

        // 设置布局
        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->addWidget(loadingLabel);
        setLayout(layout);
    }
};
#endif // LOADINGSCREEN_H
