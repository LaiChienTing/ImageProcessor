#include "zoomwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QPainter>
#include <QDebug>
#include <QScrollArea>
#include <QLabel>
#include <QToolButton>
#include <QEvent>

// 建構子：初始化放大視窗
ZoomWindow::ZoomWindow(const QImage &sourceImage, const QRect &selectedRect, double zoomFactor, QWidget *parent)
    : QMainWindow(parent), zoomFactor(zoomFactor), drawing(false), penColor(Qt::red), penWidth(3)
{
    setWindowTitle(QStringLiteral("區域放大視窗"));
    
    // 擷取選取的區域
    originalImage = sourceImage.copy(selectedRect);
    
    // 根據放大倍率縮放圖片
    int newWidth = originalImage.width() * zoomFactor;
    int newHeight = originalImage.height() * zoomFactor;
    zoomedImage = originalImage.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // 建立可繪圖的圖片副本
    drawingImage = zoomedImage.copy();
    
    // 建立捲軸區域以容納大圖片
    QScrollArea *scrollArea = new QScrollArea;
    imageLabel = new QLabel;
    imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
    imageLabel->setMouseTracking(true);
    imageLabel->installEventFilter(this);  // 安裝事件過濾器以捕捉滑鼠事件
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(false);
    
    setCentralWidget(scrollArea);
    
    createActions();
    createToolBars();
    
    // 啟用滑鼠追蹤
    setMouseTracking(true);
    
    // 設定視窗大小
    resize(800, 600);
}

ZoomWindow::~ZoomWindow()
{
}

// 建立動作選單
void ZoomWindow::createActions()
{
    // 存檔動作
    saveAction = new QAction(QStringLiteral("存檔"), this);
    saveAction->setStatusTip(QStringLiteral("儲存目前的圖片"));
    connect(saveAction, &QAction::triggered, this, &ZoomWindow::saveImage);
    
    // 畫筆顏色動作
    penColorAction = new QAction(QStringLiteral("畫筆顏色"), this);
    penColorAction->setStatusTip(QStringLiteral("選擇畫筆顏色"));
    connect(penColorAction, &QAction::triggered, this, &ZoomWindow::choosePenColor);
    
    // 清除繪圖動作
    clearAction = new QAction(QStringLiteral("清除繪圖"), this);
    clearAction->setStatusTip(QStringLiteral("清除所有繪圖內容"));
    connect(clearAction, &QAction::triggered, this, &ZoomWindow::clearDrawing);
}

// 建立工具列
void ZoomWindow::createToolBars()
{
    toolBar = addToolBar(QStringLiteral("工具"));
    toolBar->addAction(saveAction);
    toolBar->addAction(penColorAction);
    toolBar->addAction(clearAction);
    
    // 加入畫筆寬度控制
    QLabel *widthLabel = new QLabel(QStringLiteral(" 畫筆寬度: "));
    toolBar->addWidget(widthLabel);
    
    penWidthSlider = new QSlider(Qt::Horizontal);
    penWidthSlider->setMinimum(1);
    penWidthSlider->setMaximum(20);
    penWidthSlider->setValue(penWidth);
    penWidthSlider->setMaximumWidth(150);
    connect(penWidthSlider, &QSlider::valueChanged, this, &ZoomWindow::penWidthChanged);
    toolBar->addWidget(penWidthSlider);
}

// 存檔功能
void ZoomWindow::saveImage()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    QStringLiteral("儲存圖片"),
                                                    ".",
                                                    "PNG (*.png);;JPEG (*.jpg);;BMP (*.bmp)");
    if (!filename.isEmpty())
    {
        bool ok = drawingImage.save(filename);
        if (ok)
        {
            statusBar()->showMessage(QStringLiteral("圖片已儲存"), 3000);
        }
        else
        {
            statusBar()->showMessage(QStringLiteral("儲存失敗"), 3000);
        }
    }
}

// 選擇畫筆顏色
void ZoomWindow::choosePenColor()
{
    QColor color = QColorDialog::getColor(penColor, this, QStringLiteral("選擇畫筆顏色"));
    if (color.isValid())
    {
        penColor = color;
        statusBar()->showMessage(QStringLiteral("畫筆顏色已更改"), 2000);
    }
}

// 畫筆寬度改變
void ZoomWindow::penWidthChanged(int width)
{
    penWidth = width;
    statusBar()->showMessage(QStringLiteral("畫筆寬度: ") + QString::number(width), 2000);
}

// 清除繪圖
void ZoomWindow::clearDrawing()
{
    // 重設為原始放大圖片
    drawingImage = zoomedImage.copy();
    imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
    statusBar()->showMessage(QStringLiteral("繪圖已清除"), 2000);
}

// 滑鼠按下事件：開始繪圖
void ZoomWindow::mousePressEvent(QMouseEvent *event)
{
    // 這個方法保留用於主視窗級別的事件處理
    QMainWindow::mousePressEvent(event);
}

// 滑鼠移動事件：繪製線條
void ZoomWindow::mouseMoveEvent(QMouseEvent *event)
{
    // 這個方法保留用於主視窗級別的事件處理
    QMainWindow::mouseMoveEvent(event);
}

// 滑鼠釋放事件：結束繪圖
void ZoomWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // 這個方法保留用於主視窗級別的事件處理
    QMainWindow::mouseReleaseEvent(event);
}

// 事件過濾器：處理 imageLabel 上的滑鼠事件
bool ZoomWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == imageLabel)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton)
            {
                lastPoint = mouseEvent->pos();
                drawing = true;
                return true;
            }
        }
        else if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if ((mouseEvent->buttons() & Qt::LeftButton) && drawing)
            {
                drawLineTo(mouseEvent->pos());
                return true;
            }
        }
        else if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (mouseEvent->button() == Qt::LeftButton && drawing)
            {
                drawLineTo(mouseEvent->pos());
                drawing = false;
                return true;
            }
        }
    }
    
    return QMainWindow::eventFilter(watched, event);
}

// 繪製線條從上一個點到當前點
void ZoomWindow::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&drawingImage);
    painter.setPen(QPen(penColor, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(lastPoint, endPoint);
    
    lastPoint = endPoint;
    
    // 更新顯示
    imageLabel->setPixmap(QPixmap::fromImage(drawingImage));
}
