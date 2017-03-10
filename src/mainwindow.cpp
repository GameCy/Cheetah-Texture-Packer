#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QMimeData>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    exporting = false;
    ui->setupUi(this);
    connect(this, SIGNAL(renderedImage(QList<AtlasPage>)), ui->widget,
            SLOT(updatePixmap(QList<AtlasPage>)));
    ui->outDir->setText(QDir::homePath());
    exporting = false;
    ui->widget->scaleBox = ui->scale;
    tabifyDockWidget(ui->dockPreferences, ui->dockExport);
    ui->dockPreferences->raise();

    pattern = QPixmap(20, 20);
    QPainter painter(&pattern);
#define BRIGHT 190
#define SHADOW 150
    painter.fillRect(0, 0, 10, 10, QColor(SHADOW, SHADOW, SHADOW));
    painter.fillRect(10, 0, 10, 10, QColor(BRIGHT, BRIGHT, BRIGHT));
    painter.fillRect(10, 10, 10, 10, QColor(SHADOW, SHADOW, SHADOW));
    painter.fillRect(0, 10, 10, 10, QColor(BRIGHT, BRIGHT, BRIGHT));
    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::RecurseDirectory(const QString &dir)
{
    QDir dirEnt(dir);
    QFileInfoList list = dirEnt.entryInfoList();
    for(int i = 0; i < list.count() && !recursiveLoaderDone; i++)
    {
        recursiveLoaderCounter++;
        QFileInfo info = list[i];

        QString filePath = info.filePath();
        QString fileExt = info.suffix().toLower();
        QString name = dir + QDir::separator();
        if(info.isDir())
        {
            // recursive
            if(info.fileName() != ".." && info.fileName() != ".")
            {
                RecurseDirectory(filePath);
            }
        }
        else
            if(imageExtensions.contains(fileExt))
            {
                if(!QFile::exists(name + info.completeBaseName() + QString(".atlas")))
                {
                    ui->tilesList->addItem(filePath.replace(topImageDir, ""));
                    packerData *data = new packerData;
                    data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
                    data->path = info.absoluteFilePath();
                    builder.packer.addItem(data->path, data);
                }
            }
        if(recursiveLoaderCounter == 500)
        {
            if(QMessageBox::No ==
                    QMessageBox::question(
                        this,
                        tr("Directory is too big"),
                        tr("It seems that directory <b>") + topImageDir +
                        tr("</b> is too big. "
                           "Loading may take HUGE amount of time and memory. "
                           "Please, check directory again. <br>"
                           "Do you want to continue?"),
                        QMessageBox::Yes,
                        QMessageBox::No))
            {
                recursiveLoaderDone = true;
                recursiveLoaderCounter++;
                continue;
            }
            ui->previewWithImages->setChecked(false);
        }
    }
}

void MainWindow::addDir(QString dir)
{
    //FIXME
    //this is messy hack due to difference between QFileDialog and QFileInfo dir separator in Windows
    if(QDir::separator() == '\\')
    {
        topImageDir = dir.replace("\\", "/") + "/";
    }
    else
    {
        topImageDir = dir + "/";
    }
    ui->outDir->setText(dir);
    recursiveLoaderCounter = 0;
    recursiveLoaderDone = false;
    //packer.clear();
    RecurseDirectory(dir);
    QFileInfo info(dir);
    ui->outFile->setText(info.baseName());

}

void MainWindow::addTiles()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                  tr("Select tile directory"), topImageDir);
    if(dir.length() > 0)
    {
        addDir(dir);
        packerUpdate();
    }
}

void MainWindow::deleteSelectedTiles()
{
    QList<QListWidgetItem *> itemList = ui->tilesList->selectedItems();
    for(int i = 0; i < itemList.size(); i++)
    {
        for(int j = 0; j < builder.packer.images.size(); ++j)
        {
            if((static_cast<packerData *>(builder.packer.images.at(j).id))->listItem == itemList[i])
            {
                delete(static_cast<packerData *>(builder.packer.images.at(j).id));
                builder.packer.images.removeAt(j);
            }
        }
    }
    qDeleteAll(ui->tilesList->selectedItems());
}

