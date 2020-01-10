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

#include "mainwindow.h"
#include <QApplication>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QDateTime>

#define DEBUGINFO(FUNC) qDebug()<<#FUNC ":"<<file.FUNC();
void test();
int main(int argc, char *argv[])
{
    //test();
    QApplication a(argc, argv);
//    Widget w;
//    w.show();
    MainWindow m;
    m.show();

    return a.exec();
}

void test(){

    QFileInfo file("F:/C++/git/QtZipModel-master/bin/quazip.dll");

    DEBUGINFO(exists)
    DEBUGINFO(filePath)
    DEBUGINFO(absoluteFilePath)
    DEBUGINFO(canonicalFilePath)
    DEBUGINFO(fileName)
    DEBUGINFO(baseName)
    DEBUGINFO(completeBaseName)
    DEBUGINFO(suffix)
    DEBUGINFO(bundleName)
    DEBUGINFO(completeSuffix)
    DEBUGINFO(path)
    DEBUGINFO(absolutePath)
    DEBUGINFO(canonicalPath)
    DEBUGINFO(dir)
    DEBUGINFO(absoluteDir)
    DEBUGINFO(isReadable)
    DEBUGINFO(isWritable)
    DEBUGINFO(isExecutable)
    DEBUGINFO(isHidden)
    DEBUGINFO(isNativePath)
    DEBUGINFO(isRelative)
    DEBUGINFO(isAbsolute)
    DEBUGINFO(makeAbsolute)
    DEBUGINFO(isFile)
    DEBUGINFO(isDir)
    DEBUGINFO(isSymLink)
    DEBUGINFO(isRoot)
    DEBUGINFO(isBundle)
    DEBUGINFO(readLink)
    DEBUGINFO(symLinkTarget)
    DEBUGINFO(owner)
    DEBUGINFO(ownerId)
    DEBUGINFO(group)
    DEBUGINFO(groupId)
    DEBUGINFO(size)
    DEBUGINFO(created)
    DEBUGINFO(lastModified)
    DEBUGINFO(lastRead)
    DEBUGINFO(caching)

}
