#ifndef CONTACTTREEWIDGET_H
#define CONTACTTREEWIDGET_H


#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QMessageBox>
#include <QFocusEvent>

class DeleteWidget : public QWidget {
    Q_OBJECT
public:
    explicit DeleteWidget(QWidget *parent = nullptr);
    QPushButton *deleteButton; // 删除按钮

protected:
    void focusOutEvent(QFocusEvent *event) override; // 监听失去焦点事件
    void mousePressEvent(QMouseEvent *event);

signals:
    void requestClose(); // 请求关闭信号
};

struct NewFriendReqInfo
{
    NewFriendReqInfo(int uid, QString name, QString iconurl):
        _uid(uid), _name(name), _iconurl(iconurl){}
    int _uid;
    QString _name;
    QString _iconurl;
};

struct MyApplyRspInfo
{
    MyApplyRspInfo(int uid, QString name, QString iconurl, int state):
        _uid(uid), _name(name), _iconurl(iconurl), _state(state){}
    int _uid;
    QString _name;
    QString _iconurl;
    int _state;
};

struct ContactInfo
{
    ContactInfo(int uid, QString name, QString iconurl):
        _uid(uid), _name(name), _iconurl(iconurl){}
    int _uid;
    QString _name;
    QString _iconurl;
};

// 自定义 QTreeWidget 类
class ContactTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    explicit ContactTreeWidget(QWidget *parent = nullptr);

    // 添加子列表项方法
    void addNewFriendSubItem(const NewFriendReqInfo &info);
    void addContactsSubItem(const ContactInfo &info);
    void addMyApplysSubItem(const MyApplyRspInfo &info);
protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMap<int, QTreeWidgetItem*> NewFriendsubItemMap;
    QMap<int, QTreeWidgetItem*> ContactssubItemMap;
    QMap<int, QTreeWidgetItem*> MyApplyssubItemMap;

    void deleteSubItem();
    void showDeleteOption(const QPoint &pos);

    QTreeWidgetItem *_treeNewFriends;
    QTreeWidgetItem *_treeContacts;
    QTreeWidgetItem *_treeMyApplys;

    QTreeWidgetItem* _currentItem;
    DeleteWidget *_deleteWidget;                // 删除界面
signals:
    void to_newfriendpage(int uid);
    void to_myapplypage(int uid);
    void to_friendinfopage(int uid);
};

#endif // CONTACTTREEWIDGET_H
