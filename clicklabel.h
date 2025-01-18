#ifndef CLICKLABEL_H
#define CLICKLABEL_H

#include <QLabel>

class ClickLabel : public QLabel
{
    Q_OBJECT
public:
    ClickLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());
    void mousePressEvent(QMouseEvent *ev) override;
    void enterEvent(QEnterEvent *event) override ;
    void leaveEvent(QEvent *event) override;

signals:
    void clicked();

};

#endif // CLICKLABEL_H
