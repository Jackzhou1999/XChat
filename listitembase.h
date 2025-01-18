#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include "global.h"

class ListItemBase:public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget* parent=nullptr);
    void SetItemType(ListItemType itemtype);
    ListItemType GetItemType();
private:
    ListItemType _itemType;
};

#endif // LISTITEMBASE_H
