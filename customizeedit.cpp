#include "customizeedit.h"
#include <QEvent>

void CustomLineEdit::SetMaxLength(uint len)
{
    _max_length = len;
}

void CustomLineEdit::paintEvent(QPaintEvent *event)
{
    // 调用默认的绘制逻辑
    QLineEdit::paintEvent(event);

    // 手动绘制占位符，仅在文本为空时
    if (text().isEmpty() && !m_customPlaceholderText.isEmpty()) {
        QPainter painter(this);
        painter.setPen(QColor(98, 101, 103)); // 设置占位符颜色
        // 设置水平偏移量
        QRect placeholderRect(m_placeholderOffsetx, m_placeholderOffsety, width() - m_placeholderOffsetx, height());
        // painter.drawText(placeholderRect, Qt::AlignLeft | Qt::AlignVCenter, m_customPlaceholderText);
        painter.drawText(placeholderRect, Qt::AlignLeft, m_customPlaceholderText);

    }
}


void CustomLineEdit::limitTextLength()
{
    if(_max_length <= 0) return;
    QByteArray text = this->text().toUtf8();
    if(text.size() > _max_length){
        text = text.left(_max_length);
        this->setText(QString::fromUtf8(text));
    }
}

void CustomLineEdit::slot_user_search()
{
    QString msg = text();
    emit sig_user_search(msg);
}

bool CustomLineEdit::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        emit sig_switch_search();
    }

    return QLineEdit::eventFilter(object, event);
}

