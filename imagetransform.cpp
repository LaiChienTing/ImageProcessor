#include "imagetransform.h"
#include <QPixmap>
#include <QSizePolicy>
#include <QPainter>
#include <QFileDialog>
#include <QDebug>

ImageTransform::ImageTransform(QWidget *parent)
    : QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    leftLayout = new QVBoxLayout(this);
    mirrorGroup = new QGroupBox(tr("鏡射"), this);
    groupLayout = new QVBoxLayout(mirrorGroup);

    hCheckBox = new QCheckBox(tr("水平"), mirrorGroup);
    vCheckBox = new QCheckBox(tr("垂直"), mirrorGroup);
    mirrorButton = new QPushButton(tr("執行"), mirrorGroup);
    hCheckBox -> setGeometry(QRect(13, 28, 87, 19));
    vCheckBox -> setGeometry(QRect(13, 54, 87, 19));
    mirrorButton -> setGeometry(QRect(13, 80, 93, 28));
    saveButton = new QPushButton(tr("存檔"), this);
    groupLayout -> addWidget(hCheckBox);
    groupLayout -> addWidget(vCheckBox);
    groupLayout -> addWidget(mirrorButton);
    leftLayout -> addWidget(mirrorGroup);
    rotateDial = new QDial(this);
    rotateDial -> setNotchesVisible(true);
    vSpacer = new QSpacerItem(20, 58, QSizePolicy::Minimum, QSizePolicy::Expanding);

    leftLayout -> addWidget(rotateDial);
    leftLayout -> addWidget(saveButton);
    leftLayout -> addItem(vSpacer);
    mainLayout -> addLayout(leftLayout);

    inWin = new QLabel(this);
    inWin -> setScaledContents(true);
    QPixmap *initPixmap = new QPixmap(300, 200);
    initPixmap -> fill(QColor(255, 255, 255));


    inWin -> setPixmap(*initPixmap);
    inWin -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // for test
    /*--------------------------------------------------*/
    QPainter *paint = new QPainter(initPixmap);
    paint -> setPen(*(new QColor(0, 0, 0)));
    paint -> begin(initPixmap);
    paint -> drawRect(15, 15, 60, 40);
    paint -> end();
    /*--------------------------------------------------*/

    if (srcImg.isNull())
    {
        srcImg = initPixmap -> toImage();
    }

    inWin -> setPixmap(*initPixmap);
    mainLayout -> addWidget(inWin);
    connect(mirrorButton, SIGNAL(clicked(bool)), this, SLOT(mirroredImage()));
    connect(rotateDial, SIGNAL(valueChanged(int)), this, SLOT(rotatedImage()));
    connect(saveButton, SIGNAL(clicked(bool)), this, SLOT(saveDstImage()));
}

ImageTransform::~ImageTransform()
{

}

void ImageTransform::mirroredImage()
{
    bool H, V;
    //if (srcImg.isNull()) return;
    H = hCheckBox -> isChecked();
    V = vCheckBox -> isChecked();
    dstImg = srcImg.mirrored(H, V);
    inWin -> setPixmap(QPixmap::fromImage(dstImg));
}

void ImageTransform::rotatedImage()
{
    QTransform tran;
    //if (srcImg.isNull()) return;
    int angle = rotateDial -> value();
    tran.rotate(angle);
    dstImg = srcImg.transformed(tran);
    inWin -> setPixmap(QPixmap::fromImage(dstImg));
}

void ImageTransform::saveDstImage()
{
    QString filename;
    filename = QFileDialog::getSaveFileName(this, "保存影像", "..\\..\\");
    if (filename.isEmpty())
        return;
    bool ok;
    if(dstImg.isNull())
        ok = srcImg.save(filename, "PNG", 100);
    else
        ok = dstImg.save(filename, "PNG", 100);
    qDebug() << filename << " " << ok;
}
