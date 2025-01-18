#include "statewidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>


StateWidget::StateWidget(QWidget *parent):QWidget(parent),_curstate(ClickLbState::Normal)
{
    setCursor(Qt::PointingHandCursor);
    AddRedPoint();
}

void StateWidget::SetState(QString normal, QString hover, QString select)
{
    _normal = normal;
    _selected = select;
    _hover = hover;

    setProperty("state", normal);
    repolish(this);
}


ClickLbState StateWidget::GetCurState()
{
    return _curstate;
}

void StateWidget::ClearState()
{
    _curstate = ClickLbState::Normal;
    setProperty("state", _normal);
    repolish(this);
    update();
}

void StateWidget::SetSelected(bool bselected)
{
    if(bselected){
        _curstate = ClickLbState::Selected;
        setProperty("state", _selected);
        repolish(this);
        update();
        return;
    }
    _curstate = ClickLbState::Normal;
    setProperty("state", _normal);
    repolish(this);
    update();
    return;
}

void StateWidget::AddRedPoint()
{
    _red_point = new QLabel();
    _red_point->setObjectName("red_point");
    QVBoxLayout* layout2 = new QVBoxLayout;
    _red_point->setAlignment(Qt::AlignCenter);
    layout2->addWidget(_red_point);
    layout2->setContentsMargins(0,0,0,0);
    this->setLayout(layout2);
    _red_point->setVisible(false);
}

void StateWidget::ShowRedPoint(bool show)
{
    _red_point->setVisible(true);
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    return;
}

void StateWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton){
        if(_curstate != ClickLbState::Selected){
            _curstate = ClickLbState::Selected;
            setProperty("state",_selected);
        }
        repolish(this);
        update();
        emit clicked();
        QWidget::mousePressEvent(event);
        return;
    }
}


void StateWidget::enterEvent(QEnterEvent* event) {
    if(_curstate == ClickLbState::Normal){
        setProperty("state",_hover);
        repolish(this);
        update();
    }
    QWidget::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent* event){
    if(_curstate == ClickLbState::Normal){
        setProperty("state",_normal);
        repolish(this);
        update();
    }
    QWidget::leaveEvent(event);
}
