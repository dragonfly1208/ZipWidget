#include "ZipFileInfo.h"
#include "utils.h"
#include <QtWin>
#include <QApplication>
#include <QStyle>
#include <QIcon>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QDebug>
#define DEBUG qDebug()<<__func__<<__LINE__
//#include <ddk/wdm.h>
#define FILE_ATTRIBUTE_DIRECTORY          (0040000<<16)//(0x00000010 | 0x40000000)
//#define FILE_ATTRIBUTE_ENCRYPTED          0x00004000  //==0040000
QMimeDatabase ZipFileInfo::mime_database;
ZipFileInfo::ZipFileInfo(const QuaZipFileInfo64 &f, ZipFileInfo* p):
    m_fileInfo(f),
    m_parent(p)
{
    mapped = !isDir();
    m_children.clear();
}

ZipFileInfo::~ZipFileInfo()
{
    m_children.clear();
}

ZipFileInfo &ZipFileInfo::operator=(const ZipFileInfo &another)
{
    this->m_fileInfo = another.m_fileInfo;
    this->mapped = another.mapped;
    this->m_parent = another.m_parent;
    this->m_children = another.m_children;
    return *this;
}

bool ZipFileInfo::operator ==(const ZipFileInfo &another) const
{
    if(this->m_children != another.m_children)
        return false;
    if(this->m_parent != another.m_parent)
        return false;
    if(this->m_fileInfo.name != another.m_fileInfo.name)
        return false;
    return true;
}


QString ZipFileInfo::type() const
{
    std::string type;
    if(isDir())
        return QString::fromLocal8Bit(Utils::folderType().c_str());
    else{
        type =  Utils::fileType(suffix().toStdString());
        return QString::fromLocal8Bit(type.c_str());

    }



}

