#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_actionSaveAs_triggered();

    void on_actionAdd_triggered();

    void on_actionUnCompressAs_triggered();

private:
    void Connect();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
