#include "zipwidget.h"
#include <QVBoxLayout>
#include <QDebug>
#include "QuaZip/JlCompress.h"
#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>
#include <QString>
#include <QDebug>

#define DEBUG qDebug()<<__func__<<__LINE__

ZipWidget::ZipWidget(QWidget *parent)
    : QWidget(parent)
{
    //pathToArchive = QDir::currentPath() + "/ZipModel.zip";
    //qDebug()<<pathToArchive;
    //model = new ZipModel(pathToArchive, this);
    view = new FileSystemView(this);
    //view->setModel(model);

    connect(view, SIGNAL(activated(QModelIndex)),
            this, SLOT(onItemActivated(QModelIndex)));

    connect(view, &FileSystemView::doubleClicked, this, &ZipWidget::open);

    QVBoxLayout* vl = new QVBoxLayout(this);//
    vl->addWidget(view);
    setLayout(vl);
}

ZipWidget::~ZipWidget()
{
}

bool ZipWidget::setFilePath(const QString &path){
    QFileInfo info(path);
    if(!info.exists()) return false;

    ZipModel* modeltmp = new ZipModel(path, this);
    if(!modeltmp->isOpen()) return false;

    view->setModel(modeltmp);
    pathToArchive = path;
    if(model != nullptr) delete  model;
    model = modeltmp;
    curentDirIndex = QModelIndex();
    emit curentFilePathChanged(info.fileName());
    return true;
}


void ZipWidget::onItemActivated(const QModelIndex &i) const
{
    ZipFileInfo* zipfileinfo = model->FileInfo(i);
    if(zipfileinfo->isDir())
        return;

    QString path = zipfileinfo->filePath();

    QString file = QDir::tempPath() + '/' + zipfileinfo->fileInfo().name;
    if(JlCompress::extractFile(pathToArchive, path, file).length() > 0)
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }
    else
    {
        qDebug() << "Get some error on extracting " << file;
    }
}
void ZipWidget::open(QModelIndex index){

    auto fileInfo = model->FileInfo(index);
    if (fileInfo->isDir())
    {
        //qDebug()<<fileInfo->filePath();
        QString absoluteFilePath;
        if (fileInfo->displayName() != "..")//"/.."
        {
            //view->setRootIndex(index);
            curentDirIndex = index;
            //absoluteFilePath = fileInfo->absoluteFilePath();
        }
        else
        {
            QModelIndex p = model->parent(model->parent(index));
            curentDirIndex = p;
            //view->setRootIndex(p);
            //view->setModel(model.parent(index));

        }
        view->setRootIndex(curentDirIndex);
        if(curentDirIndex.isValid()){
            absoluteFilePath = model->FileInfo(curentDirIndex)->absoluteFilePath();
        }
        emit curentFilePathChanged(QString("%1/%2").arg(QFileInfo(pathToArchive).fileName()).arg(absoluteFilePath));

    }
}
bool ZipWidget::checkZipModel(){
    if(model && model->isValid()) return true;
    QMessageBox::warning(this,"警告","文件未正确打开!");
    return false;
}
bool ZipWidget::addFileToCurentZip(const QString &fPath){
    if(!checkZipModel()) return false;
    bool ok = model->addFileToCurentZip(fPath,curentDirIndex);
    DEBUG<<ok;
    if(ok){

        view->setRootIndex(curentDirIndex);
    }
    return ok;
}
bool ZipWidget::addFilesToCurentZip(const QStringList &fPaths){
    if(!checkZipModel()) return false;
    bool ok = model->addFilesToCurentZipDir(fPaths,curentDirIndex);
    DEBUG<<ok;
    if(ok){

        view->setRootIndex(curentDirIndex);
    }
    return ok;
}
bool ZipWidget::delSelectedFile(){
    if(!checkZipModel()) return false;
    QModelIndexList indexs = view->selectedIndexes();
    if(indexs.count()<=0) return true;
    DEBUG<<indexs.count();
    return model->delSelectedFiles(indexs);
}
bool ZipWidget::extractSelectedFiles(QString destDir){
    if(!checkZipModel()) return false;
    //QModelIndexList indexs = view->selectedIndexes();
    QModelIndexList indexs = view->selectionModel()->selectedRows();
    DEBUG<<indexs.count();
    if(indexs.count()<=0) return true;

    return model->extractSelectedFiles(indexs,destDir);
}
