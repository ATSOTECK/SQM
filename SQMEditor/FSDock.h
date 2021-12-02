#ifndef FSDOCK_H
#define FSDOCK_H

#include <QDockWidget>
#include <QFileSystemModel>
#include <QTreeView>

class FSDock : public QDockWidget {
    Q_OBJECT
public:
    FSDock(QWidget *parent = nullptr);
    
    void setDir(const QString &dir);
    
public slots:
    void doubleClicked(QModelIndex idx);
    
signals:
    void selected(const QString &file);
    
private:
    QTreeView *_tree;
    QFileSystemModel *_model;
};

#endif // FSDOCK_H