void MainWindow::packerUpdate()
{
    quint64 area = 0;
    ImagePacker &packer = builder.packer;
    packer.sortOrder = ui->sortOrder->currentIndex();
    packer.border.t = ui->borderTop->value();
    packer.border.l = ui->borderLeft->value();
    packer.border.r = ui->borderRight->value();
    packer.border.b = ui->borderBottom->value();
    packer.extrude = ui->extrude->value();
    packer.merge = ui->merge->isChecked();
    packer.square = ui->square->isChecked();
    packer.autosize = ui->autosize->isChecked();
    packer.minFillRate = ui->minFillRate->value();
    packer.mergeBF = false;
    packer.rotate = ui->rotationStrategy->currentIndex();

    int textureWidth = ui->textureW->value();
    int textureHeight = ui->textureH->value();
    int heuristic = ui->comboHeuristic->currentIndex();

    packer.pack(heuristic, textureWidth, textureHeight);

    bool previewWithImages = ui->previewWithImages->isChecked();
    QString outDir = ui->outDir->text();
    QString outFile = ui->outFile->text();
    QString outFormat = ui->outFormat->currentText();


    //if (exporting || previewWithImages)
        builder.BuildAtlases(previewWithImages, &pattern);

    // save files
    if(exporting)
    {
        if (!builder.SaveFiles(outDir, outFile, outFormat))
            QMessageBox::critical(0, tr("Error"), tr("Cannot create file ") + outDir+"/"+outFile);
        else
            QMessageBox::information(0, tr("Done"), tr("Your atlas successfully saved in ") + outDir);
    }

    // update color on labels
    for(int i = 0; i < packer.images.size(); i++)
    {
        const inputImage &inputImg = packer.images.at(i);
        Qt::GlobalColor labelColor = Qt::black;
        if(inputImg.pos == QPoint(999999, 999999))
            labelColor = Qt::red;

        packerData *inputData = static_cast<packerData *>(inputImg.id);
        ((QListWidgetItem*)inputData->listItem)->setForeground(labelColor);
    }


    // calculate total area

    float percent = (((float)packer.area / (float)area) * 100.0f);
    float percent2 = (float)(((float)packer.neededArea / (float)builder.TotalArea) * 100.0f);
    ui->preview->setText(tr("Preview: ") +
                         QString::number(percent) + QString("% filled, ") +
                         (packer.missingImages == 0 ? QString::number(packer.missingImages) +
                          tr(" images missed,") :
                          QString("<font color=red><b>") + QString::number(packer.missingImages) +
                          tr(" images missed,") + "</b></font>") +
                         " " + QString::number(packer.mergedImages) + tr(" images merged, needed area: ")
                         +
                         QString::number(percent2) + "%." + tr(" KBytes: ") + QString::number(builder.TotalArea * 4 / 1024));

    if (!exporting)
    {
        emit renderedImage(builder.atlasPages);
    }
    exporting = false;
}

void MainWindow::setTextureSize2048()
{
    ui->textureW->setValue(2048);
    ui->textureH->setValue(2048);
}

void MainWindow::setTextureSize256()
{
    ui->textureW->setValue(256);
    ui->textureH->setValue(256);
}

void MainWindow::setTextureSize512()
{
    ui->textureW->setValue(512);
    ui->textureH->setValue(512);
}

void MainWindow::setTextureSize1024()
{
    ui->textureW->setValue(1024);
    ui->textureH->setValue(1024);
}
void MainWindow::updateAplhaThreshold()
{
    builder.packer.cropThreshold = ui->alphaThreshold->value();
    builder.packer.UpdateCrop();
    updateAuto();
}

void MainWindow::getFolder()
{
    ui->outDir->setText(QFileDialog::getExistingDirectory(this,
                        tr("Open Directory"),
                        ui->outDir->text(),
                        QFileDialog::ShowDirsOnly));
}

void MainWindow::exportImage()
{
    exporting = true;
    packerUpdate();
}

void MainWindow::swapSizes()
{
    int buf = ui->textureW->value();
    ui->textureW->setValue(ui->textureH->value());
    ui->textureH->setValue(buf);
}

void MainWindow::clearTiles()
{
    builder.packer.images.clear();
    ui->tilesList->clear();
}

void MainWindow::updateAuto()
{
    if(ui->autoUpdate->isChecked())
    {
        packerUpdate();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> droppedUrls = event->mimeData()->urls();
    int droppedUrlCnt = droppedUrls.size();
    for(int i = 0; i < droppedUrlCnt; i++)
    {
        QString localPath = droppedUrls[i].toLocalFile();
        QFileInfo fileInfo(localPath);
        if(fileInfo.isFile())
        {
            ui->tilesList->addItem(fileInfo.fileName());
            packerData *data = new packerData;
            data->listItem = ui->tilesList->item(ui->tilesList->count() - 1);
            data->path = fileInfo.absoluteFilePath();
            builder.packer.addItem(data->path, data);
            //QMessageBox::information(this, tr("Dropped file"), "Dropping files is not supported yet. Drad and drop directory here.");
        }
        else
            if(fileInfo.isDir())
            {
                addDir(fileInfo.absoluteFilePath());
            }
    }
    packerUpdate();

    event->acceptProposedAction();
}
