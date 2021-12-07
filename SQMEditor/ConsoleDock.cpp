#include "ConsoleDock.h"

#include <QScrollBar>
#include <QTextBlock>

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

void ConsoleDock::scrollToBottom() {
    _text->verticalScrollBar()->setValue(_text->verticalScrollBar()->maximum());
}

QString ConsoleDock::line(int index) const {
    if (index >= lineCount()) {
        return QString();
    }
    
    return _text->document()->findBlockByLineNumber(index).text();
}

int ConsoleDock::lineCount() const {
    return _text->document()->blockCount();
}
