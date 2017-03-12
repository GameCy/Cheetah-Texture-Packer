#ifndef ATLASBUILDER_H
#define ATLASBUILDER_H
#include <QList>
#include <QPixmap>
#include <QFileInfo>
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

    static QFileInfoList RecurseDirectory(const QString &dir, bool recurse, int maxFiles);
    void AddFiles(QFileInfoList &fileList, QString topImageDir);

    void UpdatePacker(int heuristic, int maxTexWidth, int maxTexHeight);
    void RenderAtlases(bool withImages, QPixmap *pattern);
    bool SaveFiles(QString outDir, QString outFile, QString outFormat) const;

    ImagePacker packer;
    float   TotalArea;

    QList<AtlasPage> atlasPages;

    static QStringList ImageExtensions;
};

#endif // ATLASBUILDER_H
