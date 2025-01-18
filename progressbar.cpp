#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent)
    : QWidget(parent), m_progress(0) {
    setFixedHeight(5);
    setFixedWidth(250);
    setContentsMargins(0,0,0,0);
}

void ProgressBar::setProgress(int progress) {
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    m_progress = progress;
    update(); // 刷新绘制
}

void ProgressBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制背景（灰色）
    QRectF rect(0, 0, width(), height());
    painter.setBrush(QColor(200, 200, 200)); // 灰色背景
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 5, 5);

    // 绘制进度（蓝色）
    QRectF progressRect(0, 0, width() * m_progress / 100.0, height());
    painter.setBrush(QColor(0, 150, 255)); // 蓝色填充
    painter.drawRoundedRect(progressRect, 5, 5);
}
