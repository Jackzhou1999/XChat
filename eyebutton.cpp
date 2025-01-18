#include "eyebutton.h"


EyeButton::EyeButton(QWidget *parent):QPushButton(parent), _isVisible(Visible::Yes), _isHover(Hover::No)
{
    initMaps();
    this->setStyleSheet("background-color: white");
    this->setStyleSheet(_map_stylesheet[_isVisible][_isHover]);
}

bool EyeButton::IsVisiable()
{
    return (_isVisible == Visible::Yes)? true:false;
}

void EyeButton::initMaps()
{
    _map_stylesheet[Visible::Yes][Hover::No] = "border: none; border-image: url(:/res/visible.png);";
    _map_stylesheet[Visible::Yes][Hover::Yes] = "border: none; border-image: url(:/res/visible_hover.png);";
    _map_stylesheet[Visible::No][Hover::No] = "border: none; border-image: url(:/res/unvisible.png);";
    _map_stylesheet[Visible::No][Hover::Yes] = "border: none; border-image: url(:/res/unvisible_hover.png);";
}

void EyeButton::enterEvent(QEnterEvent *event)
{
    _isHover = Hover::Yes;
    this->setStyleSheet(_map_stylesheet[_isVisible][_isHover]);
    QPushButton::enterEvent(event);
}

void EyeButton::leaveEvent(QEvent *event)
{
    _isHover = Hover::No;
    this->setStyleSheet(_map_stylesheet[_isVisible][_isHover]);
    QPushButton::leaveEvent(event);
}
void EyeButton::mousePressEvent(QMouseEvent *e)
{
    _isVisible = (_isVisible == Visible::Yes)? Visible::No : Visible::Yes;
    _isHover = Hover::Yes;
    this->setStyleSheet(_map_stylesheet[_isVisible][_isHover]);
    emit clicked();
    QPushButton::mousePressEvent(e);
}

