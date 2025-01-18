#ifndef CONTACTINFOPAGE_H
#define CONTACTINFOPAGE_H

#include <QWidget>

namespace Ui {
class contactinfopage;
}

class contactinfopage : public QWidget
{
    Q_OBJECT

public:
    explicit contactinfopage(QWidget *parent = nullptr);
    ~contactinfopage();
    void setContactInfo(int uid);

private:
    Ui::contactinfopage *ui;
    int _uid;
signals:
    void sig_to_chat_page(int uid);
private slots:
    void slot_to_chat_page();
};

#endif // CONTACTINFOPAGE_H
