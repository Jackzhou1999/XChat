#include "imageviewer.h"

ImageViewer::ImageViewer(QWidget *parent) : QWidget(parent), _rotationAngle(0) {
    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    setWindowTitle(tr("图片查看器"));
    mainLayout->setContentsMargins(0,0,0,0);
    _scaleFactorList = std::vector<qreal>{0.1, 0.3, 0.5, 0.7, 0.9, 1, 1.1, 1.3, 1.5, 1.7, 1.9, 2};
    _scaleindex = 5;
    // 图片显示区域
    imageLabel = new QLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet("background-color: #000;");  // 背景设置为黑色
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mainLayout->addWidget(imageLabel, 9);

    // 弹性间隔，将按钮栏固定到底部
    mainLayout->addStretch();

    // 底部按钮栏布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QSpacerItem *sp1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Maximum);
    QSpacerItem *sp2 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);
    QSpacerItem *sp3 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);
    QSpacerItem *sp4 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);
    QSpacerItem *sp5 = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Maximum);

    QSpacerItem *sp6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Maximum);


    // 按钮
    QPushButton *zoomInButton = new QPushButton(this);
    zoomInButton->setFixedSize(35, 35);
    zoomInButton->setStyleSheet("border: none; border-image: url(:/res/fangda.png);");

    _scalenumber = new QLabel(this);
    QFont font("ubuntu");
    font.setPointSize(12);
    _scalenumber->setFont(font);
    _scalenumber->setAlignment(Qt::AlignCenter);
    _scalenumber->setFixedSize(40,35);

    QPushButton *zoomOutButton = new QPushButton(this);
    zoomOutButton->setFixedSize(35, 35);
    zoomOutButton->setStyleSheet("border: none; border-image: url(:/res/suoxiao.png);");

    QPushButton *rotateButton = new QPushButton(this);
    rotateButton->setFixedSize(35, 35);
    rotateButton->setStyleSheet("border: none; border-image: url(:/res/xuanzhaun.png);");

    QPushButton *saveButton = new QPushButton(this);
    saveButton->setFixedSize(35, 35);
    saveButton->setStyleSheet("border: none; border-image: url(:/res/xiazai.png);");

    // 添加按钮到按钮布局
    buttonLayout->addItem(sp1);
    buttonLayout->addWidget(zoomInButton);
    buttonLayout->addItem(sp2);

    buttonLayout->addWidget(_scalenumber);
    buttonLayout->addItem(sp3);

    buttonLayout->addWidget(zoomOutButton);
    buttonLayout->addItem(sp4);

    buttonLayout->addWidget(rotateButton);
    buttonLayout->addItem(sp5);

    buttonLayout->addWidget(saveButton);
    buttonLayout->addItem(sp6);


    // 将按钮布局添加到主布局
    mainLayout->addLayout(buttonLayout, 1);

    // 设置主布局
    setLayout(mainLayout);

    // 信号与槽（可进一步完善功能）
    connect(zoomInButton, &QPushButton::clicked, this, &ImageViewer::zoomIn);
    connect(zoomOutButton, &QPushButton::clicked, this, &ImageViewer::zoomOut);
    connect(rotateButton, &QPushButton::clicked, this, &ImageViewer::rotateImage);
    connect(saveButton, &QPushButton::clicked, this, &ImageViewer::saveImage);
}
