#include "filedialogex.h"
#include <QtWidgets/private/qfiledialog_p.h>
#include <QDialogButtonBox>
#include <QListView>
#include <QTreeView>

FileDialogEx::FileDialogEx(QWidget *parent,const QString &caption,const QString &directory,const QString &filter):
    QFileDialog(parent,caption,directory,filter){
}
FileDialogEx::FileDialogEx(const QFileDialogArgs &args):QFileDialog (args){

}
void FileDialogEx::dontUseNativeDialog(){
    //修改open按钮的动作
    this->setOption(QFileDialog::DontUseNativeDialog,true);//关键命令
    QDialogButtonBox *btnbox = this->findChild<QDialogButtonBox*>("buttonBox");
    disconnect(btnbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(btnbox, SIGNAL(accepted()), this, SLOT(on_clicked_btn_open()));
    //设置QFileDialog内部的表现方式
    QListView *lv = this->findChild<QListView*>("listView");
    if(lv)
        lv->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QTreeView *tv = this->findChild<QTreeView*>();
        tv->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void FileDialogEx::on_clicked_btn_open()
{
    QDialog::accept();

}

QStringList FileDialogEx::getOpenFileAndDirNames(
        QWidget *parent,const QString &caption,const QString &dir,const QString &filter,QString *selectedFilter,Options options){

    const QStringList schemes;// = QStringList(QStringLiteral("file"));
    const QList<QUrl> selectedUrls = getOpenFileAndDirUrls(parent, caption, QUrl::fromLocalFile(dir), filter, selectedFilter, options, schemes);
    QStringList fileNames;
    fileNames.reserve(selectedUrls.size());
    for (const QUrl &url : selectedUrls)
        fileNames << url.toLocalFile();
    return fileNames;
}
QList<QUrl> FileDialogEx::getOpenFileAndDirUrls
(QWidget *parent,const QString &caption,const QUrl &dir,const QString &filter,QString *selectedFilter,Options options,const QStringList &supportedSchemes){
    QFileDialogArgs args;
    args.parent = parent;
    args.caption = caption;
    args.directory = QFileDialogPrivate::workingDirectory(dir);
    args.selection = QFileDialogPrivate::initialSelection(dir);
    args.filter = filter;
    args.mode = ExistingFiles;
    args.options = options;

    FileDialogEx dialog(args);
    dialog.dontUseNativeDialog();
    if (selectedFilter && !selectedFilter->isEmpty())
        dialog.selectNameFilter(*selectedFilter);
    if (dialog.exec() == QDialog::Accepted) {
        if (selectedFilter)
            *selectedFilter = dialog.selectedNameFilter();
        return dialog.selectedUrls();
    }
    return QList<QUrl>();

}
