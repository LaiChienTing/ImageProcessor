#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>
#include <QMouseEvent>
#include <QStatusBar>
#include "imagetransform.h"

// 前置宣告，避免循環包含
class ZoomWindow;

class ImageProcessor : public QMainWindow
{
    Q_OBJECT

public:
    ImageProcessor(QWidget *parent = nullptr);
    ~ImageProcessor();
    void createActions();
    void createMenus();
    void createToolBars();
    void loadFile(QString filename);
    void loadImage(const QImage &image);

protected:
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);  // 繪製選取框

private slots:
    void showOpenFile();
    void showGeometryTransform();
    void openZoomWindow();  // 開啟放大視窗

private:
    ImageTransform *gWin;
    QWidget   *central;
    QMenu     *fileMenu;
    QToolBar  *fileTool;
    QImage    img;
    QString   filename;
    QLabel    *imgWin;
    QAction   *openFileAction;
    QAction   *exitAction;
    QAction   *zoomInAction;
    QAction   *zoomOutAction;
    double scaleFactor = 1.0;
    QAction   *geometryAction;
    QLabel    *statusLabel;
    QLabel    *MousePosLabel;
    
    // 區域選取相關變數
    bool isSelecting;           // 是否正在選取區域
    QPoint selectionStart;      // 選取起始點
    QPoint selectionEnd;        // 選取結束點
    QRect selectionRect;        // 選取的矩形區域
};
#endif // IMAGEPROCESSOR_H
