#ifndef GVCBUTTON_H
#define GVCBUTTON_H

#include <QPushButton>
#include <QTimer>
#include "global.h"

class GvcButton : public QPushButton
{
    Q_OBJECT
public:
    GvcButton(QWidget *parent = nullptr);
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
    void SetBtnText();

    QTimer* _timer;
    size_t times;
};

#endif // GVCBUTTON_H
