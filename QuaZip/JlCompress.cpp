/*
Copyright (C) 2010 Roberto Pompermaier
Copyright (C) 2005-2014 Sergey A. Tachenov

This file is part of QuaZIP.

QuaZIP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

QuaZIP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QuaZIP.  If not, see <http://www.gnu.org/licenses/>.

See COPYING file for the full LGPL text.

Original ZIP package is copyrighted by Gilles Vollant and contributors,
see quazip/(un)zip.h files for details. Basically it's the zlib license.
*/

#include "JlCompress.h"
#include <QDebug>

static bool copyData(QIODevice &inFile, QIODevice &outFile)
{
    while (!inFile.atEnd()) {
        char buf[4096];
        qint64 readLen = inFile.read(buf, 4096);
        if (readLen <= 0)
            return false;
        if (outFile.write(buf, readLen) != readLen)
            return false;
    }
    return true;
}

bool JlCompress::compressFile(QuaZip* zip, QString fileName, QString fileDest) {
    // zip: oggetto dove aggiungere il file
    // fileName: nome del file reale
    // fileDest: nome del file all'interno del file compresso

    // Controllo l'apertura dello zip
    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    // Apro il file originale
    QFile inFile;
    inFile.setFileName(fileName);
    if(!inFile.open(QIODevice::ReadOnly)) return false;

    // Apro il file risulato
    QuaZipFile outFile(zip);
    if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, inFile.fileName()))) return false;

    // Copio i dati
    if (!copyData(inFile, outFile) || outFile.getZipError()!=UNZ_OK) {
        return false;
    }

    // Chiudo i file
    outFile.close();
    if (outFile.getZipError()!=UNZ_OK) return false;
    inFile.close();

    return true;
}

bool JlCompress::compressSubDir(QuaZip* zip, QString dir, QString origDir, bool recursive, QDir::Filters filters,const QString &parentZipDir) {
    // zip: oggetto dove aggiungere il file
    // dir: cartella reale corrente
    // origDir: cartella reale originale
    // (path(dir)-path(origDir)) = path interno all'oggetto zip

    // Controllo l'apertura dello zip
    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    // Controllo la cartella
    QDir directory(dir);
    if (!directory.exists()) return false;

    QDir origDirectory(origDir);
	if (dir != origDir) {
		QuaZipFile dirZipFile(zip);
        QString name = parentZipDir.isEmpty()?(origDirectory.relativeFilePath(dir) + QLatin1String("/")):(parentZipDir+QLatin1String("/")+origDirectory.relativeFilePath(dir) + QLatin1String("/"));
		if (!dirZipFile.open(QIODevice::WriteOnly,
            QuaZipNewInfo(name, dir), 0, 0, 0)) {
				return false;
		}
		dirZipFile.close();
	}


    // Se comprimo anche le sotto cartelle
    if (recursive) {
        // Per ogni sotto cartella
        QFileInfoList files = directory.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot|filters);
        for (int index = 0; index < files.size(); ++index ) {
            const QFileInfo & file( files.at( index ) );
#if QT_VERSION < QT_VERSION_CHECK(4, 7, 4)
            if (!file.isDir())
                continue;
#endif
            // Comprimo la sotto cartella
            if(!compressSubDir(zip,file.absoluteFilePath(),origDir,recursive,filters,parentZipDir)) return false;
        }
    }

    // Per ogni file nella cartella
    QFileInfoList files = directory.entryInfoList(QDir::Files|filters);
    for (int index = 0; index < files.size(); ++index ) {
        const QFileInfo & file( files.at( index ) );
        // Se non e un file o e il file compresso che sto creando
        if(!file.isFile()||file.absoluteFilePath()==zip->getZipName()) continue;

        // Creo il nome relativo da usare all'interno del file compresso
        QString filename = parentZipDir.isEmpty()?origDirectory.relativeFilePath(file.absoluteFilePath()):(parentZipDir+QLatin1String("/")+origDirectory.relativeFilePath(file.absoluteFilePath()));

        // Comprimo il file
        if (!compressFile(zip,file.absoluteFilePath(),filename)) return false;
    }

    return true;
}

