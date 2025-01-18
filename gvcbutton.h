#ifndef GVCBUTTON_H
#define GVCBUTTON_H

#include <QPushButton>
#include <QTimer>

class GvcButton : public QPushButton
{
    Q_OBJECT
public:
    GvcButton(QWidget *parent = nullptr);
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    void SetBtnText();

    QTimer* _timer;
    size_t times;
};

#endif // GVCBUTTON_H
