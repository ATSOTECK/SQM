#include "FSDock.h"

#include <QDir>
#include <QVBoxLayout>

#include <QDebug>

FSDock::FSDock(QWidget *parent) :
    QDockWidget(parent),
    _tree(new QTreeView(this)),
    _model(new QFileSystemModel)
{
    _tree->setStyleSheet("QTreeView { background: #252526; }");
    
    setWindowTitle("EXPLORER");
    
    _tree->setModel(_model);
    _tree->setHeaderHidden(true);
    _tree->setColumnHidden(1, true);
    _tree->setColumnHidden(2, true);
    _tree->setColumnHidden(3, true);
    
    connect(_tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
    
    setWidget(_tree);
}

void FSDock::setDir(const QString &dir) {
    QModelIndex index = _model->setRootPath(dir);
    _tree->setRootIndex(index);
}

void FSDock::doubleClicked(QModelIndex idx) {
    QString file = _model->filePath(idx);
    
    emit selected(file);
}
