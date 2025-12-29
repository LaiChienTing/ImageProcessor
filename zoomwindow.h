#ifndef ZOOMWINDOW_H
#define ZOOMWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QMouseEvent>
#include <QPushButton>
#include <QSpinBox>
#include <QSlider>
#include <QToolBar>
#include <QAction>
#include <QColorDialog>

// 放大視窗類別：用於顯示選取區域的放大圖片，並提供畫筆和存檔功能
class ZoomWindow : public QMainWindow
{
    Q_OBJECT

public:
    ZoomWindow(const QImage &sourceImage, const QRect &selectedRect, double zoomFactor = 2.0, QWidget *parent = nullptr);
    ~ZoomWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event);  // 事件過濾器

private slots:
    void saveImage();           // 存檔功能
    void choosePenColor();      // 選擇畫筆顏色
    void penWidthChanged(int width);  // 畫筆寬度改變
    void clearDrawing();        // 清除繪圖

private:
    void createActions();       // 建立動作
    void createToolBars();      // 建立工具列
    void drawLineTo(const QPoint &endPoint);  // 繪製線條

    QLabel *imageLabel;         // 顯示圖片的標籤
    QImage originalImage;       // 原始選取區域圖片
    QImage zoomedImage;         // 放大後的圖片
    QImage drawingImage;        // 用於繪圖的圖片
    double zoomFactor;          // 放大倍率
    
    // 畫筆相關
    bool drawing;               // 是否正在繪圖
    QPoint lastPoint;           // 上一個繪圖點
    QColor penColor;            // 畫筆顏色
    int penWidth;               // 畫筆寬度
    
    // UI 元件
    QAction *saveAction;        // 存檔動作
    QAction *penColorAction;    // 畫筆顏色動作
    QAction *clearAction;       // 清除動作
    QSlider *penWidthSlider;    // 畫筆寬度滑桿
    QToolBar *toolBar;          // 工具列
};

#endif // ZOOMWINDOW_H
