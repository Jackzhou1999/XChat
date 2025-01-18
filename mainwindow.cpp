#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logindialog.h"
#include "registedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("XChat"));

    _login_dialog = new LoginDialog(this);
    setCentralWidget(_login_dialog);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_login_dialog, &LoginDialog::switch_registion_page, this, &MainWindow::openregistedialogpage);
    connect(_login_dialog, &LoginDialog::switch_forgetpwd_page, this, &MainWindow::openforgetpwdpage);
    connect(_login_dialog, &LoginDialog::switch_chat_page, this, &MainWindow::openchatdialogpage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openregistedialogpage()
{
    _registe_dialog = new RegisteDialog(this);
    _registe_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_registe_dialog, &RegisteDialog::sig_backtologinpage, this, &MainWindow::openlogindialogpage);

    setCentralWidget(_registe_dialog);
    _login_dialog->hide();
    _registe_dialog->show();
}

void MainWindow::openlogindialogpage()
{
    _login_dialog = new LoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_login_dialog, &LoginDialog::switch_registion_page, this, &MainWindow::openregistedialogpage);
    connect(_login_dialog, &LoginDialog::switch_forgetpwd_page, this, &MainWindow::openforgetpwdpage);
    connect(_login_dialog, &LoginDialog::switch_chat_page, this, &MainWindow::openchatdialogpage);

    setCentralWidget(_login_dialog);
    _registe_dialog->hide();
    _login_dialog->show();
}

void MainWindow::openforgetpwdpage()
{
    _resetpwd_dialog = new ForgetPwdDialog(this);
    _resetpwd_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_resetpwd_dialog, &ForgetPwdDialog::sig_backtologinpage, this, &MainWindow::openlogindialogpage2);

    setCentralWidget(_resetpwd_dialog);
    _login_dialog->hide();
    _resetpwd_dialog->show();
}

void MainWindow::openlogindialogpage2()
{
    _login_dialog = new LoginDialog(this);
    _login_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    connect(_login_dialog, &LoginDialog::switch_registion_page, this, &MainWindow::openregistedialogpage);
    connect(_login_dialog, &LoginDialog::switch_forgetpwd_page, this, &MainWindow::openforgetpwdpage);
    connect(_login_dialog, &LoginDialog::switch_chat_page, this, &MainWindow::openchatdialogpage);

    setCentralWidget(_login_dialog);
    _resetpwd_dialog->hide();
    _login_dialog->show();
}

void MainWindow::openchatdialogpage()
{
    _chat_dialog = new ChatDialog(this);
    _chat_dialog->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setCentralWidget(_chat_dialog);
    _login_dialog->hide();
    _chat_dialog->show();
    this->setMinimumSize(QSize(1050,900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
