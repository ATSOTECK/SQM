#include "ConsoleDock.h"

ConsoleDock::ConsoleDock(QWidget *parent) : 
    QDockWidget(parent),
    _text(new QPlainTextEdit(this))
{
    setWindowTitle("OUTPUT");
    
    _text->setReadOnly(true);
    setWidget(_text);
}

void ConsoleDock::clear() {
    _text->clear();
}

void ConsoleDock::setText(const QString &txt) {
    _text->setPlainText(txt);
}

void ConsoleDock::addText(const QString &txt) {
    _text->appendPlainText(txt);
}
