#ifndef CUSTOMITEMDELEGATE_H
#define CUSTOMITEMDELEGATE_H


#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QPixmap>

// 自定义委托类
class CustomItemDelegate : public QStyledItemDelegate {
public:
    CustomItemDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        bool isMainItem = index.data(Qt::UserRole).toBool();  // 判断是否为主列表项
        bool hasNewMessage = index.data(Qt::UserRole + 1).toBool(); // 判断是否有新消息
        bool isExpanded = index.data(Qt::UserRole + 2).toBool();
        int sonType = index.data(Qt::UserRole + 3).toInt();
        QRect rect = option.rect;
        painter->save();
        painter->setRenderHint(QPainter::Antialiasing);

        // 设置选中背景色
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(rect, QColor(220, 220, 220));
        } else {
            if (isMainItem) {
                painter->fillRect(rect, QColor(242, 243, 244));
            }else{
                painter->fillRect(rect, QColor(255, 255, 255));
            }
        }

        if (isMainItem) {
            // 主列表项样式
            QIcon arrowIcon = isExpanded ? QIcon(":/res/down.png"):QIcon(":/res/right.png");
            QRect iconRect(rect.left()+10, rect.top()+4, 15, rect.height()-4);
            arrowIcon.paint(painter, iconRect);

            painter->setPen(Qt::black);
            painter->setFont(QFont("ubuntu", 12));
            QRect textRect = QRect(rect.left() + 34, rect.top(), rect.width(), rect.height());
            painter->drawText(textRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

            painter->setPen(QColor(220, 220, 220));
            int lineY = rect.bottom();
            painter->drawLine(rect.left(), lineY, rect.right(), lineY);
        } else {
            // 子列表项样式
            if(sonType == 1){
                QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
                QRect imageRect = QRect(rect.left() + 5, rect.top() + 10, 40, 40);
                painter->drawPixmap(imageRect, pixmap);

                // 绘制头像右上角红点（新消息标识）
                if (hasNewMessage) {
                    int dotSize = 10;
                    QRect dotRect(imageRect.right() - 5, imageRect.top()-5, dotSize, dotSize);
                    painter->setBrush(Qt::red);
                    painter->setPen(Qt::NoPen);
                    painter->drawEllipse(dotRect);
                }

                // 绘制用户名
                painter->setPen(Qt::black);
                painter->setFont(QFont("ubuntu", 11));
                QRect nameRect = QRect(imageRect.right() + 10, rect.top()+10, rect.width() - 70, 20);
                painter->drawText(nameRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

                // 绘制副标题
                painter->setPen(Qt::gray);
                painter->setFont(QFont("ubuntu", 8));
                QRect subTextRect = QRect(imageRect.right() + 10, rect.top() + 35, rect.width() - 70, 20);
                painter->drawText(subTextRect, Qt::AlignVCenter, index.data(Qt::UserRole + 4).toString());

                // 绘制右侧提示文字
                painter->setPen(Qt::gray);
                painter->setFont(QFont("ubuntu", 9));
                QRect rightTextRect = QRect(rect.right() - 80, rect.top(), 70, rect.height());
                painter->drawText(rightTextRect, Qt::AlignVCenter | Qt::AlignRight, index.data(Qt::UserRole + 5).toString());

                painter->setPen(QColor(220, 220, 220));
                int lineY = rect.bottom();
                painter->drawLine(rect.left(), lineY, rect.right(), lineY);
            }else if(sonType == 2){
                QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
                QRect imageRect = QRect(rect.left() + 5, rect.top() + 10, 40, 40);
                painter->drawPixmap(imageRect, pixmap);

                // 绘制头像右上角红点（新消息标识）
                if (hasNewMessage) {
                    int dotSize = 10;
                    QRect dotRect(imageRect.right() - 5, imageRect.top()-5, dotSize, dotSize);
                    painter->setBrush(Qt::red);
                    painter->setPen(Qt::NoPen);
                    painter->drawEllipse(dotRect);
                }

                // 绘制用户名
                painter->setPen(Qt::black);
                painter->setFont(QFont("ubuntu", 11));
                QRect nameRect = QRect(imageRect.right() + 10, rect.top()+20, rect.width() - 70, 20);
                painter->drawText(nameRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

                painter->setPen(QColor(220, 220, 220));
                int lineY = rect.bottom();
                painter->drawLine(rect.left(), lineY, rect.right(), lineY);
            }else{
                QPixmap pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
                QRect imageRect = QRect(rect.left() + 5, rect.top() + 10, 40, 40);
                painter->drawPixmap(imageRect, pixmap);

                // 绘制头像右上角红点（新消息标识）
                if (hasNewMessage) {
                    int dotSize = 10;
                    QRect dotRect(imageRect.right() - 5, imageRect.top()-5, dotSize, dotSize);
                    painter->setBrush(Qt::red);
                    painter->setPen(Qt::NoPen);
                    painter->drawEllipse(dotRect);
                }

                // 绘制用户名
                painter->setPen(Qt::black);
                painter->setFont(QFont("ubuntu", 11));
                QRect nameRect = QRect(imageRect.right() + 10, rect.top()+20, rect.width() - 70, 20);
                painter->drawText(nameRect, Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

                // 绘制右侧提示文字
                painter->setPen(Qt::gray);
                painter->setFont(QFont("ubuntu", 9));
                QRect rightTextRect = QRect(rect.right() - 80, rect.top(), 70, rect.height());
                painter->drawText(rightTextRect, Qt::AlignVCenter | Qt::AlignRight, index.data(Qt::UserRole + 7).toString());

                painter->setPen(QColor(220, 220, 220));
                int lineY = rect.bottom();
                painter->drawLine(rect.left(), lineY, rect.right(), lineY);
            }
        }

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        bool isMainItem = index.data(Qt::UserRole).toBool();
        return isMainItem ? QSize(0, 40) : QSize(0, 60);  // 自定义主列表项和子列表项高度
    }
};
#endif // CUSTOMITEMDELEGATE_H
