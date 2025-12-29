#include "imageprocessor.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QDebug>
#include <QPixmap>
#include <QPainter>
#include <QInputDialog>
#include "imagetransform.h"
#include "zoomwindow.h"

ImageProcessor::ImageProcessor(QWidget *parent)
    : QMainWindow(parent), isSelecting(false)  // 初始化區域選取狀態
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
    geometryAction->setStatusTip(QStringLiteral("幾何轉換"));
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

    zoomOutAction = new QAction(QStringLiteral("縮小(&-)"), this);
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
    
    // 更新選取區域
    if (isSelecting)
    {
        QPoint labelPos = imgWin->mapFrom(this, event->pos());
        selectionEnd = labelPos;
        update();  // 觸發重繪以顯示選取框
    }
}

void ImageProcessor::mousePressEvent(QMouseEvent * event)
{
    QString str = "(" + QString::number(event->x()) + ", " +
                  QString::number(event->y()) + ")";
    if (event->button() == Qt::LeftButton)
    {
        statusBar()->showMessage(QStringLiteral("左鍵:") + str);
        
        // 開始區域選取（需按住 Ctrl 鍵）
        if (event->modifiers() & Qt::ControlModifier && !img.isNull())
        {
            // 轉換座標到 label 座標
            QPoint labelPos = imgWin->mapFrom(this, event->pos());
            if (imgWin->rect().contains(labelPos))
            {
                isSelecting = true;
                // 儲存 label 座標用於繪製選取框
                selectionStart = labelPos;
                selectionEnd = labelPos;
                statusBar()->showMessage(QStringLiteral("開始選取區域: ") + str);
            }
        }
    }
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
    
    // 完成區域選取
    if (isSelecting && event->button() == Qt::LeftButton)
    {
        isSelecting = false;
        
        // 將 label 座標轉換為實際圖片座標
        QPoint imgStart = labelToImageCoords(selectionStart);
        QPoint imgEnd = labelToImageCoords(selectionEnd);
        
        // 建立選取矩形（在圖片座標系統中）
        QPoint topLeft(qMin(imgStart.x(), imgEnd.x()),
                      qMin(imgStart.y(), imgEnd.y()));
        QPoint bottomRight(qMax(imgStart.x(), imgEnd.x()),
                          qMax(imgStart.y(), imgEnd.y()));
        
        selectionRect = QRect(topLeft, bottomRight);
        
        // 確保選取區域有效且在圖片範圍內
        if (selectionRect.width() > 10 && selectionRect.height() > 10)
        {
            // 限制在圖片範圍內
            selectionRect = selectionRect.intersected(QRect(0, 0, img.width(), img.height()));
            
            if (!selectionRect.isEmpty())
            {
                statusBar()->showMessage(QStringLiteral("區域已選取，正在開啟放大視窗..."), 2000);
                openZoomWindow();
            }
        }
        update();  // 清除選取框
    }
}

// 繪製選取框
void ImageProcessor::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
    
    // 繪製選取框
    if (isSelecting)
    {
        QPainter painter(this);
        painter.setPen(QPen(Qt::blue, 2, Qt::DashLine));
        
        QPoint start = imgWin->mapTo(this, selectionStart);
        QPoint end = imgWin->mapTo(this, selectionEnd);
        
        QRect rect(start, end);
        painter.drawRect(rect.normalized());
    }
}

// 開啟放大視窗
void ImageProcessor::openZoomWindow()
{
    if (img.isNull() || selectionRect.isEmpty())
    {
        return;
    }
    
    // 詢問使用者放大倍率
    bool ok;
    double zoomFactor = QInputDialog::getDouble(this,
                                                QStringLiteral("設定放大倍率"),
                                                QStringLiteral("請輸入放大倍率 (1.0 - 10.0):"),
                                                2.0, 1.0, 10.0, 1, &ok);
    
    if (ok)
    {
        // 建立並顯示放大視窗
        ZoomWindow *zoomWin = new ZoomWindow(img, selectionRect, zoomFactor);
        zoomWin->setAttribute(Qt::WA_DeleteOnClose);  // 關閉時自動刪除
        zoomWin->show();
    }
}

// 將 label 座標轉換為實際圖片座標
QPoint ImageProcessor::labelToImageCoords(const QPoint &labelPos)
{
    if (img.isNull() || !imgWin || imgWin->pixmap() == nullptr)
        return labelPos;
    
    // 取得 label 和圖片的尺寸
    QSize labelSize = imgWin->size();
    QSize imgSize = img.size();
    
    // 因為使用 setScaledContents(true)，圖片會縮放以填滿 label
    // 計算縮放比例
    double scaleX = static_cast<double>(imgSize.width()) / labelSize.width();
    double scaleY = static_cast<double>(imgSize.height()) / labelSize.height();
    
    // 轉換座標
    int imgX = static_cast<int>(labelPos.x() * scaleX);
    int imgY = static_cast<int>(labelPos.y() * scaleY);
    
    // 確保座標在圖片範圍內
    imgX = qBound(0, imgX, img.width() - 1);
    imgY = qBound(0, imgY, img.height() - 1);
    
    return QPoint(imgX, imgY);
}


