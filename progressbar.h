#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H
#include <QWidget>
#include <QPainter>

class ProgressBar : public QWidget {
    Q_OBJECT

public:
    explicit ProgressBar(QWidget *parent = nullptr);
    // 设置进度 (0 - 100)
    void setProgress(int progress);
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    int m_progress; // 当前进度
};


#endif // PROGRESSBAR_H
