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
#include "atlaspage.h"
#include "atlasimporter.h"

QStringList imageExtensions;

void printHelp(const char *error = NULL)
{
    if(error)
    {
        fputs(error, stderr);
    }
    printf(
        "Usage: cheetah-texture-packer [-s size] [-o OUTFILE] [options] [file|dir ...]\n"
        "Avaiable options:\n"
        "  -s, --size W[xH]           atlas maximum size (if it is not enough - create\n"
        "                             more than 1 atlas)\n"
        "  -o, --out-file OUTFILE     output atlas name\n"
        "      --disable-merge        do not merge similar images\n"
        "      --disable-crop         do not crop images\n"
        "      --crop-threshold N     crop threshold (0-255)\n"
        "      --disable-border       do not make 1px border\n"
        "      --border-size          set border size in pixels\n"
        "      --extrude-size         set extrude size in pixels\n"
        "      --enable-rotate        enable sprites rotation\n"
        "      --disable-recursion    disable recursive scan (pack only given directory)\n"
        "      --square               force to make square textures\n"
        "      --autosize-threshold N auto-optimize atlas size (0-100, 0 - disabled)\n"
        "      --min-texture-size WxH auto-optimize minimum size\n"
        "      --sort-order N         select sorting order algorithm (0-4)\n"
        "  -h, -?, --help             show this help and exit\n");
    if(error)
    {
        exit(1);
    }
    else
    {
        exit(0);
    }
}

#define check_opt(opt) (strncmp(argv[i], opt, sizeof(opt) - 1) == 0)


