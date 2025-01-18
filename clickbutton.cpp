#include "clickbutton.h"

ClickButton::ClickButton(QWidget *parent):QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickButton::SetState(QString normal, QString hover, QString press)
{
    _hover = hover;
    _normal = normal;
    _press = press;
    setProperty("state", normal);
    repolish(this);
    update();
}

void ClickButton::enterEvent(QEnterEvent *event)
{
    setProperty("state", _hover);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickButton::leaveEvent(QEvent *event)
{
    setProperty("state", _normal);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickButton::mousePressEvent(QMouseEvent *e)
{
    setProperty("state", _press);
    repolish(this);
    update();
    QPushButton::mousePressEvent(e);
}

void ClickButton::mouseReleaseEvent(QMouseEvent *e)
{
    setProperty("state", _hover);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(e);
}
