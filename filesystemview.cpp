#include "filesystemview.h"

#include <QKeyEvent>
//#include "filesystemmodel.h"
#include "ZipModel.h"
#include <QHeaderView>
static const QList<int> columnsFactor = { 5, 4, 3, 3 ,5};
static const int totalColumnsFactor = 20;

FileSystemView::FileSystemView(QWidget *parent)
:
    QTableView(parent)
{
    verticalHeader()->hide();
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSortingEnabled(true);
    //sortByColumn(FileSystemModel::eName, Qt::AscendingOrder);
    sortByColumn(ZipModel::NameColumn, Qt::AscendingOrder);

    horizontalHeader()->setStretchLastSection(true);
    horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    setAlternatingRowColors(true);
    setShowGrid(false);
}

void FileSystemView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Tab)
    {
        emit switchMe();
    }
    else if (event->key() == Qt::Key_Return)
    {
        auto index = selectionModel()->currentIndex();
        emit doubleClicked(index);
    }
    else if (event->key() == Qt::Key_Backspace)
    {
        emit goToParent();
    }
    else if (event->key() == Qt::Key_F3)
    {
        emit search();
    }
    else if (event->key() == Qt::Key_F5)
    {
        emit copy();
    }
    else if (event->key() == Qt::Key_F6)
    {
        emit move();
    }
    else if (event->key() == Qt::Key_F8 || event->key() == Qt::Key_Delete)
    {
        emit del();
    }
    else if (event->key() == Qt::Key_F7)
    {
        emit newFolder();
    }
    else
    {
        QTableView::keyPressEvent(event);
    }
}

void FileSystemView::focusInEvent(QFocusEvent *event)
{
    QTableView::focusInEvent(event);
    emit gotFocus();
}

void FileSystemView::resizeEvent(QResizeEvent* /*event*/)
{
    setColumnsWidth();
}

void FileSystemView::setColumnsWidth()
{
    //Q_ASSERT(columnsFactor.size() == FileSystemModel::eColumnCount);
    Q_ASSERT(columnsFactor.size() == ZipModel::ColumnCount);
    for (auto index = 0; index < columnsFactor.size(); index++)
    {
        const int newColumnWidth = columnsFactor[index] * width() / totalColumnsFactor;
        setColumnWidth(index, newColumnWidth);
    }
}
