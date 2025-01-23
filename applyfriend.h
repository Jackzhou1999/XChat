#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include <QDialog>
#include "clicklabelplus.h"
#include "friendlabel.h"
#include "userdata.h"
#include "global.h"

namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
    bool eventFilter(QObject *obj, QEvent *event);
    void SetInfo(const DbUserInfo &info);
private:
    Ui::ApplyFriend *ui;
    int64_t _uid;
public slots:
    void SlotApplyCancel();
    void SlotApplySure();
};

#endif // APPLYFRIEND_H
