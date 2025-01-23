#include "searchuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>

SearchUserList::SearchUserList(QWidget *parent) {
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);
}

bool SearchUserList::eventFilter(QObject *object, QEvent *event)
{
    if(object == this->viewport()){
        if(event->type() == QEvent::Enter){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    if(object == this->viewport() && event->type() == QEvent::Wheel){
        QWheelEvent *wheelevent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelevent->angleDelta().y()/8;
        int numSteps = numDegrees / 15;

        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value()-numSteps);

        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();

        if(maxScrollValue - currentValue <= 0){
            qDebug() << "search load more user";
            emit sig_loading_more_user();
        }
        return false;
    }
    return QListWidget::eventFilter(object, event);
}
