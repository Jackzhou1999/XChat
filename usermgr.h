#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"

class UserMgr : public QObject,public Singleton<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    void setname(const QString& name);
    void setEmail(const QString& email);
    void setToken(const QString& token);
    void setUid(int uid);
    void setIcon(const QString &icon);
    void setDownloadDir(const QString &path);


    QString getname();
    QString getIcon();
    QString getUid_str();
    int getUid_int();
    QString getUserHomeDir();
    QString getDownloadDir();

private:
    explicit UserMgr(QObject *parent = nullptr);
    QString _email;
    QString _token;
    QString _name;
    QString _icon;
    int _uid;
    QString _uid_str;
    QString _user_tmp_file_dir;
    QString _user_download_file_dir;
signals:
};

#endif // USERMGR_H
