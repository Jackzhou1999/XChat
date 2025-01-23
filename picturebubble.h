#ifndef PICTUREBUBBLE_H
#define PICTUREBUBBLE_H

#include "bubbleframe.h"
#include <QHBoxLayout>
#include <QPixmap>
class PictureBubble : public BubbleFrame
{
    Q_OBJECT
public:
    PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent = nullptr);
};

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QMouseEvent>
#define PIC_MAX_WIDTH 160
#define PIC_MAX_HEIGHT 90

class ImageBubble : public QWidget {
    Q_OBJECT
public:
    explicit ImageBubble(const QString& username, const QPixmap& icon, const QPixmap& image, bool isSender, QWidget* parent = nullptr)
        : QWidget(parent), m_image(image) {
        auto* mainLayout = new QHBoxLayout(this); // 主水平布局
        mainLayout->setContentsMargins(0, 10, 0, 10); // 调整主边距
        mainLayout->setSpacing(5);

        QLabel* avatar = new QLabel(); // 头像
        avatar->setScaledContents(true);
        avatar->setFixedSize(42, 42);
        avatar->setStyleSheet("border-radius: 5px; background-color: gray;");
        avatar->setPixmap(icon);

        // 用户名和图片的垂直布局
        auto* userContentLayout = new QVBoxLayout();
        userContentLayout->setContentsMargins(0, 0, 0, 0); // 去掉内部边距
        userContentLayout->setSpacing(2); // 减小间距为2像素

        QLabel* nameLabel = new QLabel(username); // 用户名
        nameLabel->setObjectName("chat_user_name");
        QFont font("ubuntu");
        font.setPointSize(9);
        nameLabel->setFont(font);
        nameLabel->setFixedHeight(20);

        nameLabel->setContentsMargins(0, 0, 0, 0); // 减小内部边距

        QLabel* imageLabel = new QLabel(); // 图片

        const QPixmap image_scale = scalePixmap(image, 120, 180, 120, 180);
        imageLabel->setPixmap(image_scale);
        imageLabel->setStyleSheet("border-radius: 5px; background-color: black; padding: 0;");
        imageLabel->setContentsMargins(0, 0, 0, 0); // 去掉图片的外边距
        imageLabel->setAlignment(Qt::AlignCenter);



        if (isSender) {
            // 发送方布局：头像在右侧
            mainLayout->addStretch();
            userContentLayout->addWidget(nameLabel, 0, Qt::AlignRight); // 用户名紧贴顶部
            userContentLayout->addWidget(imageLabel, 0, Qt::AlignRight); // 图片紧贴用户名
            mainLayout->addLayout(userContentLayout);
            mainLayout->addWidget(avatar, 0, Qt::AlignTop);
        } else {
            // 接收方布局：头像在左侧
            mainLayout->addWidget(avatar, 0, Qt::AlignTop);
            userContentLayout->addWidget(nameLabel, 0, Qt::AlignLeft); // 用户名紧贴顶部
            userContentLayout->addWidget(imageLabel, 0, Qt::AlignLeft); // 图片紧贴用户名
            mainLayout->addLayout(userContentLayout);
            mainLayout->addStretch();
        }
    }

protected:
    void mouseDoubleClickEvent(QMouseEvent* event) override {
        emit imageDoubleClicked(m_image);
        QWidget::mouseDoubleClickEvent(event);
    }

signals:
    void imageDoubleClicked(const QPixmap& image);

private:
    QPixmap scalePixmap(const QPixmap &pixmap, int minWidth, int maxWidth, int minHeight, int maxHeight) {
        // 获取原始宽高
        int originalWidth = pixmap.width();
        int originalHeight = pixmap.height();

        // 计算缩放比例
        float widthRatio = 1.0f;
        float heightRatio = 1.0f;

        // 根据宽度限制
        if (originalWidth < minWidth) {
            widthRatio = float(minWidth) / originalWidth;
        } else if (originalWidth > maxWidth) {
            widthRatio = float(maxWidth) / originalWidth;
        }

        // 根据高度限制
        if (originalHeight < minHeight) {
            heightRatio = float(minHeight) / originalHeight;
        } else if (originalHeight > maxHeight) {
            heightRatio = float(maxHeight) / originalHeight;
        }

        // 选择适用的缩放比例，保证比例缩放
        float scaleFactor = qMax(widthRatio, heightRatio);  // 确保较小的比例不被忽略

        // 按照缩放比例调整尺寸
        int newWidth = int(originalWidth * scaleFactor);
        int newHeight = int(originalHeight * scaleFactor);

        // 返回缩放后的图像
        return pixmap.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QPixmap m_image;
};
#endif // PICTUREBUBBLE_H
