#ifndef BROWSERWIDGET_H
#define BROWSERWIDGET_H

#include <QWidget>
#include <QFileSystemWatcher>
#include <QTimer>
#include <QCompleter>
#include <QDirModel>


#include "filesystemmodel.h"

namespace Ui {
class BrowserWidget;
}

class BrowserWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BrowserWidget(QWidget *parent = Q_NULLPTR);
    ~BrowserWidget();
    QStringList getSelected() const;
    QString getRootPath() const;

private:
    void CustomizeUI();
    void Connect();
    void SelectFirstRow(bool directoryChanged) const;

signals:
    void switchMe();
    void search();
    void copy();
    void move();
    void del();
    void newFolder();
    void gotFocus();

public slots:
    void toggleDriveMenu();

private slots:
    void open(QModelIndex);
    void handleRootPathChanged(QString);
    void handleSwitchMeRequest();
    void handleGotFocus();
    void goToParent();
    void setPath(QString);
    void setHome();
    void checkPathNotAvailable();
    void populateDriveList();
    void customContextMenuRequested(QPoint);
    void showHiddenFiles(bool);
    void matchDriveToPath(QString);
    void openExplorer(QString);

private:
    Ui::BrowserWidget *ui;
    QFileIconProvider iconProvider;
    FileSystemModel* fileSystemModel;
    QTimer* driveTimer;
    QCompleter* completer;
    QDirModel* dirModel;
};

#endif // BROWSERWIDGET_H
