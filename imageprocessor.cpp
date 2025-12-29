#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include "imagetransform.h"

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300,200);
    gWin = new ImageTransform();
    initPixmap->fill(QColor(255,255,255));
    imgWin->resize(300,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    createActions();
    createMenus();
    createToolBars();
    statusLabel = new QLabel;
    statusLabel->setText(QStringLiteral("指標位置"));
    statusLabel->setFixedWidth(100);
    MousePosLabel = new QLabel;
    MousePosLabel->setText(tr(" "));
    MousePosLabel->setFixedWidth(100);
    statusBar()->addPermanentWidget(statusLabel);
    statusBar()->addPermanentWidget(MousePosLabel);
    setMouseTracking(true);
    imgWin->setMouseTracking(true);
    central->setMouseTracking(true);
}

ImageProcessor::~ImageProcessor()
{

}

void ImageProcessor::createActions()
{
    openFileAction = new QAction(QStringLiteral("開啟檔案(&O)"), this);
    openFileAction->setShortcut(tr("Ctrl+O"));
    openFileAction->setStatusTip(QStringLiteral("開啟影像檔案"));
    connect(openFileAction, SIGNAL(triggered()), this, SLOT(showOpenFile()));

    exitAction = new QAction(QStringLiteral("結束(&Q)"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(QStringLiteral("退出程式"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    geometryAction = new QAction(QStringLiteral("幾何轉換"), this);
    geometryAction->setShortcut(tr("Ctrl+G"));
    geometryAction->setStatusTip(QStringLiteral("幾何幾何轉換"));
    connect(geometryAction, SIGNAL(triggered()), this, SLOT(showGeometryTransform()));
    connect(exitAction, SIGNAL(triggered()), gWin, SLOT(close()));

    zoomInAction = new QAction(QStringLiteral("放大(&+)"), this);
    zoomInAction->setShortcut(tr("Ctrl++"));
    connect(zoomInAction, &QAction::triggered, this, [=]() {

        if (img.isNull()) return;

        QImage result = img.scaled(
            img.width() * 1.5,
            img.height() * 1.5,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        ImageProcessor *resultWin = new ImageProcessor();
        resultWin->setWindowTitle(QStringLiteral("處理結果"));
        resultWin->show();
        resultWin->loadImage(result);
    });

    zoomOutAction = new QAction(QStringLiteral("縮小(&-)"), this);\
    zoomOutAction->setShortcut(tr("Ctrl+-"));
    connect(zoomOutAction, &QAction::triggered, this, [=]() {

        if (img.isNull()) return;

        QImage result = img.scaled(
            img.width() * 0.5,
            img.height() * 0.5,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            );

        ImageProcessor *resultWin = new ImageProcessor();
        resultWin->setWindowTitle(QStringLiteral("處理結果"));
        resultWin->show();
        resultWin->loadImage(result);
    });
}

void ImageProcessor::createMenus()
{
    fileMenu = menuBar()->addMenu(QStringLiteral("檔案(&F)"));
    fileMenu->addAction(openFileAction);
    fileMenu->addAction(exitAction);
    fileMenu->addAction(geometryAction);

    fileMenu = menuBar()->addMenu(QStringLiteral("工具(&T)"));
    fileMenu->addAction(zoomInAction);
    fileMenu->addAction(zoomOutAction);
}

void ImageProcessor::createToolBars()
{
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);
    fileTool->addAction(geometryAction);

    fileTool = addToolBar("tool");
    fileTool->addAction(zoomInAction);
    fileTool->addAction(zoomOutAction);
}

void ImageProcessor::loadFile(QString filename)
{
    img.load(filename);
    scaleFactor = 1.0;
    imgWin->setPixmap(QPixmap::fromImage(img));
    imgWin->adjustSize();
}

void ImageProcessor::loadImage(const QImage &image)
{
    img = image;
    scaleFactor = 1.0;
    imgWin->setPixmap(QPixmap::fromImage(img));
    imgWin->adjustSize();
}

void ImageProcessor::showOpenFile()
{
    filename = QFileDialog::getOpenFileName(this,
                                            QStringLiteral("開啟影像"),
                                            tr("."),
                                            "bmp(*.bmp);;png(*.png);;jpeg(*.jpg)");

    if (!filename.isEmpty())
    {
        if (img.isNull())
        {
            loadFile(filename);
        }
        else
        {
            ImageProcessor *newIPWin = new ImageProcessor();
            newIPWin->show();
            newIPWin->loadFile(filename);
        }
    }
}

void ImageProcessor::showGeometryTransform()
{
    if(!img.isNull())
    {
        gWin->srcImg = img;
        gWin->inWin->setPixmap(QPixmap::fromImage(gWin->srcImg));
        gWin->show();
    }
}

void ImageProcessor::mouseDoubleClickEvent(QMouseEvent * event)
{
    QString str = "(" + QString::number(event->x()) + ", " +
                  QString::number(event->y()) + ")";
    statusBar()->showMessage(QStringLiteral("雙擊")+str);
    qDebug()<< "雙擊";
}

void ImageProcessor::mouseMoveEvent(QMouseEvent * event)
{
    int x = event->x();
    int y = event->y();
    QString str = "(" + QString::number(x) + ", " +
                  QString::number(y) + ")";
    if (!img.isNull() && x >= 0 && x < img.width() && y >= 0 && y < img.height())
    {
        int gray = qGray(img.pixel(x, y));
        str += " = " + QString::number(gray);
    }

    MousePosLabel->setText(str);
}

void ImageProcessor::mousePressEvent(QMouseEvent * event)
{
    QString str = "(" + QString::number(event->x()) + ", " +
                  QString::number(event->y()) + ")";
    if (event->button() == Qt::LeftButton)
        statusBar()->showMessage(QStringLiteral("左鍵:") + str);
    else if (event->button() == Qt::RightButton)
        statusBar()->showMessage(QStringLiteral("右鍵:") + str);
    else if (event->button() == Qt::MiddleButton)
        statusBar()->showMessage(QStringLiteral("中鍵:") + str);
    qDebug()<< "按壓";
}

void ImageProcessor::mouseReleaseEvent(QMouseEvent * event)
{
    QString str = "(" + QString::number(event->x()) + ", " +
                  QString::number(event->y()) + ")";
    statusBar()->showMessage(QStringLiteral("釋放:")+str);
    qDebug()<< "釋放";
}
