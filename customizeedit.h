#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>
#include <QPainter>


class CustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit CustomLineEdit(QWidget *parent = nullptr) : QLineEdit(parent),_max_length(0) {
        connect(this, &CustomLineEdit::textChanged, this, &CustomLineEdit::limitTextLength);
        installEventFilter(this);
    }

    void SetMaxLength(uint len);
    void setPlaceholderOffset(int offsetx, int offsety) {
        m_placeholderOffsetx = offsetx; // shuiping偏移量
        m_placeholderOffsety = offsety;
        update(); // 触发重绘
    }

    // 重写 setPlaceholderText，保存自定义占位符文本
    void setPlaceholderText(const QString &text) {
        m_customPlaceholderText = text;
        update(); // 触发重绘
    }

protected:
    void focusInEvent(QFocusEvent *event) override {
        // 清空默认占位符，防止焦点获取时重新绘制默认占位符
        QLineEdit::setPlaceholderText("");
        QLineEdit::focusInEvent(event);
    }

    void focusOutEvent(QFocusEvent *event) override {
        // 焦点丢失时恢复占位符，仅在我们手动绘制时使用
        QLineEdit::setPlaceholderText("");
        QLineEdit::focusOutEvent(event);
    }

    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *object, QEvent *event);

private:
    void limitTextLength();

    QString m_customPlaceholderText; // 存储自定义占位符文本
    int m_placeholderOffsetx = 0;     // 默认水平偏移量为0
    int m_placeholderOffsety = 0;     // 默认水平偏移量为0
    int _max_length;
signals:
    void sig_switch_search();
};
#endif // CUSTOMIZEEDIT_H
