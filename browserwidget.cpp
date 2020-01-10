#include "browserwidget.h"
#include "ui_browserWidget.h"

#include <QDir>
#include <QDirModel>
#include <QStorageInfo>
#include <QDesktopServices>
#include <QUrl>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QMenu>
#include <QDebug>
#include <QtGlobal>

BrowserWidget::BrowserWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BrowserWidget)
{
    ui->setupUi(this);

    fileSystemModel = new FileSystemModel(this);
    driveTimer = new QTimer(this);
    completer = new QCompleter(this);
    dirModel = new QDirModel(completer);
    dirModel->setFilter(QDir::Dirs | QDir::Drives | QDir::NoDotAndDotDot);
    completer->setModel(dirModel);
    ui->currentPath->setCompleter(completer);

    CustomizeUI();
    Connect();

    driveTimer->setInterval(1000);
    driveTimer->start(1000);
}

BrowserWidget::~BrowserWidget()
{
    delete ui;
}

QStringList BrowserWidget::getSelected() const
{
    QStringList filePaths;
    for (auto index : ui->fileSystemView->selectionModel()->selection().indexes())
    {
        if (index.column() == 0)
        {
            filePaths.push_back(fileSystemModel->fileInfo(index).absoluteFilePath());
        }
    }
    return filePaths;
}

QString BrowserWidget::getRootPath() const
{
    return fileSystemModel->rootPath();
}

void BrowserWidget::CustomizeUI()
{
    populateDriveList();

    const QString osType = QSysInfo::productType();

    ui->homeButton->setIcon(iconProvider.icon(QFileIconProvider::Computer));
    fileSystemModel->setFilter(QDir::AllDirs | QDir::NoDot | QDir::Dirs | QDir::Files);
    ui->fileSystemView->setModel(fileSystemModel);

    if (osType == "osx" || osType == "fedora")
    {
        QString homePath = QDir::homePath();
        ui->currentPath->setText(homePath);
        ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(QDir::homePath()));
    }
    else //windows
    {
        ui->currentPath->setText(ui->driveList->itemText(0));
        ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(ui->driveList->itemText(0)));
    }

    ui->showHiddenFilesButton->setChecked(false);

    setFocusProxy(ui->fileSystemView);
}

void BrowserWidget::Connect()
{
    connect(ui->fileSystemView, &FileSystemView::doubleClicked, this, &BrowserWidget::open);
    connect(ui->fileSystemView, &FileSystemView::switchMe, this, &BrowserWidget::handleSwitchMeRequest);
    connect(ui->fileSystemView, &FileSystemView::gotFocus, this, &BrowserWidget::handleGotFocus);
    connect(ui->fileSystemView, &FileSystemView::goToParent, this, &BrowserWidget::goToParent);
    connect(ui->fileSystemView, &FileSystemView::search, this, &BrowserWidget::search);
    connect(ui->fileSystemView, &FileSystemView::copy, this, &BrowserWidget::copy);
    connect(ui->fileSystemView, &FileSystemView::move, this, &BrowserWidget::move);
    connect(ui->fileSystemView, &FileSystemView::del, this, &BrowserWidget::del);
    connect(ui->fileSystemView, &FileSystemView::newFolder, this, &BrowserWidget::newFolder);
    connect(ui->fileSystemView, &FileSystemView::customContextMenuRequested, this, &BrowserWidget::customContextMenuRequested);

    connect(ui->homeButton, &QToolButton::clicked, this, &BrowserWidget::setHome);
    connect(ui->driveList, QOverload<const QString&>::of(&QComboBox::currentIndexChanged), this, &BrowserWidget::setPath);
    connect(ui->currentPath, &QLineEdit::textChanged, this, &BrowserWidget::setPath);
    connect(ui->showHiddenFilesButton, &QRadioButton::toggled, this, &BrowserWidget::showHiddenFiles);

    connect(fileSystemModel, &FileSystemModel::directoryLoaded, this, &BrowserWidget::handleRootPathChanged);
    connect(fileSystemModel, &FileSystemModel::directoryLoaded, this, &BrowserWidget::matchDriveToPath);

    connect(driveTimer, &QTimer::timeout, this, &BrowserWidget::populateDriveList);
}

void BrowserWidget::SelectFirstRow(const bool directoryChanged) const
{
    if (directoryChanged ||
        ui->fileSystemView->selectionModel()->selection().indexes().count() == 0)
    {
        ui->fileSystemView->clearSelection();
        ui->fileSystemView->selectRow(0);
    }
}

