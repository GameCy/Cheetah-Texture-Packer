#include "atlasimporter.h"
#include <QFile>
#include <QTextStream>

AtlasImporter::AtlasImporter(QString path, bool normalized)
    : PixelWidth(0)
    , PixelHeight(0)
    , normalizedCoords(normalized)
{
    QFile file(path);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        ParseHeader(stream.readLine(200));
        stream.readLine();  // skip empty line

        while (!stream.atEnd())
        {
            ParseLine(stream.readLine(200));
        }
        file.close();
    }

}

bool AtlasImporter::IsValid()
{
    return ( (!FileName.isEmpty())
             && (PixelWidth>0)
             && (PixelHeight>0)
             && (UVRectMap.size()>0) );
}

void AtlasImporter::ParseHeader(QString line)
{
    QStringList items = line.split(",");
    if (items.size()<3)
        return;

    FileName = items[0];
    PixelWidth = items[1].toInt();
    PixelHeight = items[2].toInt();
}

void AtlasImporter::ParseLine(QString line)
{
    QStringList items = line.split(",");
    if (items.size()<5)
        return;
    float u1 = items[1].toFloat();
    float v1 = items[2].toFloat();
    UVRect rect( u1, v1, u1 + items[3].toFloat(), v1 + items[4].toFloat());
    if (normalizedCoords)
        rect.Normalize(float(PixelWidth), float(PixelHeight));
    UVRectMap.insert(items[0], rect);
}
