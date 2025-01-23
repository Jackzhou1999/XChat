#include "statewidget.h"
#include <QVBoxLayout>
#include <QPainter>
#include <QStyleOption>
#include <QMouseEvent>


StateWidget::StateWidget(QWidget *parent):QWidget(parent),_curstate(ClickLbState::Normal)
{
    setCursor(Qt::PointingHandCursor);
    AddRedPoint();
    setContentsMargins(0,0,0,0);
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
    _red_point = new QLabel(this);
    _red_point->setObjectName("red_point");
    _red_point->setFixedSize(8, 8);

    _red_point->setStyleSheet(
        "background-color: red; "
        "color: white; "
        "border-radius: 4px; "
        "font-size: 11px;"
        "font-weight: bold;"
        "text-align: center;"
        );

    _red_point->setVisible(false);

}

void StateWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    updateLabelPosition(); // 窗口大小变化时，更新 QLabel 位置
}


void StateWidget::updateLabelPosition() {
    int x = width() - _red_point->width();
    int y = 0;
    _red_point->move(x, y);
}

void StateWidget::ShowRedPoint()
{
    _red_point->setVisible(true);
}

void StateWidget::HideRedPoint()
{
    _red_point->setVisible(false);
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
        HideRedPoint();
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
