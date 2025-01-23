#include "gvcbutton.h"

GvcButton::GvcButton(QWidget *parent) {
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &GvcButton::SetBtnText);
    times = 10;
}

void GvcButton::SetState(QString normal, QString hover, QString press)
{
    _hover = hover;
    _normal = normal;
    _press = press;
    setProperty("state", normal);
    repolish(this);
    update();
}

void GvcButton::enterEvent(QEnterEvent *event)
{
    setProperty("state", _hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void GvcButton::leaveEvent(QEvent *event)
{
    setProperty("state", _normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void GvcButton::mousePressEvent(QMouseEvent *e)
{
    setProperty("state", _press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(e);
}

void GvcButton::mouseReleaseEvent(QMouseEvent *e)
{
    setProperty("state", _press);
    repolish(this);
    update();

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
