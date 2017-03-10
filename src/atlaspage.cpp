#include "atlaspage.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QUrl>

AtlasPage::AtlasPage(int pageID, int width, int height, QPixmap *bgPattern)
    : PageID(pageID)
    , buffer(width, height, QImage::Format_ARGB32)
{
    if (bgPattern)
    {
        QPainter p(&buffer);
        p.fillRect(QRect(0, 0, width, height), *bgPattern);
    }
    else
        buffer.fill(Qt::transparent);
}

AtlasPage::~AtlasPage()
{
}

void AtlasPage::DrawImage(int destX, int destY, const QImage &srcImage, QRect sourceRect) const
{
    QPainter p(&buffer);
    p.drawImage(destX, destY, srcImage, sourceRect.x(), sourceRect.y(), sourceRect.width(), sourceRect.height());
}

void AtlasPage::DrawRect(int destX, int destY, int width, int height) const
{
    QPainter p(&buffer);
    p.drawRect(destX, destY, width, height);
}

bool AtlasPage::Save(QString folder, QString fileName, QString format, const ImagePacker &packer, bool includePageId)
{
    QString atlasPath = folder + QDir::separator() + fileName;
    if(includePageId)
    {
        atlasPath += QString("_") + QString::number(PageID + 1);
    }
    QString imagePath = atlasPath + "." + format.toLower();
    atlasPath += ".atlas";
    QString imgFile = fileName;
    if(includePageId)
    {
        imgFile += QString("_") + QString::number(PageID + 1);
    }
    imgFile += ".";
    imgFile += format.toLower();

    QFile atlasFile(atlasPath);
    if(!atlasFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    int extrude = packer.extrude;
    QTextStream out(&atlasFile);
    QString seperator=",";
    out << imgFile << seperator << buffer.width() << seperator << buffer.height() << "\n\n";

    for(int i = 0; i < packer.images.size(); i++)
    {
        const inputImage &inputImg = packer.images.at(i);
        if(inputImg.textureId != PageID)
        {
            continue;
        }
        QPoint pos(inputImg.pos.x() + packer.border.l + extrude,
                   inputImg.pos.y() + packer.border.t + extrude);
        QSize size, sizeOrig;
        QRect crop;
        sizeOrig = inputImg.size;
        if(!packer.cropThreshold)
        {
            size = inputImg.size;
            crop = QRect(0, 0, size.width(), size.height());
        }
        else
        {
            size = inputImg.crop.size();
            crop = inputImg.crop;
        }
        if(inputImg.rotated)
        {
            size.transpose();
            crop = QRect(crop.y(), crop.x(), crop.height(), crop.width());
        }

        QUrl url(inputImg.path);
        out << url.fileName() << seperator <<
                pos.x() << seperator <<
                pos.y() << seperator <<
                crop.width() << seperator <<
                crop.height() << seperator <<
                crop.x() << seperator <<
                crop.y() << seperator <<
                sizeOrig.width() << seperator <<
                sizeOrig.height() << seperator <<
                (packer.images.at(i).rotated ? "r" : "") << "\n";

    }
    atlasFile.close();

    // save texture
    const char *fmt = qPrintable(format);
    if(format == "JPG")      buffer.save(imagePath, fmt, 100);
    else                     buffer.save(imagePath);

    return true;
}

QImage& AtlasPage::GetImageBuffer() const
{
    return buffer;
}


