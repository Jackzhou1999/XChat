#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>

class ImageViewer : public QWidget {
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr);

    void setImage(QPixmap pixmap) {
        _image = pixmap;
        if (!pixmap.isNull()) {
            // 获取图片原始大小
            QSize originalSize = pixmap.size();
            int originalWidth = originalSize.width();
            int originalHeight = originalSize.height();

            QSize finalSize(600, 600);

            // 检查是否需要缩放
            if (std::max(originalWidth, originalHeight) > 600) {
                // 计算缩放比例
                double scaleFactor = 600.0 / std::max(originalWidth, originalHeight);
                finalSize = QSize(originalWidth * scaleFactor, originalHeight * scaleFactor);
                _scalenumber->setText(QString::number(int(scaleFactor*100))+"%");
                // 按比例缩放图片
                pixmap = pixmap.scaled(finalSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            _scaleimage = pixmap;
            // 设置 QLabel 的大小为图片的最终显示大小
            imageLabel->setMinimumSize(finalSize);

            // 将图片设置到 QLabel
            imageLabel->setPixmap(pixmap);
        } else {
            qDebug() << "加载图片失败";
        }
    }
protected:
    void resizeEvent(QResizeEvent *event) override {
        Q_UNUSED(event);
        updatePixmap();
    }

private slots:
    // 放大图片
    void zoomIn() {
        if(_scaleindex < 11){
            ++_scaleindex;
            updateImage();
        }
    }

    // 缩小图片
    void zoomOut() {
        if(_scaleindex > 0){
            --_scaleindex;
            updateImage();
        }
    }

    // 旋转图片
    void rotateImage() {
        // 增加旋转角度
        _rotationAngle = (_rotationAngle + 90) % 360;

        // 创建旋转变换
        QTransform transform;
        transform.rotate(90);
        QPixmap currentpixmap = imageLabel->pixmap();
        // 应用旋转到图片
        QPixmap rotatedPixmap = currentpixmap.transformed(transform, Qt::SmoothTransformation);
        // 更新 QLabel 显示的图片
        imageLabel->setPixmap(rotatedPixmap);
    }

    // 下载图片
    void saveImage() {
        // 添加保存逻辑
    }

private:
    void updatePixmap() {
        if (!_image.isNull()) {
            // 获取窗口大小
            QSize availableSize = imageLabel->size();
            qDebug() << availableSize;

            QPixmap scaledPixmap = _image;
            int originalWidth = scaledPixmap.width();
            int originalHeight = scaledPixmap.height();

            if (std::max(originalWidth, originalHeight) > 600) {
                // 将图片缩放到 QLabel 的大小范围内，保持比例
                scaledPixmap = _image.scaled(availableSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }

            double scaleFactor = double(scaledPixmap.height())/_image.height();
            _scalenumber->setText(QString::number(int(scaleFactor*100))+"%");

            // 应用旋转到图片
            if(_rotationAngle != 0){
                QTransform transform;
                transform.rotate(_rotationAngle);
                scaledPixmap = scaledPixmap.transformed(transform, Qt::SmoothTransformation);
            }

            // 设置缩放后的图片到 QLabel
            imageLabel->setPixmap(scaledPixmap);
            imageLabel->setAlignment(Qt::AlignCenter);
        }
    }

    void updateImage() {
        if (_image.isNull())
            return;
        QPixmap pixmap = _scaleimage;
        // 根据缩放比例调整图片大小
        QPixmap scaledPixmap = pixmap.scaled(pixmap.size() * _scaleFactorList[_scaleindex], Qt::KeepAspectRatio, Qt::SmoothTransformation);

        double scaleFactor = double(scaledPixmap.height())/_image.height();
        _scalenumber->setText(QString::number(int(scaleFactor*100))+"%");
        // 确保图片始终居中

        // 应用旋转到图片
        if(_rotationAngle != 0){
            QTransform transform;
            transform.rotate(_rotationAngle);
            scaledPixmap = scaledPixmap.transformed(transform, Qt::SmoothTransformation);
        }
        imageLabel->setPixmap(scaledPixmap);
        imageLabel->setAlignment(Qt::AlignCenter);
    }



    QLabel *imageLabel;  // 图片显示控件
    QPixmap _image;

    QPixmap _scaleimage;
    std::vector<qreal> _scaleFactorList;      // 缩放比例
    size_t _scaleindex;
    QLabel *_scalenumber;

    int _rotationAngle;
};

#endif // IMAGEVIEWER_H
