#ifndef LOADINGDLG_H
#define LOADINGDLG_H
#include <QDialog>
#include <QLabel>
#include <QMovie>
#include <QVBoxLayout>

namespace Ui {
class LoadingDialog;
}

class LoadingDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoadingDialog(QWidget *parent = nullptr) : QDialog(parent) {
        // 设置无边框和透明背景
        setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);
        setFixedSize(200, 200); // 设置加载窗口大小

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
        layout->setAlignment(Qt::AlignCenter); // 居中显示
        setLayout(layout);
    }

    // 在目标窗口中心显示
    void showCentered(QWidget *target) {
        if (target) {
            // 计算加载界面的位置
            QPoint center = target->geometry().center() - rect().center();
            move(target->mapToGlobal(center));
        }
        show();
    }
};

#endif // LOADINGDLG_H