int main(int argc, char *argv[])
{
/*
    QString fname= "/home/mastros/Desktop/projects/Game/assets/textures/textures.atlas";
    AtlasImporter atlas(fname, true);

    bool kkk = true;
    kkk = atlas.IsValid();
if (!kkk)
    return 1;
*/

    QApplication a(argc, argv);
    imageExtensions << "bmp" << "png" << "jpg" << "jpeg";
/*
    //command-line version
    if(argc > 1)
    {
        int textureWidth = 512;
        int textureHeight = 512;
        bool merge = true;
        bool crop = true;
        int border = 1;
        int extrude = 0;
        bool rotate = false;
        bool recursion = true;
        bool square = false;
        bool autosize = false;
        int cropThreshold = 1;
        int autosizeThreshold = 80;
        int minTextureSizeX = 32;
        int minTextureSizeY = 32;
        int sortorder = 4;

        ImagePacker packer;
        mainPacker = &packer;
        QString outDir = QDir::currentPath();
        QString outFile = "atlas";
        for(int i = 1; i < argc; ++i)
        {
            if(check_opt("--help") || check_opt("-h") || check_opt("-?"))
            {
                printHelp();
            }
            else
                if(check_opt("-s") || check_opt("--size"))
                {
                    ++i;
                    if(i >= argc)
                    {
                        printHelp("Argument needed for option -s");
                    }
                    if(sscanf(argv[i], "%10dx%10d", &textureWidth, &textureHeight) != 2)
                    {
                        if(sscanf(argv[i], "%10d", &textureWidth) != 1)
                        {
                            printHelp("Wrong texture size format");
                        }
                        else
                        {
                            textureHeight = textureWidth;
                        }
                    }
                    printf("Setting texture size: %dx%d\n", textureWidth, textureHeight);
                }
                else
                    if(check_opt("-o") || check_opt("--out-file"))
                    {
                        ++i;
                        if(i >= argc)
                        {
                            printHelp("Argument needed for option -o");
                        }
                        QFileInfo info(argv[i]);
                        outFile = info.baseName();
                        outDir = info.absolutePath();
                    }
                    else
                        if(check_opt("--disable-merge"))
                        {
                            merge = false;
                        }
                        else
                            if(check_opt("--disable-crop"))
                            {
                                crop = false;
                            }
                            else
                                if(check_opt("--disable-recursion"))
                                {
                                    recursion = false;
                                }
                                else
                                    if(check_opt("--square"))
                                    {
                                        square = true;
                                    }
                                    else
                                        if(check_opt("--autosize-threshold"))
                                        {
                                            autosize = true;
                                            ++i;
                                            if(i >= argc)
                                            {
                                                printHelp("Argument needed for option --autosize-threshold");
                                            }
                                            if((sscanf(argv[i], "%10d", &autosizeThreshold) != 1) ||
                                                    (autosizeThreshold < 0) ||
                                                    (autosizeThreshold > 100))
                                            {
                                                printHelp("Wrong autosize threshold");
                                            }
                                        }
                                        else
                                            if(check_opt("--extrude-size"))
                                            {
                                                ++i;
                                                if(i >= argc)
                                                {
                                                    printHelp("Argument needed for option --extrude-size");
                                                }
                                                if((sscanf(argv[i], "%10d", &extrude) != 1) || (extrude < 0))
                                                {
                                                    printHelp("Wrong extrude size");
                                                }
                                            }
                                            else
                                                if(check_opt("--border-size"))
                                                {
                                                    ++i;
                                                    if(i >= argc)
                                                    {
                                                        printHelp("Argument needed for option --border-size");
                                                    }
                                                    if((sscanf(argv[i], "%10d", &border) != 1) || (border < 0))
                                                    {
                                                        printHelp("Wrong border size");
                                                    }
                                                }
                                                else
                                                    if(check_opt("--min-texture-size"))
                                                    {
                                                        ++i;
                                                        if(i >= argc)
                                                        {
                                                            printHelp("Argument needed for option -min-texture-size");
                                                        }
                                                        if(sscanf(argv[i], "%10dx%10d", &minTextureSizeX, &minTextureSizeY) != 2)
                                                        {
                                                            if(sscanf(argv[i], "%10d", &minTextureSizeX) != 1)
                                                            {
                                                                printHelp("Wrong texture size format");
                                                            }
                                                            else
                                                            {
                                                                minTextureSizeY = minTextureSizeX;
                                                            }
                                                        }
                                                    }
                                                    else
                                                        if(check_opt("--crop-threshold"))
                                                        {
                                                            ++i;
                                                            if(i >= argc)
                                                            {
                                                                printHelp("Argument needed for option --crop-threshold");
                                                            }
                                                            if((sscanf(argv[i], "%10d", &cropThreshold) != 1) ||
                                                                    (cropThreshold < 0) ||
                                                                    (cropThreshold > 255))
                                                            {
                                                                printHelp("Wrong crop threshold");
                                                            }
                                                        }
                                                        else
                                                            if(check_opt("--sort-order"))
                                                            {
                                                                ++i;
                                                                if(i >= argc)
                                                                {
                                                                    printHelp("Argument needed for option --sort-order");
                                                                }
                                                                if((sscanf(argv[i], "%10d", &sortorder) != 1) ||
                                                                        (sortorder < 0) ||
                                                                        (sortorder > 4))
                                                                {
                                                                    printHelp("Wrong sortorder must be from 0 to 4");
                                                                }
                                                            }
                                                            else
                                                                if(check_opt("--disable-border"))
                                                                {
                                                                    border = 0;
                                                                }
                                                                else
                                                                    if(check_opt("--enable-rotate"))
                                                                    {
                                                                        rotate = true;
                                                                    }
                                                                    //dir or file
                                                                    else
                                                                    {
                                                                        QFileInfo file(argv[i]);
                                                                        if(file.isFile())
                                                                        {
                                                                            packerData *data = new packerData;
                                                                            data->path = file.absoluteFilePath();
                                                                            data->file = file.fileName();
                                                                            packer.addItem(data->path, data);
                                                                        }
                                                                        else
                                                                            if(file.isDir())
                                                                            {
                                                                                topImageDir = file.absoluteFilePath();
                                                                                RecurseDirectory(file.absoluteFilePath(), recursion);
                                                                            }
                                                                    }
        }

        qDebug() << "Saving to dir" << outDir << "and file" << outFile;
        packer.sortOrder = sortorder;
        packer.border.t = 0;
        packer.border.l = 0;
        packer.border.r = border;
        packer.border.b = border;
        packer.extrude = extrude;
        packer.cropThreshold = crop ? cropThreshold : 0;
        packer.minFillRate = autosize ? autosizeThreshold : 0;
        packer.minTextureSizeX = minTextureSizeX;
        packer.minTextureSizeY = minTextureSizeY;
        packer.merge = merge;
        packer.mergeBF = false;
        packer.rotate = rotate;
        packer.square = square;
        packer.autosize = autosize;
        int heuristic = 1;

        QString outFormat("PNG");

        if(packer.images.size() == 0)
        {
            fprintf(stderr, "No images found, exitting\n");
            exit(1);
        }

        AtlasBuilder builder;
        // setup
        //  build
        // save
        packer.pack(heuristic, textureWidth, textureHeight);
        builder.Save(... .. ..  .)

        qint64 area = 0;
        for(int i = 0; i < textures.count(); i++)
        {
            area += textures.at(i).width() * textures.at(i).height();
        }
        float percent = (((float)packer.area / (float)area) * 100.0f);
        //        float percent2 = (float)(((float)packer.neededArea / (float)area) * 100.0f );
        printf("Atlas generated. %f%% filled, %d images missed, %d merged, %d KB\n",
               percent, packer.missingImages, packer.mergedImages, (int)((area * 4) / 1024));


        return 0;
    }
*/
    QTranslator myTranslator;
    myTranslator.load("tile_" + QLocale::system().name(), "qm");
    a.installTranslator(&myTranslator);
    MainWindow w;
    w.show();


    return a.exec();
}
