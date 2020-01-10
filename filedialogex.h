#ifndef QFILEDIALOGEX_H
#define QFILEDIALOGEX_H

#include <QFileDialog>

class FileDialogEx : public QFileDialog
{
    Q_OBJECT
public:
    explicit FileDialogEx(QWidget *parent = Q_NULLPTR,
                         const QString &caption = QString(),
                         const QString &directory = QString(),
                         const QString &filter = QString());

static QStringList getOpenFileAndDirNames(QWidget *parent = Q_NULLPTR,
                                          const QString &caption = QString(),
                                          const QString &dir = QString(),
                                          const QString &filter = QString(),
                                          QString *selectedFilter = Q_NULLPTR,
                                          Options options = Options());
static QList<QUrl> getOpenFileAndDirUrls(QWidget *parent = Q_NULLPTR,
                                         const QString &caption = QString(),
                                         const QUrl &dir = QUrl(),
                                         const QString &filter = QString(),
                                         QString *selectedFilter = Q_NULLPTR,
                                         Options options = Options(),
                                         const QStringList &supportedSchemes = QStringList());
protected:
    FileDialogEx(const QFileDialogArgs &args);
private:
    void dontUseNativeDialog();
signals:

public slots:
    void on_clicked_btn_open();
};

#endif // QFILEDIALOGEX_H
