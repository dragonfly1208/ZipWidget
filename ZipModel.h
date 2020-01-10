/* ZipModel using Qt5 and QuaZip.
* Copyright (C) 2014 Alexey Lebedev alexey.broadcast@gmail.com
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/
#ifndef ZIPMODEL_H
#define ZIPMODEL_H

#include <QAbstractItemModel>
#include "QuaZip/quazip.h"
#include "ZipFileInfo.h"

class ZipModel : public QAbstractItemModel
{
	Q_OBJECT
public:
    explicit ZipModel(QObject *parent = 0);
    ZipModel(const QString& path, QObject *parent = 0);
    ~ZipModel();

	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	bool canFetchMore(const QModelIndex &parent) const;
	void fetchMore(const QModelIndex &parent);
	Qt::ItemFlags flags(const QModelIndex &index) const;
    bool hasChildren(const QModelIndex &parent) const;
    inline bool isOpen(){return zip&& zip->isOpen();}
    bool isValid();
    inline ZipFileInfo* FileInfo(const QModelIndex& index) const
    {
        return static_cast<ZipFileInfo*>(index.internalPointer());
    }
    inline QString zipName(){return zip?zip->getZipName():"";}
    bool addFileToCurentZip(const QString &fPath,const QModelIndex &dirIndex = QModelIndex());
    bool addFilesToCurentZipDir(QStringList fileNames,const QModelIndex &dirIndex = QModelIndex());
    bool delSelectedFiles(QModelIndexList indexs);
    bool extractSelectedFiles(QModelIndexList indexs,QString destDir);
    enum Columns
    {
        RamificationColumn,
        NameColumn = RamificationColumn,
        TypeColumn,
        SizeColumn,
        CompressedSizeColumn,
        ModificationDateColumn,
        ColumnCount
    };
private:

private:


    QSharedPointer<QuaZip> zip;
    QList<ZipFileInfo> nodes;//root nodes
    ZipFileInfo rootInfo;

	void fetchRootDirectory();
    int findRow(const ZipFileInfo* nodeInfo) const;//get row of "nodeInfo"
};

#endif // ZIPMODEL_H
