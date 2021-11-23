#include "CentralDocument.h"

#include <QAction>
#include <QFile>

Titlebar::Titlebar(const QString &title, QWidget *parent):
    QWidget(parent),
    _title(title),
    _layout(new QHBoxLayout(this)),
    _toolbar(new QToolBar(this)),
    _box(new QComboBox(this))
{
    setFixedHeight(32); //TODO(Skyler): Changeable?
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->addWidget(_toolbar);
    
    QAction *closeAction = new QAction(QIcon("/img/close3.png"), QString(), this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(closeClicked()));
    _toolbar->addAction(closeAction);
    _toolbar->addWidget(_box);
    
    _box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(_box, SIGNAL(activated(QString)), this, SIGNAL(documentSelected(QString)));
    
    _toolbar->setIconSize((QSize(12, 12)));
    _toolbar->setStyleSheet(QString("background: #101010;"));
}

CentralDocument::CentralDocument(QWidget *parent):
    QWidget(parent)
{
    _layout->setContentsMargins(0, 0, 0, 0);
}
