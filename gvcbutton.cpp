#include "gvcbutton.h"

GvcButton::GvcButton(QWidget *parent) {
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &GvcButton::SetBtnText);
    times = 10;
}

void GvcButton::mouseReleaseEvent(QMouseEvent *e)
{
    _timer->start(1000);
    this->setEnabled(false);
    QPushButton::mouseReleaseEvent(e);
    this->setText(QString::number(times));
    emit clicked();
}

void GvcButton::SetBtnText()
{
    if(times > 0){
        --times;
        this->setText(QString::number(times));
        return;
    };
    times = 10;
    _timer->stop();
    this->setEnabled(true);
    this->setText(tr("获取"));
    return;

}