void BrowserWidget::toggleDriveMenu()
{
    ui->driveList->showPopup();
}

void BrowserWidget::open(QModelIndex index)
{
    auto fileInfo = fileSystemModel->fileInfo(index);
    if (fileInfo.isDir())
    {
        if (fileInfo.absoluteFilePath() != "/..")
        {
            ui->fileSystemView->setRootIndex(index/*fileSystemModel->setRootPath(fileInfo.absoluteFilePath())*/);
        }
        else
        {
            ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(""));
        }
    }
    else
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
}

void BrowserWidget::handleRootPathChanged(QString newPath)
{
    ui->currentPath->setText(newPath);
    SelectFirstRow(true);
}

void BrowserWidget::handleSwitchMeRequest()
{
    emit switchMe();
}

void BrowserWidget::handleGotFocus()
{
    SelectFirstRow(false);
    emit gotFocus();
}

void BrowserWidget::goToParent()
{
    QDir currentDir(fileSystemModel->rootPath());
    if (currentDir.cdUp())
    {
        ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(currentDir.path()));
    }
}

void BrowserWidget::setPath(QString path)
{
    ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(path));
}

void BrowserWidget::setHome()
{
    setPath("");
}

void BrowserWidget::checkPathNotAvailable()
{
    if (!QDir(fileSystemModel->rootPath()).exists())
    {
        ui->fileSystemView->setRootIndex(fileSystemModel->setRootPath(QDir::homePath()));
    }
}

void BrowserWidget::populateDriveList()
{
    auto storageInfoList = QStorageInfo::mountedVolumes();

    if (storageInfoList.count() != ui->driveList->count())
    {
        ui->driveList->clear();

        for (auto storageInfo : storageInfoList)
        {
            auto storagePath = storageInfo.rootPath();
            auto icon = iconProvider.icon(QFileInfo(storageInfo.rootPath()));

            ui->driveList->addItem(icon, storagePath);
        }

        checkPathNotAvailable();
    }
}

void BrowserWidget::customContextMenuRequested(QPoint position)
{
    QModelIndex index = ui->fileSystemView->indexAt(position);
    QFileInfo fileInfo;
    QString path;

    if (!index.isValid())
    {
        path = fileSystemModel->rootPath();
        fileInfo = QFileInfo(path);
    }
    else
    {
        fileInfo = fileSystemModel->fileInfo(index);
        path = fileInfo.absoluteFilePath();
    }

    QMenu* menu = new QMenu(this);

    qDebug() << "Menu created: " << menu;

    QAction* action = Q_NULLPTR;

    if (fileInfo.isDir())
    {
        action = new QAction("Open in explorer.", menu);
    }
    else
    {
        action = new QAction("Open file location.", menu);
    }

    menu->addAction(action);
    menu->popup(ui->fileSystemView->viewport()->mapToGlobal(position));

    connect(action, &QAction::triggered, [this, path]{ openExplorer(path); });
    connect(menu, &QMenu::destroyed, [menu](){qDebug() << "Menu destroyed: " << menu; });
}

void BrowserWidget::showHiddenFiles(bool show)
{
    QDir::Filters fileSystemFilter = fileSystemModel->filter();
    QDir::Filters dirFilter = dirModel->filter();

    if (show)
    {
        fileSystemFilter |= (QDir::Hidden | QDir::System);
        dirFilter |= (QDir::Hidden | QDir::System);
    }
    else
    {
        fileSystemFilter &= (~(QDir::Hidden | QDir::System));
        dirFilter &= (~(QDir::Hidden | QDir::System));
    }

    fileSystemModel->setFilter(fileSystemFilter);
    dirModel->setFilter(dirFilter);
}

void BrowserWidget::matchDriveToPath(QString currentPath)
{
    int maxDrivePath = 0;
    int mathcIndex = -1;

    for (auto index = 0; index < ui->driveList->count(); index++)
    {
        QString drivePath = ui->driveList->itemText(index);
        if (currentPath.startsWith(drivePath) && drivePath.size() > maxDrivePath)
        {
            maxDrivePath = drivePath.size();
            mathcIndex = index;
        }
    }

    Q_ASSERT(mathcIndex != -1);
    ui->driveList->setCurrentIndex(mathcIndex);
}

void BrowserWidget::openExplorer(QString path)
{
    QFileInfo fileInfo(path);
    QString pathToOpen = fileInfo.isDir() ? path : fileInfo.dir().absolutePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(pathToOpen));
}
