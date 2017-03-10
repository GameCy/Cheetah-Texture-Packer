#ifndef ATLASIMPORTER_H
#define ATLASIMPORTER_H
#include <QString>
#include <QMap>

struct UVRect
{
    UVRect(float u1, float v1, float u2, float v2)
    { U1=u1; V1=v1; U2=u2; V2=v2; }

    void Normalize(float nx, float ny)
    {
        U1 /= nx;   U2/=nx;
        V1/=ny;     V2/=ny;
    }

    float U1,V1,U2,V2;
};

class AtlasImporter
{
public:
    AtlasImporter(QString path, bool normalized);
    bool IsValid();

    QString FileName;
    QMap<QString,UVRect>  UVRectMap;

    int PixelWidth;
    int PixelHeight;

private:
    void ParseHeader(QString line);
    void ParseLine(QString line);

    bool normalizedCoords;
};

#endif // ATLASIMPORTER_H
