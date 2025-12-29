#ifndef IMAGETRANSFORM_H
#define IMAGETRANSFORM_H

#include <QMainWindow>
#include <QWindow>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDial>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>

class ImageTransform : public QWidget
{
    Q_OBJECT

public:
    ImageTransform(QWidget *parent = nullptr);
    ~ImageTransform();
    QLabel        *inWin;
    QGroupBox     *mirrorGroup;
    QCheckBox     *hCheckBox;
    QCheckBox     *vCheckBox;
    QPushButton   *mirrorButton;
    QPushButton   *saveButton;
    QDial         *rotateDial;
    QSpacerItem   *vSpacer;
    QHBoxLayout   *mainLayout;
    QVBoxLayout   *groupLayout;
    QVBoxLayout   *leftLayout;
    QImage        srcImg;
    QImage        dstImg;

private slots:
    void mirroredImage();
    void rotatedImage();
    void saveDstImage();
};

#endif // IMAGETRANSFORM_H
