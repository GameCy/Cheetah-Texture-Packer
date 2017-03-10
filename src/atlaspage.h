#ifndef ATLASPAGE_H
#define ATLASPAGE_H
#include <QString>
#include <QImage>
#include <QPainter>
#include "imagepacker.h"

//struct packerData
//{
//    QString path, file;
//};

class AtlasPage
{
public:
    AtlasPage(int pageID, int width, int height, QPixmap *bgPattern);
    ~AtlasPage();

    void DrawImage(int destX, int destY, const QImage &srcImage, QRect sourceRect) const;
    void DrawRect(int destX, int destY, int width, int height) const;

    bool Save(QString folder, QString fileName, QString format, const ImagePacker &packer, bool includePageId);
    QImage& GetImageBuffer() const;

    int PageID;
private:
    mutable QImage  buffer;
};

#endif // ATLASPAGE_H
