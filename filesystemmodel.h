#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include <QFileSystemModel>
#include "QuaZip/quazip.h"
#include "ZipFileInfo.h"

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    enum Columns
    {
        eName = 0,
        eSize,
        eType,
        eDate,
        eColumnCount
    };

    FileSystemModel(QObject *parent = Q_NULLPTR);
    FileSystemModel(const QString& path, QObject *parent = 0);
    ~FileSystemModel(){if(zip != nullptr) delete zip;}

    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual Qt::DropActions supportedDropActions() const override;
    virtual QStringList mimeTypes() const override;
    virtual QMimeData* mimeData(const QModelIndexList &indexes) const override;
    virtual bool dropMimeData(const QMimeData *data,
                              Qt::DropAction action,
                              int row,
                              int column,
                              const QModelIndex &parent) override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

signals:
    void dropFiles(QStringList, QString);
private:
    void fetchRootDirectory();
private:
    QuaZip* zip = nullptr;
    QList<ZipFileInfo> nodes;//root nodes
};

#endif // FILESYSTEMMODEL_H
