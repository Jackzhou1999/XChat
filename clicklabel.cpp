#include "clicklabel.h"
#include <QMouseEvent>

ClickLabel::ClickLabel(QWidget *parent, Qt::WindowFlags f):QLabel(parent, f)
{
    this->setCursor(Qt::PointingHandCursor);
}

void ClickLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton){
        emit clicked();
    }
    QLabel::mousePressEvent(ev);
}

void ClickLabel::enterEvent(QEnterEvent *event)
{
    this->setStyleSheet("color: rgb(93, 173, 226);");
    QLabel::enterEvent(event);
}

void ClickLabel::leaveEvent(QEvent *event)
{
    this->setStyleSheet("color: black;");
    QLabel::leaveEvent(event);
}
