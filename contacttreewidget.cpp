#include "contacttreewidget.h"
#include "customitemdelegate.h"
#include <QMouseEvent>
#include <QPushButton>
#include "userdata.h"
#include <QVBoxLayout>
#include "sqlmgr.h"
#include "usermgr.h"
#include "cachemanager.h"


DeleteWidget::DeleteWidget(QWidget *parent) : QWidget(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup); // 无边框弹出窗口
    setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(this);
    deleteButton = new QPushButton("删除");
    deleteButton->setFixedHeight(30);
    deleteButton->setStyleSheet("QPushButton { background-color: white; border: 1px solid gray; border-radius: 2px; }"
                                "QPushButton:hover { background-color: lightgray; }");
    layout->addWidget(deleteButton);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void DeleteWidget::mousePressEvent(QMouseEvent *event) {
    if (!rect().contains(event->pos())) {
        emit requestClose(); // 鼠标点击外部位置，关闭弹出界面
    }
    QWidget::mousePressEvent(event);
}

void DeleteWidget::focusOutEvent(QFocusEvent *event) {
    Q_UNUSED(event);
    emit requestClose(); // 发出关闭信号
}


ContactTreeWidget::ContactTreeWidget(QWidget *parent) : QTreeWidget(parent), _deleteWidget(nullptr){
    this->setHeaderHidden(true); // 隐藏表头

    setColumnCount(1);
    setHeaderHidden(true);
    setIndentation(0);
    // 设置委托
    CustomItemDelegate *delegate = new CustomItemDelegate(this);
    setItemDelegate(delegate);

    // 添加主列表项
    _treeNewFriends = new QTreeWidgetItem(this);
    _treeNewFriends->setData(0, Qt::DisplayRole, "新的朋友");
    _treeNewFriends->setData(0, Qt::UserRole, true);  // 标记为主列表项
    _treeNewFriends->setData(0, Qt::UserRole+3, false);

    _treeMyApplys = new QTreeWidgetItem(this);
    _treeMyApplys->setData(0, Qt::DisplayRole, "我的申请");
    _treeMyApplys->setData(0, Qt::UserRole, true);  // 标记为主列表项
    _treeMyApplys->setData(0, Qt::UserRole+3, false);

    _treeContacts = new QTreeWidgetItem(this);
    _treeContacts->setData(0, Qt::DisplayRole, "联系人");
    _treeContacts->setData(0, Qt::UserRole, true);  // 标记为主列表项
    _treeContacts->setData(0, Qt::UserRole+3, false);

    connect(this, &QTreeWidget::itemExpanded, [this](QTreeWidgetItem* item){
        bool isMainItem = item->data(0, Qt::UserRole).toBool();  // 主列表项标识
        if (isMainItem) {
            item->setData(0, Qt::UserRole+2, true);
            update();
        }
    });

    connect(this, &QTreeWidget::itemCollapsed, [this](QTreeWidgetItem* item){
        bool isMainItem = item->data(0, Qt::UserRole).toBool();  // 主列表项标识
        if (isMainItem) {
            item->setData(0, Qt::UserRole+2, false);
            update();
        }
    });

    // 设置主列表项展开
    _treeNewFriends->setExpanded(false);
    _treeContacts->setExpanded(false);
    _treeMyApplys->setExpanded(false);

    int uid = UserMgr::GetInstance()->getUid_int();
    std::vector<FriendRequest> friendrequests = DatabaseManager::getNewFriendRequests(uid);
    for(auto& friendrequest: friendrequests){
        int sender_id = friendrequest.sender_id;
        DbUserInfo sender_info;
        auto sender_info_opt = DatabaseManager::getUserInfoByUid(sender_id);
        if(sender_info_opt.has_value()){
            sender_info = sender_info_opt.value();
            CacheManager::getInstance().putUserInfo(sender_info);
            NewFriendReqInfo info(sender_info.uid, sender_info.name, sender_info.icon);
            addNewFriendSubItem(info);
        }
    }

    std::vector<FriendRequest> myfriendapplications =  DatabaseManager::getMyFriendRequests(uid);
    for(auto& myapply : myfriendapplications){
        int receiver_id = myapply.receiver_id;
        DbUserInfo receiver_info;
        auto receiver_info_opt = DatabaseManager::getUserInfoByUid(receiver_id);
        if(receiver_info_opt.has_value()){
            receiver_info = receiver_info_opt.value();
            CacheManager::getInstance().putUserInfo(receiver_info);
            MyApplyRspInfo info(receiver_info.uid, receiver_info.name, receiver_info.icon, myapply.status);
            addMyApplysSubItem(info);
        }
    }

    QVector<qint64> friendids = DatabaseManager::getAllFriendIds();
    std::vector<DbUserInfo> friendinfos = DatabaseManager::getUserInfoByUids(friendids);
    for(auto& friendinfo: friendinfos){
        CacheManager::getInstance().putUserInfo(friendinfo);
        ContactInfo info(friendinfo.uid, friendinfo.name, friendinfo.icon);
        addContactsSubItem(info);
    }
}

