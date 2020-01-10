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
#ifndef ZIPFILEINFO_H
#define ZIPFILEINFO_H

#include "QuaZip/quazip.h"
#include "QuaZip/quazipdir.h"
#include "QuaZip/quazipfile.h"
#include <QIcon>
//#define MEMBERFUNC(MEM) ss
class QIcon;
class QMimeDatabase;
class ZipFileInfo :public QFileInfo
{
public:
    ZipFileInfo(const QuaZipFileInfo64& f, ZipFileInfo* p = nullptr);
    ~ZipFileInfo();
    ZipFileInfo& operator= (const ZipFileInfo& another);
    bool operator== (const ZipFileInfo& another) const;
    //bool isDir() const;
    QString type() const;//file extension

    QString displayName() const;//for display
    int childrenCount() const;
    //QString filePath() const;//full path from archive root

    bool exists() const{return true;}//
    //static bool exists(const QString &file);
    //void refresh();

    QString filePath() const;
    QString absoluteFilePath() const;
    QString canonicalFilePath() const;
    QString fileName() const;
    QString baseName() const;
    QString completeBaseName() const;
    QString suffix() const;
    QString bundleName() const{return "";}
    QString completeSuffix() const;

    QString path() const;
    QString absolutePath() const;
    QString canonicalPath() const;
    QDir dir() const;
    QDir absoluteDir() const;

    bool isReadable() const;
    bool isWritable() const;
    bool isExecutable() const;
    bool isHidden() const;
    bool isNativePath() const;

    bool isRelative() const;
    inline bool isAbsolute() const { return !isRelative(); }
    bool makeAbsolute();

    bool isFile() const;
    bool isDir() const;
    bool isSymLink() const;
    bool isRoot() const;
    bool isBundle() const{return false;}

    //QString readLink() const;
    //inline QString symLinkTarget() const { return readLink(); }

    QString owner() const;
    uint ownerId() const;
    QString group() const;
    uint groupId() const;

    bool permission(QFile::Permissions permissions) const;
    QFile::Permissions permissions() const;

    quint64 size() const;
    quint64 uncompressedSize() const;
    quint64 compressedSize() const;
    QString displaySize() const;//for display
    QString displayUncompressedSize() const;//for display
    QString displayCompressedSize() const;//for display
    QString displaySize(quint64 size) const;//for display


    QDateTime created() const;
    QDateTime lastModified() const;
    QDateTime lastRead() const;

    bool caching() const{return false;}//
    //void setCaching(bool on);




    QIcon icon()const;
    QIcon iconForFilename(const QString &filename);
    bool mapped;//true if we have read children for this node

    QuaZipFileInfo64 fileInfo(){return m_fileInfo;}
    ZipFileInfo* parent(){return m_parent;}
    const ZipFileInfo* parent()const {return m_parent;}
    void setParent(ZipFileInfo* parent){m_parent = parent;}
    QList<ZipFileInfo> &children(){return m_children;}
    const QList<ZipFileInfo> &children()const {return m_children;}
    void refresh();
    inline void clearChildren(){m_children.clear();}
    void SetRoot();
private:
    QuaZipFileInfo64 m_fileInfo;
    ZipFileInfo* m_parent;
    QList<ZipFileInfo> m_children;
    static QMimeDatabase mime_database;
};

#endif // ZIPFILEINFO_H
