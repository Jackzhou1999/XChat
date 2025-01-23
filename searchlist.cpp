#include "searchlist.h"
#include<QScrollBar>
#include "adduseritem.h"
#include "tcpmgr.h"
#include "customizeedit.h"
#include "loadingdlg.h"
#include "userdata.h"
#include "usermgr.h"
#include "searchuseritem.h"

SearchList::SearchList(QWidget *parent):QListWidget(parent), _search_edit(nullptr), _send_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::addTipItem()
{
    auto *invalid_item = new QWidget();
    QListWidgetItem *item_tmp = new QListWidgetItem;
    item_tmp->setSizeHint(QSize(250,8));
    this->addItem(item_tmp);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item_tmp, invalid_item);
    item_tmp->setFlags(item_tmp->flags() & ~Qt::ItemIsSelectable);


    auto *add_user_item = new AddUserItem();
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(add_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, add_user_item);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget *widget = this->itemWidget(item); // 获取自定义widget对象
    if(!widget){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    // 对自定义widget进行操作， 将item 转化为基类ListItemBase
    ListItemBase *customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem){
        qDebug()<< "slot item clicked widget is nullptr";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM){
        qDebug()<< "slot invalid item clicked ";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM){
        emit to_globalsearchpage();
        return;
    }
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> si)
{

}


void SearchList::addUserItem(const DbUserInfo& info)
{
    auto *search_user_item = new SearchUserItem();
    search_user_item->setInfo(info);
    QListWidgetItem *item = new QListWidgetItem;
    item->setSizeHint(search_user_item->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, search_user_item);
    connect(search_user_item, &SearchUserItem::sig_tofriendinfopage, this, &SearchList::to_friendinfopage);
}

void SearchList::ClearItem(){
    for (int i = 0; i < count(); ++i) {
        QListWidgetItem* searchuserItem = item(i);
        QWidget* widget = itemWidget(searchuserItem);
        // 检查 itemWidget 是否是 SearchUserItem 类型
        if (widget && qobject_cast<SearchUserItem*>(widget)) {
            // 删除 widget 和 QListWidgetItem
            removeItemWidget(searchuserItem);
            delete widget;  // 删除 widget
            delete searchuserItem;    // 删除 QListWidgetItem
            // 减少索引，继续检查下一个项
            --i;
        }
    }
}
