#include "commandline.h"
#include <QSettings>
#include <QDir>
#include <QDebug>

void CommandLine::PrintHelp(const char *error)
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

    exit(error?1:0);
}

#define check_opt(opt) (strncmp(argv[i], opt, sizeof(opt) - 1) == 0)

int CommandLine::Run(int argc, char *argv[])
{
    //command-line version
    if(argc < 2)
        return 1;

    AtlasBuilder builder;

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
    int maxFiles = 500;
    int cropThreshold = 1;
    int autosizeThreshold = 80;
    int minTextureSizeX = 32;
    int minTextureSizeY = 32;
    int sortorder = 4;

    QFileInfoList fileList;
    QString outDir = QDir::currentPath();
    QString outFile = "atlas";
    QString baseFolder;
    for(int i = 1; i < argc; ++i)
    {
        if(check_opt("--help") || check_opt("-h") || check_opt("-?"))
        {
            PrintHelp();
            return 0;
        }
        else if(check_opt("-s") || check_opt("--size"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option -s");
            }
            if(sscanf(argv[i], "%10dx%10d", &textureWidth, &textureHeight) != 2)
            {
                if(sscanf(argv[i], "%10d", &textureWidth) != 1)
                {
                    PrintHelp("Wrong texture size format");
                }
                else
                {
                    textureHeight = textureWidth;
                }
            }
            printf("Setting texture size: %dx%d\n", textureWidth, textureHeight);
        }
        else if(check_opt("-o") || check_opt("--out-file"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option -o");
            }
            QFileInfo info(argv[i]);
            outFile = info.baseName();
            outDir = info.absolutePath();
        }
        else if(check_opt("--disable-merge"))        { merge = false; }
        else if(check_opt("--disable-crop"))         { crop = false; }
        else if(check_opt("--disable-recursion"))    { recursion = false; }
        else if(check_opt("--square"))               { square = true; }
        else if(check_opt("--disable-border"))        { border = 0; }
        else if(check_opt("--enable-rotate"))         { rotate = true; }
        else if(check_opt("--autosize-threshold"))
        {
            autosize = true;
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option --autosize-threshold");
            }
            if((sscanf(argv[i], "%10d", &autosizeThreshold) != 1) ||
                    (autosizeThreshold < 0) ||
                    (autosizeThreshold > 100))
            {
                PrintHelp("Wrong autosize threshold");
            }
        }
        else if(check_opt("--extrude-size"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option --extrude-size");
            }
            if((sscanf(argv[i], "%10d", &extrude) != 1) || (extrude < 0))
            {
                PrintHelp("Wrong extrude size");
            }
        }
        else if(check_opt("--border-size"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option --border-size");
            }
            if((sscanf(argv[i], "%10d", &border) != 1) || (border < 0))
            {
                PrintHelp("Wrong border size");
            }
        }
        else if(check_opt("--min-texture-size"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option -min-texture-size");
            }
            if(sscanf(argv[i], "%10dx%10d", &minTextureSizeX, &minTextureSizeY) != 2)
            {
                if(sscanf(argv[i], "%10d", &minTextureSizeX) != 1)
                {
                    PrintHelp("Wrong texture size format");
                }
                else
                {
                    minTextureSizeY = minTextureSizeX;
                }
            }
        }
        else if(check_opt("--crop-threshold"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option --crop-threshold");
            }
            if((sscanf(argv[i], "%10d", &cropThreshold) != 1) ||
                    (cropThreshold < 0) ||
                    (cropThreshold > 255))
            {
                PrintHelp("Wrong crop threshold");
            }
        }
        else if(check_opt("--sort-order"))
        {
            ++i;
            if(i >= argc)
            {
                PrintHelp("Argument needed for option --sort-order");
            }
            if((sscanf(argv[i], "%10d", &sortorder) != 1) ||
                    (sortorder < 0) ||
                    (sortorder > 4))
            {
                PrintHelp("Wrong sortorder must be from 0 to 4");
            }
        }
        else //dir or file
        {
            QFileInfo file(argv[i]);
            if(file.isFile())
            {
                fileList << file;
            }
            else if(file.isDir())
            {
                baseFolder = file.absoluteFilePath();
                fileList += builder.RecurseDirectory(file.absoluteFilePath(), recursion, maxFiles);
            }
        }
    }

    qDebug() << "Saving to dir" << outDir << "and file" << outFile;
    ImagePacker packer;
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

    builder.UpdatePacker(heuristic, textureWidth, textureHeight);
    builder.SaveFiles(outDir, outFile, outFormat);

    float percent = (((float)packer.area / (float)builder.TotalArea) * 100.0f);
    //        float percent2 = (float)(((float)packer.neededArea / (float)area) * 100.0f );
    printf("Atlas generated. %f%% filled, %d images missed, %d merged, %d KB\n",
           percent, packer.missingImages, packer.mergedImages, (int)((builder.TotalArea * 4) / 1024));

    return 0;
}
