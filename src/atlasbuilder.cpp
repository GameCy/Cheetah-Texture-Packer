#include "atlasbuilder.h"
#include <QDir>

QStringList AtlasBuilder::ImageExtensions;

AtlasBuilder::AtlasBuilder()
{
}

QFileInfoList AtlasBuilder::RecurseDirectory(const QString &dir, bool recurse, int maxFiles)
{
    QFileInfoList fileList;
    QDir dirEnt(dir);
    QFileInfoList list = dirEnt.entryInfoList();
    for(int i = 0; i < list.count() && (maxFiles>=0); i++)
    {
        QFileInfo info = list[i];

        QString filePath = info.filePath();
        QString fileExt = info.suffix().toLower();
        QString name = dir + QDir::separator();
        if(info.isDir())
        {
            // recursive
            if(recurse && info.fileName() != ".." && info.fileName() != ".")
            {
                fileList += RecurseDirectory(filePath, recurse, maxFiles);
            }
        }
        else if(ImageExtensions.contains(fileExt))
        {
            if(!QFile::exists(name + info.completeBaseName() + QString(".atlas")))
            {
                maxFiles--;
                fileList << info;
            }
        }
    }
    return fileList;
}

void AtlasBuilder::AddFiles(QFileInfoList &fileList, QString topImageDir)
{
    foreach(QFileInfo info, fileList)
    {
        packerData *data = new packerData;
        data->path = info.absoluteFilePath();
        data->filename = info.filePath().replace(topImageDir, "");
        data->listItem = 0;
        packer.addItem(data->path, data);
    }
}

void AtlasBuilder::SetBorders(int top, int bottom, int left, int right)
{
    packer.border.t = bottom;
    packer.border.b = top;
    packer.border.l = left;
    packer.border.r = right;
}

void AtlasBuilder::SetParams(int cropThreshold, int extrude, int rotate, bool merge, bool square, bool autosize)
{
    packer.cropThreshold = cropThreshold;
    packer.extrude = extrude;
    packer.rotate = rotate;
    packer.merge = merge;
    packer.square = square;
    packer.autosize = autosize;
}

void AtlasBuilder::UpdatePacker(int heuristic, int maxTexWidth, int maxTexHeight)
{
    packer.pack(heuristic, maxTexWidth, maxTexHeight);
}

void AtlasBuilder::RenderAtlases(bool withImages, QPixmap *pattern)
{
    // create empty pages
    TotalArea = 0;
    atlasPages.clear();
    for(int i = 0; i < packer.bins.size(); i++)
    {
        int w = packer.bins.at(i).width();
        int h = packer.bins.at(i).height();
        atlasPages << AtlasPage(i, w,h, pattern);
        TotalArea += w*h;
    }

    // pre-process input images and build atlases
    int extrude = packer.extrude;
    for(int i = 0; i < packer.images.size(); i++)
    {
        const inputImage &inputImg = packer.images.at(i);
        packerData *inputData = static_cast<packerData *>(inputImg.externalData);
        if ( (inputImg.duplicateId != NULL && packer.merge) ||
             (inputImg.pos == QPoint(999999, 999999)) )
        {
            continue;
        }

        QSize size = inputImg.size;
        QRect crop = QRect(0, 0, size.width(), size.height());
        if(packer.cropThreshold)
        {
            size = inputImg.crop.size();
            crop = inputImg.crop;
        }

        // load image
        QImage img(inputData->path);

        if(inputImg.rotated) // rotate
        {
            QTransform myTransform;
            myTransform.rotate(90);
            img = img.transformed(myTransform);
            size.transpose();
            crop = QRect(inputImg.size.height() - crop.y() - crop.height(),
                         crop.x(), crop.height(), crop.width());
        }

        // paint image if belongs to valid atlas
        if(inputImg.textureId < atlasPages.size())
        {
            float px = inputImg.pos.x() + packer.border.l;
            float py = inputImg.pos.y() + packer.border.t;
            const AtlasPage &page = atlasPages.at(inputImg.textureId);

            if(withImages)  page.DrawImage(px + extrude, py + extrude, img, crop);
            else            page.DrawRect(px, py, size.width() - 1, size.height() - 1);
        }
    }
}

bool AtlasBuilder::SaveFiles(QString outDir, QString outFile, QString outFormat) const
{
    bool manyPages = (atlasPages.count()>1);

    for(int i=0; i<atlasPages.size(); ++i)
    {
        AtlasPage page = atlasPages.at(i);
        // save atlas
        if (!(page.Save(outDir, outFile,outFormat, packer, manyPages)))
        {
            return false;
        }
    }
    return true;
}