bool JlCompress::extractFile(QuaZip* zip, QString fileName, QString fileDest) {
    // zip: oggetto dove aggiungere il file
    // filename: nome del file reale
    // fileincompress: nome del file all'interno del file compresso

    // Controllo l'apertura dello zip
    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdUnzip) return false;

    // Apro il file compresso
    if (!fileName.isEmpty())
        zip->setCurrentFile(fileName);
    QuaZipFile inFile(zip);
    if(!inFile.open(QIODevice::ReadOnly) || inFile.getZipError()!=UNZ_OK) return false;

    // Controllo esistenza cartella file risultato
    QDir curDir;
    if (fileDest.endsWith(QLatin1String("/"))) {
        if (!curDir.mkpath(fileDest)) {
            return false;
        }
    } else {
        if (!curDir.mkpath(QFileInfo(fileDest).absolutePath())) {
            return false;
        }
    }

    QuaZipFileInfo64 info;
    if (!zip->getCurrentFileInfo(&info))
        return false;

    QFile::Permissions srcPerm = info.getPermissions();
    if (fileDest.endsWith(QLatin1String("/")) && QFileInfo(fileDest).isDir()) {
        if (srcPerm != 0) {
            QFile(fileDest).setPermissions(srcPerm);
        }
        return true;
    }

    // Apro il file risultato
    QFile outFile;
    outFile.setFileName(fileDest);
    if(!outFile.open(QIODevice::WriteOnly)) return false;

    // Copio i dati
    if (!copyData(inFile, outFile) || inFile.getZipError()!=UNZ_OK) {
        outFile.close();
        removeFile(QStringList(fileDest));
        return false;
    }
    outFile.close();

    // Chiudo i file
    inFile.close();
    if (inFile.getZipError()!=UNZ_OK) {
        removeFile(QStringList(fileDest));
        return false;
    }

    if (srcPerm != 0) {
        outFile.setPermissions(srcPerm);
    }
    return true;
}

bool JlCompress::removeFile(QStringList listFile) {
    bool ret = true;
    // Per ogni file
    for (int i=0; i<listFile.count(); i++) {
        // Lo elimino
        ret = ret && QFile::remove(listFile.at(i));
    }
    return ret;
}

bool JlCompress::compressFile(QString fileCompressed, QString file) {
    // Creo lo zip
    QuaZip zip(fileCompressed);
    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip.open(QuaZip::mdCreate)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Aggiungo il file
    if (!compressFile(&zip,file,QFileInfo(file).fileName())) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        QFile::remove(fileCompressed);
        return false;
    }

    return true;
}

bool JlCompress::compressFiles(QString fileCompressed, QStringList files) {
    // Creo lo zip
    QuaZip zip(fileCompressed);
    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip.open(QuaZip::mdCreate)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Comprimo i file
    QFileInfo info;
    for (int index = 0; index < files.size(); ++index ) {
        const QString & file( files.at( index ) );
        info.setFile(file);
        if (!info.exists() || !compressFile(&zip,file,info.fileName())) {
            QFile::remove(fileCompressed);
            return false;
        }
    }

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        QFile::remove(fileCompressed);
        return false;
    }

    return true;
}

bool JlCompress::compressDir(QString fileCompressed, QString dir, bool recursive) {
    return compressDir(fileCompressed, dir, recursive, 0);
}

bool JlCompress::compressDir(QString fileCompressed, QString dir,
                             bool recursive, QDir::Filters filters)
{
    // Creo lo zip
    QuaZip zip(fileCompressed);
    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip.open(QuaZip::mdCreate)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Aggiungo i file e le sotto cartelle
    if (!compressSubDir(&zip,dir,dir,recursive, filters)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        QFile::remove(fileCompressed);
        return false;
    }

    return true;
}

QString JlCompress::extractFile(QString fileCompressed, QString fileName, QString fileDest) {
    // Apro lo zip
    QuaZip zip(fileCompressed);
    return extractFile(zip, fileName, fileDest);
}

QString JlCompress::extractFile(QuaZip &zip, QString fileName, QString fileDest)
{
    if(!zip.open(QuaZip::mdUnzip)) {
        return QString();
    }

    // Estraggo il file
    if (fileDest.isEmpty())
        fileDest = fileName;
    if (!extractFile(&zip,fileName,fileDest)) {
        return QString();
    }

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(QStringList(fileDest));
        return QString();
    }
    return QFileInfo(fileDest).absoluteFilePath();
}

QStringList JlCompress::extractFiles(QString fileCompressed, QStringList files, QString dir) {
    // Creo lo zip
    QuaZip zip(fileCompressed);
    return extractFiles(zip, files, dir);
}

