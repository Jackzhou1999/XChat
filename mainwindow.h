#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registedialog.h"
#include "forgetpwddialog.h"
#include "chatdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginDialog* _login_dialog;
    RegisteDialog* _registe_dialog;
    ForgetPwdDialog* _resetpwd_dialog;
    ChatDialog* _chat_dialog;
private slots:
    void openregistedialogpage();
    void openlogindialogpage();
    void openforgetpwdpage();
    void openlogindialogpage2();
    void openchatdialogpage();

};
#endif // MAINWINDOW_H
