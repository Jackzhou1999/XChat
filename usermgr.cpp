#include "usermgr.h"
#include "sqlmgr.h"
#include "fileclient.h"

UserMgr::~UserMgr()
{
    if(!_user_tmp_file_dir.isEmpty()){
        QDir dir(_user_tmp_file_dir);
        if (dir.exists()) {
            if (dir.removeRecursively()) {
                qDebug() << "Successfully deleted directory:" << _user_tmp_file_dir;
            } else {
                qDebug() << "Failed to delete directory:" << _user_tmp_file_dir;
            }
        } else {
            qDebug() << "Directory does not exist:" << _user_tmp_file_dir;
        }
    }
}

void UserMgr::setname(const QString &name)
{
    _name = name;
}

void UserMgr::setEmail(const QString &email)
{
    _email = email;
}

void UserMgr::setToken(const QString &token)
{
    _token = token;
}

void UserMgr::setIcon(const QString &icon)
{
    _icon = icon;
}

void UserMgr::setDownloadDir(const QString &path)
{
    _user_download_file_dir = path;
}

QString UserMgr::getUserHomeDir(){
    return _user_tmp_file_dir;
}

QString UserMgr::getDownloadDir()
{
    return _user_download_file_dir;
}

void UserMgr::setUid(int uid)
{
    _uid = uid;
    _uid_str = QString::number(_uid);

    QString app_path = QCoreApplication::applicationDirPath();
    QString userDir = QDir::toNativeSeparators(app_path + QDir::separator() + "users");
    _user_tmp_file_dir = QDir::toNativeSeparators(userDir + QDir::separator() + QString::number(uid));

    qDebug() << "用户家目录：" << _user_tmp_file_dir;
    QDir dir;
    if (dir.mkpath(_user_tmp_file_dir)) {
        qDebug() << "Directory created successfully:" << _user_tmp_file_dir;
    } else {
        qDebug() << "Failed to create directory:" << _user_tmp_file_dir;
    }

    QString sqlPath = QDir::toNativeSeparators(_user_tmp_file_dir + QDir::separator() + "app_database.sqlite");
    qDebug() << " 数据库路径：" << sqlPath;
    SqliteConnectionPool::getInstance().initialize(sqlPath);
}

QString UserMgr::getname()
{
    return _name;
}

QString UserMgr::getIcon()
{
    return _icon;
}

int UserMgr::getUid_int()
{
    return _uid;
}

QString UserMgr::getUid_str()
{
    return _uid_str;
}

UserMgr::UserMgr(QObject *parent)
    : QObject{parent}
{

}
