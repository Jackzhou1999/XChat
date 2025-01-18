#ifndef CLICKBUTTON_H
#define CLICKBUTTON_H

#include <QPushButton>
#include "global.h"

class ClickButton : public QPushButton
{
    Q_OBJECT
public:
    ClickButton(QWidget * parent=nullptr);
    ~ClickButton() = default;
    void SetState(QString normal, QString hover, QString press);
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
private:
    QString _normal;
    QString _hover;
    QString _press;

};

#endif // CLICKBUTTON_H