QString ZipFileInfo::displaySize(quint64 s) const
{
    if(s < 1024)
        return QString("%1 Bytes").arg(s);
    if(s < 1024 * 1024)
        return QString("%1 KB").arg(static_cast<double>(s) / 1024, 0, 'g', 4);
    if(s < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(static_cast<double>(s) / (1024 * 1024), 0, 'g', 4);
    return QString("%1 GB").arg(static_cast<double>(s) / (1024 * 1024 * 1024), 0, 'g', 4);
}

QString ZipFileInfo::displaySize() const{
    return displaySize(m_fileInfo.uncompressedSize);
}
QString ZipFileInfo::displayUncompressedSize() const{
    return displaySize(m_fileInfo.uncompressedSize);
}
QString ZipFileInfo::displayCompressedSize() const{
    return displaySize(m_fileInfo.compressedSize);
}


QString ZipFileInfo::displayName() const{
    int index = m_fileInfo.name.lastIndexOf('/');
    if(isDir() &&index == (m_fileInfo.name.length()-1))
        return m_fileInfo.name.left(index);
    return m_fileInfo.name;
}

int ZipFileInfo::childrenCount() const
{
    if(!isDir())
        return 0;
    else
        return m_children.size();
}

//QString ZipFileInfo::filePath() const
//{
//    if(!m_parent)
//        return m_fileInfo.name;
//    return m_parent->filePath() + m_fileInfo.name;
//}
QIcon ZipFileInfo::iconForFilename(const QString &filename)
{
  QIcon icon;
  QList<QMimeType> mime_types = mime_database.mimeTypesForFileName(filename);
  for (int i=0; i < mime_types.count() && icon.isNull(); i++)
    icon = QIcon::fromTheme(mime_types[i].iconName());

  if (icon.isNull())
    return QApplication::style()->standardIcon(QStyle::SP_FileIcon);
  else
    return icon;
}


QString ZipFileInfo::filePath() const{//
    return m_fileInfo.name;
}
QString ZipFileInfo::absoluteFilePath() const{
    if(!m_parent || m_parent->isRoot())
        return fileName();
    return QString("%1/%2").arg( m_parent->absoluteFilePath()).arg(fileName());
}
QString ZipFileInfo::canonicalFilePath() const{//
    return absoluteFilePath();
}
QString ZipFileInfo::fileName() const{
    return m_fileInfo.name;
}
QString ZipFileInfo::baseName() const{
    int index = -1;
    if(isDir() || (index = m_fileInfo.name.indexOf('.')) <=0)
        return m_fileInfo.name;
    return m_fileInfo.name.left(m_fileInfo.name.length()-index);
}
QString ZipFileInfo::completeBaseName() const{//
    QString sf = suffix();
    if(sf.isEmpty()) return m_fileInfo.name;
    return m_fileInfo.name.left(m_fileInfo.name.length() - sf.length()-1);
}
QString ZipFileInfo::suffix() const{
    int index = -1;
    if(isDir() || (index = m_fileInfo.name.lastIndexOf('.')) <=0)
        return "";
    return m_fileInfo.name.mid(index);
}

QString ZipFileInfo::completeSuffix() const{
    QString name = baseName();
    if(name.length() == m_fileInfo.name.length() ) return "";
    return m_fileInfo.name.mid(name.length()+1);
}

QString ZipFileInfo::path() const{//
    return absolutePath();
}
QString ZipFileInfo::absolutePath() const{
    if(!m_parent)
        return "";//"/";
    return m_parent->absoluteFilePath();
}
QString ZipFileInfo::canonicalPath() const{//
    return absolutePath();
}
//QDir ZipFileInfo::dir() const;
//QDir ZipFileInfo::absoluteDir() const;

bool ZipFileInfo::isReadable() const{
    return m_fileInfo.getPermissions() &QFile::ReadOwner;// & QFile::ReadGroup &QFile::ReadOther;
}
bool ZipFileInfo::isWritable() const{
    return m_fileInfo.getPermissions() &QFile::WriteOwner;
}
bool ZipFileInfo::isExecutable() const{
    {
        return m_fileInfo.getPermissions() &QFile::ExeOwner;
    }
}
bool ZipFileInfo::isHidden() const{//
    return false;
}
bool ZipFileInfo::isNativePath() const{//
    return false;
}

bool ZipFileInfo::isRelative() const{//
    return false;
}

bool ZipFileInfo::makeAbsolute(){
    return false;
}

bool ZipFileInfo::isFile() const{//
    return !isDir();
}
bool ZipFileInfo::isDir() const
{
    QString s = m_fileInfo.name;
    if(m_fileInfo.externalAttr & FILE_ATTRIBUTE_DIRECTORY /*|| s.at(s.length()-1) == '/'*/)
        return true;
    else
        return false;
}
bool ZipFileInfo::isSymLink() const{//
    return false;
}
bool ZipFileInfo::isRoot() const{//
    return m_fileInfo.name.isEmpty();
}


//QString ZipFileInfo::readLink() const;


QString ZipFileInfo::owner() const{
    return "";
}
uint ZipFileInfo::ownerId() const{//
    return  m_fileInfo.versionCreated;
}
QString ZipFileInfo::group() const{
    return "";
}
uint ZipFileInfo::groupId() const{//
    return  m_fileInfo.versionNeeded;
}


bool ZipFileInfo::permission(QFile::Permissions permissions) const{
    return m_fileInfo.getPermissions() & permissions;
}
QFile::Permissions ZipFileInfo::permissions() const{
    return m_fileInfo.getPermissions();
}

quint64 ZipFileInfo::size() const{
    return m_fileInfo.uncompressedSize;
}
quint64 ZipFileInfo::uncompressedSize() const{
    return m_fileInfo.uncompressedSize;
}
quint64 ZipFileInfo::compressedSize() const{
    return m_fileInfo.compressedSize;
}

QDateTime ZipFileInfo::created() const{
    return m_fileInfo.dateTime;
}
QDateTime ZipFileInfo::lastModified() const{
    return m_fileInfo.dateTime;
}
QDateTime ZipFileInfo::lastRead() const{
    return m_fileInfo.dateTime;
}
QIcon ZipFileInfo::icon()const {
    if(isDir())
        return QIcon(QtWin::fromHICON(Utils::folderIcon()));
    else{
        if(!suffix().isEmpty())
            return QIcon(QtWin::fromHICON(Utils::fileIcon(suffix().toStdString())));
        else
            return QFileIconProvider().icon(QFileIconProvider::File);

    }

}
void ZipFileInfo::SetRoot(){
    if(parent()){
        qWarning()<<"SetRoot error:has parent.";
    }
    m_fileInfo.externalAttr |= FILE_ATTRIBUTE_DIRECTORY;
    m_fileInfo.name.clear();
    mapped = !isDir();
    m_children.clear();
}

void ZipFileInfo::refresh(){

}
