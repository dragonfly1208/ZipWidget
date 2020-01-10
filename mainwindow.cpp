#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDialogButtonBox>
#include "filedialogex.h"
#include "JlCompress.h"

#include <QDebug>
#define DEBUG qDebug()<<__func__<<__LINE__
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionOpen,&QAction::triggered,this,[&](){
        QString filePath = QFileDialog::getOpenFileName(this,"选择文件",QDir::currentPath(),tr("Compress (*.zip *.gz);;All (*)"));
        if(filePath.isEmpty()) return;
        bool ok = ui->browserWidget->setFilePath(filePath);
        if(!ok){
            QMessageBox::critical(this,"错误",QString("打开文件\n\"%1\"\n失败！").arg(filePath));
        }
    });

    connect(ui->btnOpen,&QPushButton::clicked,this,[&](){
        //FileDialogEx smfs;//(this,"选择",".");
//        smfs.setOption(QFileDialog::DontUseNativeDialog,true);
//        smfs.setFileMode(QFileDialog::ExistingFiles);
//        smfs.setFilter(QDir::Drives | QDir::AllDirs | QDir::Dirs|QDir::Files);
//        smfs.setLabelText(QFileDialog::Accept,"打开111");
//        QDialogButtonBox *btnbox = smfs.findChild<QDialogButtonBox*>("buttonBox");
//        DEBUG<< btnbox;
//        if(smfs.exec())
//        {
//            qDebug()<< smfs.selectedFiles();
//        }

        //QStringList filePaths = FileDialogEx::getOpenFileAndDirNames(this,"打开",QDir::currentPath(),tr("All (*)"));
        //qDebug()<<filePaths;
        //JlCompress::compressDir("F:/C++/git/QtZipModel-master/bin/nnn.zip","F:/C++/git/QtZipModel-master/bin/nnn1");

         DEBUG<<JlCompress::extractDir("F:/C++/git/QtZipModel-master/bin/nnn1.zip","F:/C++/git/QtZipModel-master/bin/nnn2","nnn1/aaa/");

    });


    Connect();
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Connect(){
    connect(ui->browserWidget,&ZipWidget::curentFilePathChanged,[&](QString path){
        ui->leditPath->setText(path);
    });
}

void MainWindow::on_actionSaveAs_triggered()
{

}

void MainWindow::on_actionAdd_triggered(){
//    QString filePath = QFileDialog::getOpenFileName(this,"打开",QDir::currentPath(),tr("All (*)"));
//    if(filePath.isEmpty()) return;
//    bool ok = ui->browserWidget->addFileToCurentZip(filePath);
//    if(!ok){
//        QMessageBox::critical(this,"错误",QString("添加文件\n\"%1\"\n失败！").arg(filePath));
//    }


//    QFileDialog fileDlg( this, tr("选择文件和文件夹"), QDir::currentPath(), tr("All (*)"));
//    fileDlg.setFileMode(QFileDialog::ExistingFiles);
//    fileDlg.setOption(QFileDialog::DontUseNativeDialog,true);
//    if(!fileDlg.exec()) return ;
//    QStringList filePaths = fileDlg.selectedFiles();
//    DEBUG<<filePaths;
    QStringList filePaths = FileDialogEx::getOpenFileAndDirNames(this,"打开",QDir::currentPath(),tr("All (*)"));

    if(filePaths.isEmpty()) return;
    bool ok = ui->browserWidget->addFilesToCurentZip(filePaths);
    if(!ok){
        QMessageBox::critical(this,"错误",QString("添加文件失败！"));
    }
}

void MainWindow::on_actionUnCompressAs_triggered(){
    QString dirPath = QFileDialog::getExistingDirectory(this,"保存",QDir::currentPath());
    if(dirPath.isEmpty()) return;
    bool ok = ui->browserWidget->extractSelectedFiles(dirPath);
    if(!ok){
        QMessageBox::critical(this,"错误",QString("解压文件失败！"));
    }
}