QStringList JlCompress::extractFiles(QuaZip &zip, const QStringList &files, const QString &dir)
{
    if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }

    // Estraggo i file
    QStringList extracted;
    for (int i=0; i<files.count(); i++) {
        QString absPath = QDir(dir).absoluteFilePath(files.at(i));
        if (!extractFile(&zip, files.at(i), absPath)) {
            removeFile(extracted);
            return QStringList();
        }
        extracted.append(absPath);
    }

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }

    return extracted;
}

QStringList JlCompress::extractDir(QString fileCompressed, QString dir,const QString &parentZipDir) {
    // Apro lo zip
    QuaZip zip(fileCompressed);
    return extractDir(zip, dir,parentZipDir);
}

QStringList JlCompress::extractDir(QuaZip &zip, const QString &dir,const QString &parentZipDir)
{
    if(zip.getMode() == QuaZip::mdUnzip)
        ;
    else if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }
    QString cleanDir = QDir::cleanPath(dir);
    QDir directory(cleanDir);
    QString absCleanDir = directory.absolutePath();
    QStringList extracted;
    if (!zip.goToFirstFile()) {
        return QStringList();
    }

    bool isempty = parentZipDir.isEmpty();
    do {
        QString name = zip.getCurrentFileName();
        if(!isempty && name.startsWith(parentZipDir)) continue;
        QString absFilePath = directory.absoluteFilePath(name);
        QString absCleanPath = QDir::cleanPath(absFilePath);
        if (!absCleanPath.startsWith(absCleanDir + QLatin1String("/")))
            continue;
        if (!extractFile(&zip, QLatin1String(""), absFilePath)) {
            removeFile(extracted);
            return QStringList();
        }
        extracted.append(absFilePath);
    } while (zip.goToNextFile());

    // Chiudo il file zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }

    return extracted;
}

QStringList JlCompress::getFileList(QString fileCompressed) {
    // Apro lo zip
    QuaZip* zip = new QuaZip(QFileInfo(fileCompressed).absoluteFilePath());
    return getFileList(zip);
}

QStringList JlCompress::getFileList(QuaZip *zip)
{
    if(!zip->open(QuaZip::mdUnzip)) {
        delete zip;
        return QStringList();
    }

    // Estraggo i nomi dei file
    QStringList lst;
    QuaZipFileInfo64 info;
    for(bool more=zip->goToFirstFile(); more; more=zip->goToNextFile()) {
      if(!zip->getCurrentFileInfo(&info)) {
          delete zip;
          return QStringList();
      }
      lst << info.name;
      //info.name.toLocal8Bit().constData()
    }

    // Chiudo il file zip
    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        return QStringList();
    }
    delete zip;
    return lst;
}

QStringList JlCompress::extractDir(QIODevice *ioDevice, QString dir)
{
    QuaZip zip(ioDevice);
    return extractDir(zip, dir);
}

QStringList JlCompress::getFileList(QIODevice *ioDevice)
{
    QuaZip *zip = new QuaZip(ioDevice);
    return getFileList(zip);
}

QString JlCompress::extractFile(QIODevice *ioDevice, QString fileName, QString fileDest)
{
    QuaZip zip(ioDevice);
    return extractFile(zip, fileName, fileDest);
}

