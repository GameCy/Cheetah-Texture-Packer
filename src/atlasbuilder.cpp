#include "atlasbuilder.h"
#include <QDir>

AtlasBuilder::AtlasBuilder()
{

}

void AtlasBuilder::BuildAtlases(bool withImages, QPixmap *pattern)
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
        packerData *inputData = static_cast<packerData *>(inputImg.id);
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
