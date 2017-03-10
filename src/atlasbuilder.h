#ifndef ATLASBUILDER_H
#define ATLASBUILDER_H
#include <QList>
#include <QPixmap>
#include "atlaspage.h"
#include "imagepacker.h"

struct packerData
{
    void* listItem; // QListWidgetItem
    QString path;
    QString filename;
};

class AtlasBuilder
{
public:
    AtlasBuilder();

    void BuildAtlases(bool withImages, QPixmap *pattern);
    bool SaveFiles(QString outDir, QString outFile, QString outFormat) const;

    ImagePacker packer;
    float   TotalArea;

    QList<AtlasPage> atlasPages;
private:
};

#endif // ATLASBUILDER_H
