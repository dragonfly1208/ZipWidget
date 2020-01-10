#include "ZipModel.h"
#include "JlCompress.h"
#include "utils.h"
#include <QDebug>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QMessageBox>

#define DEBUG qDebug()<<__func__<<__LINE__
ZipModel::ZipModel(const QString& path, QObject *parent) :
    QAbstractItemModel(parent),rootInfo(QuaZipFileInfo64())
{
    rootInfo.SetRoot();
    zip = QSharedPointer<QuaZip>(new QuaZip(path));
    fetchRootDirectory();
}

ZipModel::~ZipModel()
{

}

QModelIndex ZipModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    if (!parent.isValid())
        return createIndex(row, column, const_cast<ZipFileInfo*>(&rootInfo.children()[row]));

    ZipFileInfo* parentInfo = FileInfo(parent);
    return createIndex(row, column, &parentInfo->children()[row]);
}

QModelIndex ZipModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return QModelIndex();

    ZipFileInfo* childInfo = FileInfo(child);
    ZipFileInfo* parentInfo = childInfo->parent();
    if (!parentInfo->isRoot()/*parentInfo != nullptr && parentInfo->*/)
        return createIndex(findRow(parentInfo), RamificationColumn, parentInfo);
    else
        return QModelIndex();
}

int ZipModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return rootInfo.childrenCount();

    return FileInfo(parent)->childrenCount();
}

int ZipModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return ColumnCount;
}

#include <QIcon>

QVariant ZipModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ZipFileInfo* nodeInfo;
    if(index.parent().isValid())
        nodeInfo = FileInfo(index);
    else
        nodeInfo = &rootInfo.children().at(index.row());

    switch (index.column()) {
    case NameColumn:
        if(role == Qt::DecorationRole)
        {
            return nodeInfo->icon();
        }
        else if(role == Qt::DisplayRole)
        {

            return nodeInfo->displayName();
        }
        break;
    case ModificationDateColumn:
        if (role == Qt::DisplayRole)
            return nodeInfo->lastModified();
        break;
    case SizeColumn:
        if (role == Qt::DisplayRole)
            return nodeInfo->isDir()? QVariant(): nodeInfo->displaySize();
        break;
    case CompressedSizeColumn:
        if (role == Qt::DisplayRole)
            return nodeInfo->isDir()? QVariant(): nodeInfo->displayCompressedSize();
        break;
    case TypeColumn:
        if (role == Qt::DisplayRole)
            return nodeInfo->type();
        break;
    default:
        break;
    }
    return QVariant();
}

QVariant ZipModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole &&
       section < ColumnCount)
    {
        QStringList headers;
        headers << "Name" << "Type" << "Size" <<"ZipSize"<< "DateModified";
        return headers[section];
    }
    return QVariant();
}

bool ZipModel::canFetchMore(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return false;

    const ZipFileInfo* parentInfo = FileInfo(parent);
    return !parentInfo->mapped;
}

void ZipModel::fetchMore(const QModelIndex &parent)
{
    ZipFileInfo* parentInfo = FileInfo(parent);
    QuaZipDir dir(zip.data(), parentInfo->absoluteFilePath());
    QList<QuaZipFileInfo64> children = dir.entryInfoList64(QStringList(),QDir::AllEntries | QDir::NoDot,
                                                       QDir::Name);
    DEBUG<<dir.filePath("qqq.mm");
    beginInsertRows(parent, 0, children.size() - 1);
    for(int i = 0; i < children.count(); ++i)
    {
        ZipFileInfo nodeInfo(children[i], parentInfo);
        nodeInfo.mapped = !nodeInfo.isDir();
        parentInfo->children().push_back(nodeInfo);
    }
    parentInfo->mapped = true;
    endInsertRows();
}


Qt::ItemFlags ZipModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    return flags;
}

bool ZipModel::hasChildren(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        const ZipFileInfo* parentInfo = FileInfo(parent);
        if(!parentInfo->isDir())
            return false;
        if(!parentInfo->mapped)
            return true;
    }
    return QAbstractItemModel::hasChildren(parent);
}

int ZipModel::findRow(const ZipFileInfo *nodeInfo) const
{
    const QList<ZipFileInfo>& parentInfoChildren = (!nodeInfo->parent() ->isRoot() && !nodeInfo->isRoot()) ?
                                                   nodeInfo->parent()->children() : rootInfo.children();
    QList<ZipFileInfo>::const_iterator position = qFind(parentInfoChildren, *nodeInfo);
    return std::distance(parentInfoChildren.begin(), position);
}

