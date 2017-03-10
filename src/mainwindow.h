#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QMainWindow>
#   include <QtWidgets/QListWidget>
#else
#   include <QMainWindow>
#   include <QListWidget>
#endif

#include <QDropEvent>
#include <QUrl>
#include <QDrag>
#include "atlasbuilder.h"
#include "atlaspage.h"

extern QStringList imageExtensions;

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    private:
        Ui::MainWindow *ui;

        AtlasBuilder    builder;
        QString topImageDir;
        QList<packedImage> packedImageList;
        bool exporting;
        int recursiveLoaderCounter;
        bool recursiveLoaderDone;
        QPixmap pattern;
        void addDir(QString dir);
        void RecurseDirectory(const QString &dir);

    protected:
        void dropEvent(QDropEvent *event);
        void dragEnterEvent(QDragEnterEvent *event);
    signals:
        void renderedImage(const QList<AtlasPage> &image);
    public slots:
        void addTiles();
        void deleteSelectedTiles();
        void packerUpdate();
        void updateAuto();
        void setTextureSize2048();
        void setTextureSize256();
        void setTextureSize512();
        void setTextureSize1024();
        void updateAplhaThreshold();
        void getFolder();
        void exportImage();
        void swapSizes();
        void clearTiles();
};

#endif // MAINWINDOW_H
