#include "CentralWidget.h"

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent)
{
    _hLayout = new QHBoxLayout(this);
    _hLayout->setContentsMargins(0, 0, 0, 0);
    _hLayout->setSpacing(0);
    
    _splitterArea = new QSplitter(Qt::Vertical);
    _hLayout->addWidget(_splitterArea);
    
    _documentSplitterArea = new QSplitter(Qt::Horizontal);
    _splitterArea->addWidget(_documentSplitterArea);
    
    _splitterArea->setFrameStyle(QFrame::NoFrame);
    _documentSplitterArea->setFrameStyle(QFrame::NoFrame);
}

CentralWidget::~CentralWidget() {
    
}

void CentralWidget::addWidget(QWidget *w) {
    _splitterArea->addWidget(w);
}

void CentralWidget::addDocument(QWidget *w) {
    _documentSplitterArea->addWidget(w);
}

QSplitter *CentralWidget::splitter() {
    return _splitterArea;
}

QSplitter *CentralWidget::documentSplitter() {
    return _documentSplitterArea;
}
