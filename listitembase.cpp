#include "listitembase.h"

ListItemBase::ListItemBase(QWidget *parent):QWidget(parent)
{

}


void ListItemBase::SetItemType(ListItemType itemtype)
{
    _itemType = itemtype;
}

ListItemType ListItemBase::GetItemType()
{
    return _itemType;
}
