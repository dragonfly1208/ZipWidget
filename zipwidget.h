#ifndef ZIPWIDGET_H
#define ZIPWIDGET_H

#include <QWidget>
#include <QTreeView>
#include "filesystemview.h"
#include <ZipModel.h>

class ZipWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ZipWidget(QWidget *parent = nullptr);
    ~ZipWidget();

    bool setFilePath(const QString &path);
    bool addFileToCurentZip(const QString &fPath);
    bool addFilesToCurentZip(const QStringList &fPaths);
    bool delSelectedFile();
    bool extractSelectedFiles(QString destDir);
signals:
    void curentFilePathChanged(QString curentFilePath);
private:
    bool checkZipModel();
private slots:
    void onItemActivated(const QModelIndex&) const;//let's open some file
    void open(QModelIndex index);
private:
    FileSystemView* view = nullptr;
    QString pathToArchive;
    ZipModel* model = nullptr;
    QModelIndex curentDirIndex;


};

#endif // ZIPWIDGET_H