void ContactTreeWidget::addNewFriendSubItem(const NewFriendReqInfo &info)
{
    QTreeWidgetItem *childItem1 = new QTreeWidgetItem(_treeNewFriends);
    childItem1->setData(0, Qt::DisplayRole, info._name);
    childItem1->setData(0, Qt::UserRole, false);  // 标记为子列表项
    childItem1->setData(0, Qt::UserRole + 1, true);  // 有新消息标识
    childItem1->setData(0, Qt::UserRole + 3, 1);
    childItem1->setData(0, Qt::UserRole + 4, "通过通讯录好友添加");  // 副标题
    childItem1->setData(0, Qt::UserRole + 5, "等待验证");  // 被申请状态
    childItem1->setData(0, Qt::UserRole + 6, info._uid);
    childItem1->setData(0, Qt::DecorationRole, QPixmap(info._iconurl));  // 头像图片
    NewFriendsubItemMap.insert(info._uid, childItem1);
}

void ContactTreeWidget::addContactsSubItem(const ContactInfo &info)
{
    QTreeWidgetItem *childItem2 = new QTreeWidgetItem(_treeContacts);
    childItem2->setData(0, Qt::DisplayRole, info._name);
    childItem2->setData(0, Qt::UserRole, false);
    childItem2->setData(0, Qt::UserRole + 1, false);  // 无新消息
    childItem2->setData(0, Qt::UserRole + 3, 2);
    childItem2->setData(0, Qt::UserRole + 6, info._uid);
    childItem2->setData(0, Qt::DecorationRole, QPixmap(info._iconurl));
    ContactssubItemMap.insert(info._uid, childItem2);
}

void ContactTreeWidget::addMyApplysSubItem(const MyApplyRspInfo &info)
{
    QTreeWidgetItem *childItem1 = new QTreeWidgetItem(_treeMyApplys);
    childItem1->setData(0, Qt::DisplayRole, info._name);
    childItem1->setData(0, Qt::UserRole, false);  // 标记为子列表项
    childItem1->setData(0, Qt::UserRole + 1, true);  // 有新消息标识
    childItem1->setData(0, Qt::UserRole + 3, 3);
    QString words;
    if(info._state == 0){
        words = "等待对方处理";
    }else if(info._state == 1){
        words = "已接受";
    }else{
        words = "已拒绝";
    }
    childItem1->setData(0, Qt::UserRole + 7, words);  // 被申请状态
    childItem1->setData(0, Qt::UserRole + 6, info._uid);
    childItem1->setData(0, Qt::DecorationRole, QPixmap(info._iconurl));  // 头像图片
    MyApplyssubItemMap.insert(info._uid, childItem1);
}

// 重写鼠标点击事件
void ContactTreeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) { // 检查是否为鼠标 键
        QTreeWidgetItem *item = this->itemAt(event->pos());
        _currentItem = item;
        bool isMainItem = item->data(0, Qt::UserRole).toBool();  // 主列表项标识
        if (!isMainItem) {
            int uid = item->data(0, Qt::UserRole + 6).toInt();
            int type = item->data(0, Qt::UserRole + 3).toInt();
            if(type != 2){
                showDeleteOption(event->globalPos());
            }
        }
    }
    if(event->button() == Qt::LeftButton){
        QTreeWidgetItem *item = this->itemAt(event->pos());
        if(_deleteWidget){
            _deleteWidget->close();
            _deleteWidget = nullptr;
        }
        if(item){
            bool isMainItem = item->data(0, Qt::UserRole).toBool();  // 主列表项标识

            if (!isMainItem) {
                int itemtype = item->data(0, Qt::UserRole + 3).toInt();
                int uid = item->data(0, Qt::UserRole + 6).toInt();
                if(itemtype == 1){
                    qDebug() << "点击了新的朋友子列表项";
                    item->setData(0, Qt::UserRole+1, false);
                    update();
                    emit to_newfriendpage(uid);

                }else if(itemtype == 2){
                    qDebug() << "点击了联系人子列表项";
                    item->setData(0, Qt::UserRole+1, false);
                    update();
                    emit to_friendinfopage(uid);
                }else{
                    qDebug() << "点击了我的申请子列表项";
                    item->setData(0, Qt::UserRole+1, false);
                    update();
                    emit to_myapplypage(uid);
                }
            }
        }
    }
    // 调用基类处理其他鼠标事件
    QTreeWidget::mousePressEvent(event);
}

// 删除子列表项
void ContactTreeWidget::deleteSubItem() {
    int uid = _currentItem->data(0, Qt::UserRole + 6).toInt();

    if (NewFriendsubItemMap.contains(uid)){
        NewFriendsubItemMap.remove(uid);
    }else if (ContactssubItemMap.contains(uid)) {
        ContactssubItemMap.remove(uid);
    }else{
        MyApplyssubItemMap.remove(uid);
    }
    delete _currentItem;// 删除 QTreeWidgetItem
    _currentItem = nullptr;

    _deleteWidget->close();
    _deleteWidget = nullptr;
}

void ContactTreeWidget::showDeleteOption(const QPoint &pos) {

    if(_deleteWidget){
        _deleteWidget->close();
        _deleteWidget = nullptr;
    }
    _deleteWidget = new DeleteWidget(this);
    _deleteWidget->move(pos); // 将按钮移动到鼠标点击位置
    _deleteWidget->show();    // 显示按钮

    connect(_deleteWidget->deleteButton, &QPushButton::clicked, this, &ContactTreeWidget::deleteSubItem);
    connect(_deleteWidget, &DeleteWidget::requestClose, [this](){
        _deleteWidget->close();
        _deleteWidget = nullptr;
    });
}