QStringList JlCompress::extractFiles(QIODevice *ioDevice, QStringList files, QString dir)
{
    QuaZip zip(ioDevice);
    return extractFiles(zip, files, dir);
} 
/*
int DeleteFileFromZIP(const char* zip_name, const char* del_file)
{
    bool some_was_del = false;

    // change name for temp file
    char* tmp_name = (char*)malloc((strlen(zip_name) + 5)*sizeof(char));
    strcpy(tmp_name, zip_name);
    strncat(tmp_name, ".tmp", 5);

    // open source and destination file
    zlib_filefunc64_def ffunc;
    fill_qiodevice64_filefunc(&ffunc);//

    zipFile szip = unzOpen2_64((voidpf)zip_name, &ffunc);
    if (szip==NULL) { free(tmp_name); return -1; }
    zipFile dzip = zipOpen2_64(tmp_name, APPEND_STATUS_CREATE, NULL, &ffunc);
    if (dzip==NULL) { unzClose(szip); free(tmp_name); return -1; }

    // get global commentary
    unz_global_info glob_info;
    if (unzGetGlobalInfo(szip, &glob_info) != UNZ_OK) { zipClose(dzip, NULL); unzClose(szip); free(tmp_name); return -1; }

    char* glob_comment = NULL;
    if (glob_info.size_comment > 0)
    {
        glob_comment = (char*)malloc(glob_info.size_comment+1);
        if ((glob_comment==NULL)&&(glob_info.size_comment!=0)) { zipClose(dzip, NULL); unzClose(szip); free(tmp_name); return -1; }

        if ((unsigned int)unzGetGlobalComment(szip, glob_comment, glob_info.size_comment+1) != glob_info.size_comment)  { zipClose(dzip, NULL); unzClose(szip); free(glob_comment); free(tmp_name); return -1; }
    }

    // copying files
    int n_files = 0;

    int rv = unzGoToFirstFile(szip);
    while (rv == UNZ_OK)
    {
        // get zipped file info
        unz_file_info64 unzfi;
        char dos_fn[QuaZip::MAX_FILE_NAME_LENGTH];
        if (unzGetCurrentFileInfo64(szip, &unzfi, dos_fn, QuaZip::MAX_FILE_NAME_LENGTH, NULL, 0, NULL, 0) != UNZ_OK) break;
        char fn[QuaZip::MAX_FILE_NAME_LENGTH];
        OemToChar(dos_fn, fn);

        // if not need delete this file
        if (stricmp(fn, del_file)==0) // lowercase comparison
            some_was_del = true;
        else
        {
            char* extrafield = (char*)malloc(unzfi.size_file_extra);
            if ((extrafield==NULL)&&(unzfi.size_file_extra!=0)) break;
            char* commentary = (char*)malloc(unzfi.size_file_comment);
            if ((commentary==NULL)&&(unzfi.size_file_comment!=0)) {free(extrafield); break;}

            if (unzGetCurrentFileInfo64(szip, &unzfi, dos_fn, QuaZip::MAX_FILE_NAME_LENGTH, extrafield, unzfi.size_file_extra, commentary, unzfi.size_file_comment) != UNZ_OK) {free(extrafield); free(commentary); break;}

            // open file for RAW reading
            int method;
            int level;
            if (unzOpenCurrentFile2(szip, &method, &level, 1)!=UNZ_OK) {free(extrafield); free(commentary); break;}

            int size_local_extra = unzGetLocalExtrafield(szip, NULL, 0);
            if (size_local_extra<0) {free(extrafield); free(commentary); break;}
            void* local_extra = malloc(size_local_extra);
            if ((local_extra==NULL)&&(size_local_extra!=0)) {free(extrafield); free(commentary); break;}
            if (unzGetLocalExtrafield(szip, local_extra, size_local_extra)<0) {free(extrafield); free(commentary); free(local_extra); break;}

            // this malloc may fail if file very large
            void* buf = malloc(unzfi.compressed_size);
            if ((buf==NULL)&&(unzfi.compressed_size!=0)) {free(extrafield); free(commentary); free(local_extra); break;}

            // read file
            int sz = unzReadCurrentFile(szip, buf, unzfi.compressed_size);
            if ((unsigned int)sz != unzfi.compressed_size) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            // open destination file
            zip_fileinfo zfi;
            memcpy (&zfi.tmz_date, &unzfi.tmu_date, sizeof(tm_unz));
            zfi.dosDate = unzfi.dosDate;
            zfi.internal_fa = unzfi.internal_fa;
            zfi.external_fa = unzfi.external_fa;

            if (zipOpenNewFileInZip2(dzip, dos_fn, &zfi, local_extra, size_local_extra, extrafield, unzfi.size_file_extra, commentary, method, level, 1)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            // write file
            if (zipWriteInFileInZip(dzip, buf, unzfi.compressed_size)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            if (zipCloseFileInZipRaw(dzip, unzfi.uncompressed_size, unzfi.crc)!=UNZ_OK) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}

            if (unzCloseCurrentFile(szip)==UNZ_CRCERROR) {free(extrafield); free(commentary); free(local_extra); free(buf); break;}
            free(commentary);
            free(buf);
            free(extrafield);
            free(local_extra);

            n_files ++;
        }

        rv = unzGoToNextFile(szip);
    }

    zipClose(dzip, glob_comment);
    unzClose(szip);

    free(glob_comment);


    // if fail
    if ( (!some_was_del) || (rv!=UNZ_END_OF_LIST_OF_FILE) )
    {
        remove(tmp_name);
        free(tmp_name);
        return -1;
    }

    remove(zip_name);
    if (rename(tmp_name, zip_name) != 0)
    {
        free(tmp_name);
        return -1;
    }

    // if all files were deleted
    if (n_files==0) remove(zip_name);

    free(tmp_name);
    return 0;
}
*/
