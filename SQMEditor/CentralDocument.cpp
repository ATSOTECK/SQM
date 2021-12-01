#include "CentralDocument.h"

#include <QAction>
#include <QFile>
#include <QPainter>

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

void Titlebar::setName(const QString &name) {
    bool canAdd = true;
    for (int i = 0; i < _box->count(); i++) {
        if (_box->itemText(i) == name) {
            canAdd = false;
        }
    }
    
    if (canAdd) {
        _box->addItem(name);
    }
    
    for (int j = 0; j < _box->count(); j++) {
        if (_box->itemText(j) == name) {
            _box->setCurrentIndex(j);
            return;
        }
    }
}

void Titlebar::closeClicked() {
    emit closeCurrent();
}

void Titlebar::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.fillRect(rect(), QColor(0x10, 0x10, 0x10));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

CentralDocument::CentralDocument(QWidget *parent) :
    QWidget(parent),
    _titlebar(new Titlebar(QString(tr("Script")), 0)),
    _stack(new QStackedWidget(this)),
    _layout(new QVBoxLayout(this)),
    _titlebarAdded(false),
    _titlebarHidden(false)
{
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);
    
    connect(_stack, SIGNAL(currentChanged(int)), this, SLOT(indexChanged(int)));
    connect(_titlebar, SIGNAL(closeCurrentDockument()), this, SLOT(closeCurrentDocumentClicked()));
    connect(_titlebar, SIGNAL(documentSelected(QString)), this, SIGNAL(documentSelected(QString)));
}

void CentralDocument::indexChanged(int index) {
    emit currentChanged(index);
}

void CentralDocument::closeCutrrentClicked() {
    int index = _stack->currentIndex();
    
    _names.removeAt(_stack->currentIndex());
    _stack->removeWidget(_stack->currentWidget());
    
    if (_stack->count() == 0) {
        _titlebar->hide();
        _titlebarHidden = true;
    } else {
        _titlebar->setName(_names.at(_stack->currentIndex()));
    }
    
    emit closeCurrent(index);
}

void CentralDocument::addWidget(QWidget *widget, const QString &name) {
    if (!widget) {
        return;
    }
    
    if (!_titlebarAdded) {
        _layout->addWidget(_titlebar);
        _layout->addWidget(_stack);
        _titlebarAdded = true;
    }
    
    if (_titlebarHidden) {
        _titlebar->show();
        _titlebarHidden = false;
    }
    
    _stack->addWidget(widget);
    _titlebar->setName(name);
    _names.append(name);
}

void CentralDocument::currentIndexSet(int index) {
    _stack->setCurrentIndex(index);
}

int CentralDocument::currentIndexGet() const {
    return _stack->currentIndex();
}

QWidget *CentralDocument::currentWidgetGet() const {
    return _stack->currentWidget();
}

QString CentralDocument::currentWidgetGetName() const {
    return _names.at(_stack->currentIndex());
}

void CentralDocument::setName(const QString &name) {
    _titlebar->setName(name);
}
