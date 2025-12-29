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

private slots:
    void showOpenFile();
    void showGeometryTransform();

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
};
#endif // IMAGEPROCESSOR_H
