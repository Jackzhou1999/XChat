#ifndef EYEBUTTON_H
#define EYEBUTTON_H

#include <QPushButton>
#include <QMap>

class EyeButton : public QPushButton
{
    Q_OBJECT
private:
    enum class Visible{
        Yes,
        No,
    };

    enum class Hover{
        Yes,
        No,
    };

public:
    EyeButton(QWidget *parent = nullptr);
    bool IsVisiable();
private:
    void initMaps();
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *e) override;

    Visible _isVisible;
    Hover _isHover;
    QMap<Visible, QMap<Hover, QString>> _map_stylesheet;
};

#endif // EYEBUTTON_H
