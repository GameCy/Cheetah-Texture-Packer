#include <QtGlobal>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#   include <QtWidgets/QApplication>
#else
#   include <QtGui/QApplication>
#endif

#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>
#include <QDir>
#include <QDebug>
#include <QPainter>
#include "stdio.h"
#include "stdlib.h"
#include "atlasbuilder.h"
#include "commandline.h"
#include "atlasimporter.h"
#include <QStandardPaths>


int main(int argc, char *argv[])
{
    AtlasBuilder::ImageExtensions << "bmp" << "png" << "jpg" << "jpeg";
    if (argc>1)
    {
        return CommandLine::Run(argc, argv);
    }
/*
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fname= folder+"/projects/Game/assets/textures/textures.atlas";
    AtlasImporter atlas(fname, true);

    bool kkk = true;
    kkk = atlas.IsValid();
if (!kkk)
    return 1;
*/

    QApplication a(argc, argv);
    QTranslator myTranslator;
    myTranslator.load("tile_" + QLocale::system().name(), "qm");
    a.installTranslator(&myTranslator);
    MainWindow w;
    w.show();

    return a.exec();
}