void ZipModel::fetchRootDirectory()
{
    if((zip->getMode() == QuaZip::mdNotOpen && zip->open(QuaZip::mdUnzip)) ||
       (zip->getMode() == QuaZip::mdUnzip))
    {
        QuaZipDir dir(zip.data());
        QList<QuaZipFileInfo64> lst = dir.entryInfoList64(QDir::AllEntries);

        DEBUG<<lst.count();
        rootInfo.clearChildren();
        for(int i = 0; i < lst.count(); ++i)
            rootInfo.children().append(ZipFileInfo(lst[i],&rootInfo));
    }
}
bool ZipModel::isValid(){
    return isOpen();
}
bool ZipModel::addFileToCurentZip(const QString &fPath ,const QModelIndex &dirIndex){
    //QSharedPointer<QuaZip> addzip = QSharedPointer<QuaZip>(new QuaZip(zip->getZipName()));
    ZipFileInfo *parentInfo = FileInfo(dirIndex);
    if(!parentInfo) parentInfo = &rootInfo;
    QString fileDest = QString("%1/%2").arg(parentInfo->absoluteFilePath()).arg(QFileInfo(fPath).fileName());
    zip->close();//关闭并以mdAdd模式打开
    if(!zip->open(QuaZip::mdAdd)){
        DEBUG<<"mdAppend mode 打开文件失败";
        return false;
    }
    DEBUG<<fPath<<fileDest;
    QuaZipFileInfo64 zipFileInfo;

    bool ok = JlCompress::compressFile(zip.data(),const_cast<QString&>(fPath),const_cast<QString&>(fileDest));
    DEBUG<<ok;
    if(ok){
        zip->close();
        ok = zip->open(QuaZip::mdUnzip);
    }
    if(ok){
        bool ok1 = zip->setCurrentFile(fileDest);
        DEBUG<<ok1;
        if(ok1){
            ok1 = zip->getCurrentFileInfo(&zipFileInfo);
            zipFileInfo.name = QFileInfo(zipFileInfo.name).fileName();
            //parentInfo->children().push_back(ZipFileInfo(zipFileInfo,parentInfo));

            beginInsertRows(dirIndex, parentInfo->childrenCount(), parentInfo->childrenCount());
            //for(int i = 0; i < children.count(); ++i)
            {
                ZipFileInfo nodeInfo(zipFileInfo,parentInfo);
                nodeInfo.mapped = !nodeInfo.isDir();
                parentInfo->children().push_back(nodeInfo);
            }
            parentInfo->mapped = true;
            endInsertRows();
        }
        DEBUG<<ok1;
    }

    DEBUG<<ok;

    return ok;

}
bool ZipModel::delSelectedFiles(QModelIndexList indexs){
    for(auto index:indexs){
        ZipFileInfo *info = FileInfo(index);
        info->absoluteFilePath();

    }
return true;
}
bool ZipModel::extractSelectedFiles(QModelIndexList indexs,QString destDir){
    QDir dir(destDir);
    if(!dir.exists()){
        DEBUG<<QString("directory \"%1\" is not exites.").arg(destDir);
        return false;
    }
    for(auto index:indexs){
        ZipFileInfo *info = FileInfo(index);
        QString zipFilePath = info->absoluteFilePath();
        QString localFilePath = dir.absoluteFilePath(info->fileName());
        DEBUG<<zipFilePath<<localFilePath;
        if(info->isFile()){
            DEBUG<<JlCompress::extractFile(zip.data(),zipFilePath,localFilePath);
        }else{
            if(!zipFilePath.isEmpty()&&zipFilePath[zipFilePath.length()-1] != '/') zipFilePath.push_back('/');
            DEBUG<<JlCompress::extractDir(*zip.data(),localFilePath,zipFilePath);
        }
    }
    return true;
}
bool ZipModel::addFilesToCurentZipDir( QStringList fileNames,const QModelIndex &dirIndex) {
    ZipFileInfo *parentInfo = FileInfo(dirIndex);
    if(!parentInfo) parentInfo = &rootInfo;
    QString fileDestDir = parentInfo->absoluteFilePath();

    zip->close();//关闭并以mdAdd模式打开
    if(!zip->open(QuaZip::mdAdd)){
        DEBUG<<"mdAppend mode 打开文件失败";
        zip->close();
        zip->open(QuaZip::mdUnzip);
        return false;
    }

    QuaZipFileInfo64 zipFileInfo;

    bool ok = false;
    QStringList fileDestList;
    //QStringList DirDestList;
    for(auto localName:fileNames){
            QFileInfo info(localName);
            if(!info.exists()){//不存在
                DEBUG<<QString("file \"%1\" is not exists.");
                continue;
            }
            QString fileDest;
            if(parentInfo->isRoot()){
                fileDest = info.fileName();
            }else
                fileDest = QString("%1/%2").arg(fileDestDir).arg(info.fileName());
DEBUG<<fileDest<<localName;
            if(info.isFile()){
                ok = JlCompress::compressFile(zip.data(),localName,fileDest);
                DEBUG<<ok;
                if(!ok){
                    DEBUG<<QString("file \"%1\" compress err.").arg(localName);
                    continue;
                }
            }else if(info.isDir()){
                //DirDestList.push_back(localName);
                ok = JlCompress::compressSubDir(zip.data(),localName,info.path(),true,0/*QDir::Filters(nullptr)*/,fileDestDir);
                DEBUG<<ok;
                if(!ok){
                    DEBUG<<QString("dir \"%1\" compress err.").arg(localName);
                    continue;
                }
            }
            if(ok){
                fileDestList.append(fileDest);
            }
    }

    if(fileDestList.length()>0){
        zip->close();
        ok = zip->open(QuaZip::mdUnzip);

        beginInsertRows(dirIndex, parentInfo->childrenCount(), parentInfo->childrenCount()+fileDestList.length()-1);

        for(auto fileDest:fileDestList){
            bool ok1 = zip->setCurrentFile(fileDest);
            DEBUG<<ok1;
            if(ok1){
                ok1 = zip->getCurrentFileInfo(&zipFileInfo);
                zipFileInfo.name = QFileInfo(Utils::trimmedRight(zipFileInfo.name,'/')).fileName();

                ZipFileInfo nodeInfo(zipFileInfo,parentInfo);
                nodeInfo.mapped = !nodeInfo.isDir();
                parentInfo->children().push_back(nodeInfo);
            }
            DEBUG<<ok1;
        }
        parentInfo->mapped = true;
        endInsertRows();
    }

    return true;
}
