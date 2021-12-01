#ifndef CENTRALWIDGET_H
#define CENTRALWIDGET_H

#include "common.h"

#include <QSplitter>
#include <QHBoxLayout>
#include <QWidget>

class CentralWidget : public QWidget {
    Q_OBJECT
    
public:
    CentralWidget(QWidget *parent = 0);
    ~CentralWidget();
    
    void addWidget(QWidget *w);
    void addDocument(QWidget *w);
    
    keep QSplitter *splitter();
    keep QSplitter *documentSplitter();
    
private:
    QSplitter *_splitterArea;
    QSplitter *_documentSplitterArea;
    QSplitter *_consoleSplitterArea;
    QHBoxLayout *_hLayout;
};

#endif // CENTRALWIDGET_H
