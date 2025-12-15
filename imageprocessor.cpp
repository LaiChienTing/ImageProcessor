#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("影像處理"));
    central = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    imgWin = new QLabel();
    QPixmap *initPixmap = new QPixmap(300,200);
    initPixmap->fill(QColor(255,255,255));
    imgWin->resize(300,200);
    imgWin->setScaledContents(true);
    imgWin->setPixmap(*initPixmap);
    mainLayout->addWidget(imgWin);
    setCentralWidget(central);
    createActions();
    createMenus();
    createToolBars();
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

    fileMenu = menuBar()->addMenu(QStringLiteral("工具(&T)"));
    fileMenu->addAction(zoomInAction);
    fileMenu->addAction(zoomOutAction);
}

void ImageProcessor::createToolBars()
{
    fileTool = addToolBar("file");
    fileTool->addAction(openFileAction);

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
