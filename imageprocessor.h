#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QImage>
#include <QLabel>

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

private slots:
    void showOpenFile();

private:
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
};
#endif // IMAGEPROCESSOR_H
