#include "CodeEditor.h"

#include "AddRemoveText.h"

#include <QtGui>
#include <QCompleter>
#include <QKeyEvent>
#include <QAbstractItemView>
#include <QtDebug>
#include <QApplication>
#include <QModelIndex>
#include <QAbstractItemModel>
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QUndoStack>
#include <QDebug>
#include <QMessageBox>

CodeEditor::CodeEditor(QString name, QWidget *parent):
    QPlainTextEdit(parent),
    _completer(0),
    _name(name),
    _highlighter(new Highlighter(document())),
    _set(false),
    _isModified(false),
    _useTabs(false),
    _undoStack(new QUndoStack(this)),
    _initialSelectionAdded(false)
{
    _cornerWidget = new QWidget(this);
    _cornerWidget->setStyleSheet("background: #32302F;");
    _cornerWidget->setVisible(false);
    
    _lineNumberArea = new LineNumberArea(this);
    
    //_highlighter->setPar(document()); causes pointer error for some reason
    
    _timer = new QTimer(this);
    
    setStyleSheet("QPlainTextEdit { background-color: #32302F; color: #EFE2BA; }");
    document()->setDefaultFont(QFont("Monaco"));
    
    setUpMiniMap();
    
    //Preferences *prefs = Preferences::instance();
    /*
    if (prefs->showTabsSpaces()) {
        QTextOption opt = document()->defaultTextOption();
        opt.setFlags(opt.flags() | QTextOption::ShowTabsAndSpaces);
        document()->setDefaultTextOption(opt);
    }
    
    _useTabs = prefs->useTabs();
    if (prefs->wordWrap()) {
        setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    } else {
        setWordWrapMode(QTextOption::NoWrap);
    }
    */

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect, int)), this, SLOT(updateLineNumberArea(QRect, int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
    //connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightSelectedWord()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(blockOrColumnChanged()));
    connect(this, SIGNAL(textChanged()), this, SLOT(setModified()));
    connect(_undoStack, SIGNAL(cleanChanged(bool)), this, SLOT(cleanStateChanged(bool)));
    
    /*
    connect(prefs, SIGNAL(showCodeEditorMiniMapChanged(bool)), this, SLOT(setUseMiniMap(bool)));
    connect(prefs, SIGNAL(setUseTabsChanged(bool)), this, SLOT(setUseTabs(bool)));
    connect(prefs, SIGNAL(setShowTabsSpacesChanged(bool)), this, SLOT(setShowTabsSpaces(bool)));
    connect(prefs, SIGNAL(setUseWordWrapChanged(bool)), this, SLOT(setUseWordWrap(bool)));
    */
    
    //connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateCorner()));
    //connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateCorner()));
    //connect(verticalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(updateCorner()));
    //connect(horizontalScrollBar(), SIGNAL(rangeChanged(int,int)), this, SLOT(updateCorner()));
    
    connect(verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(updateCorner()));
    connect(horizontalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(updateCorner()));
    
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(repaint()));
    
    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
    //highlightSelectedWord();

    this->setCenterOnScroll(true);
    setAcceptDrops(true);
    
    setMouseTracking(true);
    
    setFrameStyle(QFrame::NoFrame);
    
    _undoStack->setClean();
}

CodeEditor::~CodeEditor() {
    //
}

void CodeEditor::setUpMiniMap() {
    Highlighter *miniHighlighter = nullptr;
    _miniMap = new MiniMap(this, miniHighlighter, this);
    miniHighlighter = new Highlighter(_miniMap->document());
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateMiniMapVisibleArea()));
    connect(_timer, SIGNAL(timeout()), this, SLOT(updateMiniMapText()));
    //connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateMiniMapScrollPos()));
    _miniMap->adjustToParent();
    updateMiniMapText();
    _timer->start(5000);
    _miniMap->show();
}

void CodeEditor::tearDownMiniMap() {
    disconnect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateMiniMapVisibleArea()));
    disconnect(_timer, SIGNAL(timeout()), this, SLOT(updateMiniMapText()));
    delete _miniMap;
    _miniMap = 0;
    _timer->stop();
}

QTextBlock CodeEditor::editorFirstVisibleBlock() {
    return firstVisibleBlock();
}

int CodeEditor::editorFirstVisibleBlockNumber() {
    return firstVisibleBlock().blockNumber();
}

QPointF CodeEditor::editorContentOffset() {
    return contentOffset();
}

QRectF CodeEditor::editorBlockBoundingGeometry(const QTextBlock &block) const {
    return blockBoundingGeometry(block);
}

QString CodeEditor::getName() const {
    return _name;
}

bool CodeEditor::isModified() const {
    return true;
}

int CodeEditor::lineNumberAreaWidth() const {
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space + 18;
}

void CodeEditor::updateLineNumberAreaWidth(int /*newBlockCount*/) {
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy) {
    if (dy) {
        _lineNumberArea->scroll(0, dy);
    } else {
        _lineNumberArea->update(0, rect.y(), rect.width(), rect.height());
    }

    if (rect.contains(viewport()->rect())) {
        updateLineNumberAreaWidth(0);
    }
}

void CodeEditor::resizeEvent(QResizeEvent *e) {
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    _lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    
    if (_miniMap != nullptr) {
        _miniMap->adjustToParent();
    }
}

void CodeEditor::blockOrColumnChanged() {
    int b = textCursor().blockNumber() + 1;
    int t = blockCount();
    int c = textCursor().columnNumber();
    QString statusInfo = QString("%1/%3: %2").arg(b).arg(c).arg(t);
    emit statusInfoChanged(statusInfo);
}

void CodeEditor::highlightCurrentLine() {
    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor("#3C3836");

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        if (_initialSelectionAdded) {
            _extraSelections.replace(0, selection);
        } else {
            _initialSelectionAdded = true;
            _extraSelections.append(selection);
        }
    }

    setExtraSelections(_extraSelections);
}

void CodeEditor::findMatch(QString text, bool next, bool caseSensitive, bool wholeWord, int foo) {
    if (next) {
        moveCursor(QTextCursor::NoMove, QTextCursor::KeepAnchor);
    } else {
        moveCursor(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    }
    
    QTextDocument::FindFlags flags;
    
    if (foo < 5) {
       flags = QTextDocument::FindFlags(foo);
    }
    
    if (caseSensitive) {
        flags |= QTextDocument::FindCaseSensitively;
    }
    
    if (wholeWord) {
        flags |= QTextDocument::FindWholeWords;
    }
    
    bool found = find(text, flags);
    
    if (!found) {
        QTextCursor cursor = textCursor();
        moveCursor(QTextCursor::Start);
        found = find(text, flags);
        if (!found) {
            setTextCursor(cursor);
        }
    } else {
        //highlightSelectedWord(text);
    }
}

QUndoStack *CodeEditor::undoStack() const {
    return _undoStack;
}

void CodeEditor::highlightSelectedWord(QString txt) {
    QString word = textUnderCursor();
    if (!txt.isEmpty()) {
        word = txt;
    }
    if (word.isEmpty())
        return;
    if (word != _selectedText) {
        _selectedText = word;
        _highlighter->setSelectedWord(word);
        QList<int> lines;
        int pos = 0;
        int wordLength = word.length();
        QTextCursor cursor = document()->find(word, pos, QTextDocument::FindCaseSensitively);
        while (cursor.position() != -1) {
            int t = cursor.blockNumber();
            lines.append(t);
            pos = cursor.position() + wordLength;
            cursor = document()->find(word, pos, QTextDocument::FindCaseSensitively);
        }
        int size = lines.size();
        for (int  i = 0; i < size; ++i) {
            _highlighter->rehighlightLines(lines.at(i));
        }
    }
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event) {
    int pageBottom = this->viewport()->height();
    QTextBlock currentBlock = this->document()->findBlock(this->textCursor().position());

    QPainter painter(_lineNumberArea);
    painter.setFont(QFont("Monaco"));
    painter.fillRect(_lineNumberArea->rect(), QColor("#32302F"));

    QTextBlock block = firstVisibleBlock();
    QPointF viewportOffset = contentOffset();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    
    bool current = false;
    
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block == currentBlock) {
            current = true;
            /*
            int y = blockBoundingGeometry(currentBlock).translated(contentOffset()).top();
            int h = blockBoundingGeometry(currentBlock).height();
            int x = _lineNumberArea->rect().x();
            int w = _lineNumberArea->width();
            */
            //QRect rec(x, y - 1, w, h + 1);
            //painter.fillRect(rec, QColor(Qt::darkGray).darker(380));//450
            painter.setPen(QColor(Qt::white).darker(115));
        } else {
            painter.setPen(QColor(Qt::white).darker(180));
        }
        
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            
            painter.drawText(-18, top, _lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

    //code folding
    _foldArea = 15;
    int xofs = _lineNumberArea->width() - _foldArea;
    QRect r(xofs, 0, _foldArea, _lineNumberArea->height());
    painter.fillRect(r, QColor("#32302F"));
    
    if (current) {
        /*
        int y = blockBoundingGeometry(currentBlock).translated(contentOffset()).top();
        int h = blockBoundingGeometry(currentBlock).height();
        int x = _lineNumberArea->width() - _foldArea;
        int w = _lineNumberArea->width();
        */
        //QRect rec(x, y - 1, w, h + 1);
        //painter.fillRect(rec, QColor(Qt::darkGray).darker(380));//380
    }
    
    if (_foldArea != _rightArrowIcon.width()) {
        QPolygonF polygon;// = new QPolygonF();

        _rightArrowIcon = QPixmap(_foldArea, _foldArea);
        _rightArrowIcon.fill(Qt::transparent);
        _downArrowIcon = QPixmap(_foldArea, _foldArea);
        _downArrowIcon.fill(Qt::transparent);

        polygon.append(QPointF(_foldArea * 0.4, _foldArea * 0.25));
        polygon.append(QPointF(_foldArea * 0.4, _foldArea * 0.75));
        polygon.append(QPointF(_foldArea * 0.8, _foldArea * 0.5));
        QPainter iconPainter(&_rightArrowIcon);
        iconPainter.setRenderHint(QPainter::Antialiasing);
        iconPainter.setPen(Qt::NoPen);
        iconPainter.setBrush(QColor(Qt::lightGray).darker(150));
        iconPainter.drawPolygon(polygon);

        polygon.clear();
        polygon.append(QPointF(_foldArea * 0.25, _foldArea * 0.4));
        polygon.append(QPointF(_foldArea * 0.75, _foldArea * 0.4));
        polygon.append(QPointF(_foldArea * 0.5, _foldArea * 0.8));
        QPainter iconPainterd(&_downArrowIcon);
        iconPainterd.setRenderHint(QPainter::Antialiasing);
        iconPainterd.setPen(Qt::NoPen);
        iconPainterd.setBrush(QColor(Qt::lightGray).darker(150));
        iconPainterd.drawPolygon(polygon);
    }

    block = firstVisibleBlock();
    while (block.isValid()) {
        if (!block.isVisible()) {
            block = block.next();
            continue;
        }
        
        QPointF position = this->blockBoundingGeometry(block).topLeft() + viewportOffset;

        if (position.y() > pageBottom) {
            break;
        }

        //painter.drawPixmap(xofs, qRound(position.y()), _downArrowIcon);
        //painter.drawPixmap(xofs, qRound(position.y()), _rightArrowIcon);
        QString txt = block.text();
        if ((txt.contains("{") || (txt.contains("if") && txt.contains("{") && !txt.contains("elseif"))) && !insideComment(block.blockNumber())) {
            if (!_foldedLines.contains(block.blockNumber())) {
                if (block.isVisible()) {
                    painter.drawPixmap(xofs, qRound(position.y()), _downArrowIcon);
                }
            }
        } 
        
        if (txt.contains("/*")) {
            if (!_foldedLines.contains(block.blockNumber())) {
                if (block.isVisible()) {
                    painter.drawPixmap(xofs, qRound(position.y()), _downArrowIcon);
                }
            }
        } 
        
        if (_foldedLines.contains(block.blockNumber()) && block.isVisible()) {
            painter.drawPixmap(xofs, qRound(position.y()), _rightArrowIcon);
        }
        
        if (txt.contains("//") && txt.contains("TODO")) {
            QLinearGradient linearGradient = QLinearGradient(xofs, qRound(position.y()), xofs + _foldArea, qRound(position.y()) + _foldArea);
            linearGradient.setColorAt(0, QColor(13, 62, 243).lighter(120));
            linearGradient.setColorAt(1, QColor(5, 27, 106).lighter(120));
            painter.setRenderHints(painter.Antialiasing, true);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(linearGradient));
            painter.drawEllipse(xofs + 1, qRound(position.y()), _foldArea - 2, _foldArea - 1);
        }
        
        if (txt.contains("//") && txt.contains("LINK")) {
            QLinearGradient linearGradient = QLinearGradient(xofs, qRound(position.y()), xofs + _foldArea, qRound(position.y()) + _foldArea);
            linearGradient.setColorAt(0, QColor(13, 62, 243).lighter(120));
            linearGradient.setColorAt(1, QColor(5, 27, 106).lighter(120));
            painter.setRenderHints(painter.Antialiasing, true);
            painter.setPen(Qt::NoPen);
            painter.setBrush(QBrush(linearGradient));
            painter.drawEllipse(xofs + 1, qRound(position.y()), _foldArea - 2, _foldArea - 1);
        }

        block = block.next();
    }

    painter.end();
    QWidget::paintEvent(event);
}

void CodeEditor::paintEvent(QPaintEvent *e) {
    QPlainTextEdit::paintEvent(e);
    
    int pageBottom = this->viewport()->height();
    QPainter painter(viewport());
    painter.setPen(QPen("black"));
    QTextBlock block = firstVisibleBlock();
    QPointF viewportOffset = contentOffset();
    
    QFont font = currentCharFormat().font();
    int charWidth = QFontMetrics(font).averageCharWidth();
    int px = (charWidth * textCursor().positionInBlock()) 
            + contentOffset().x() 
            + document()->documentMargin();
    
    px = cursorRect(textCursor()).x();
    int py = cursorRect(textCursor()).y();
    
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    if (cursor.selectedText() == "isKeyDown") {
        QString hint = "bool isKeyDown(int key)\nChecks if the given key is down.";
        painter.setOpacity(0.75);
        QFontMetrics met(this->font());
        int width = met.boundingRect(viewport()->rect(), Qt::AlignLeft | Qt::AlignVCenter, hint).width();
        int height = met.boundingRect(viewport()->rect(), Qt::AlignLeft | Qt::AlignVCenter, hint).height();
        QRect rekt(px, py + 16, width + 16, height + 4);
        painter.fillRect(rekt, Qt::darkCyan);
        painter.setPen(QPen(Qt::cyan));
        painter.drawRect(rekt);
        painter.setOpacity(1.0);
        rekt = QRect(px + 8, py + 18, width, height);
        painter.drawText(rekt, Qt::AlignLeft | Qt::AlignVCenter, hint);
    } else if (textLeftOfCursor(true).contains("isPressed") || cursor.selectedText() == "isPressed") {
        QString hint = "bool isPressed(int keyOrButton)\nChecks if a key or mouse button is pressed\nOnly returns true on press.";
        painter.setOpacity(0.75);
        QFontMetrics met(this->font());
        int width = met.boundingRect(viewport()->rect(), Qt::AlignLeft | Qt::AlignVCenter, hint).width();
        int height = met.boundingRect(viewport()->rect(), Qt::AlignLeft | Qt::AlignVCenter, hint).height();
        QRect rekt(px, py + 16, width + 16, height + 4);
        painter.fillRect(rekt, Qt::black);
        painter.setPen(QPen(Qt::white));
        painter.drawRect(rekt);
        painter.setOpacity(1.0);
        rekt = QRect(px + 8, py + 18, width, height);
        painter.drawText(rekt, Qt::AlignLeft | Qt::AlignVCenter, hint);
    }
}

void CodeEditor::updateMiniMapText() {
    if (_miniMap == nullptr) {
        return;
    }
    
    if (_set) {
        return;
    }
    
    //if (!_set) {
        //_set = true;
    //}

    _miniMap->setCode(toPlainText());
    //_miniMap->setTextCursor(textCursor());
    _miniMap->updateVisibleArea();
}

void CodeEditor::updateMiniMapVisibleArea() {
    if (_miniMap == nullptr) {
        return;
    }

    _miniMap->updateVisibleArea();
}

void CodeEditor::updateMiniMapScrollPos() {
    if (_miniMap == nullptr) {
        return;
    }

    _miniMap->verticalScrollBar()->setValue(verticalScrollBar()->value());
}

void CodeEditor::updateCorner() {
    if (verticalScrollBar()->isVisible() && horizontalScrollBar()->isVisible()) {
        setCornerWidget(_cornerWidget);
    } else {
        setCornerWidget(0);
    }
}

void CodeEditor::wheelEvent(QWheelEvent *e) {
    QPlainTextEdit::wheelEvent(e);
    /*
    float steps = e->delta() / 8;
    if (e->orientation() == Qt::Vertical) {
        float s = steps;
        if (fabs(steps) > 20) {
            steps = 20;
            if (s < 0) {
                steps *= -1;
            }
        }
        
        if (fabs(steps) > 0 && fabs(steps) < 1) {
            steps = 1;
            if (s < 0) {
                steps *= -1;
            }
        }
        verticalScrollBar()->setValue(verticalScrollBar()->value() - steps);
        if (_miniMap)
            _miniMap->verticalScrollBar()->setValue(verticalScrollBar()->value() - steps);
    } else if (e->orientation() == Qt::Horizontal) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - e->delta());
    }
    */
}

void CodeEditor::lineNumberAreaMousePressEvent(QMouseEvent *e) {
    QPlainTextEdit::mousePressEvent(e);
}

void CodeEditor::lineNumberAreaMouseMoveEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseMoveEvent(e);
}

void CodeEditor::lineNumberAreaWheelEvent(QWheelEvent *e) {
    wheelEvent(e);
}

void CodeEditor::setCompleter(QCompleter *completer) {
    if (_completer != nullptr) {
        QObject::disconnect(_completer, 0, this, 0);
    }

    _completer = completer;

    if (_completer == nullptr) {
        return;
    }

    _completer->setWidget(this);
    _completer->setCompletionMode(QCompleter::PopupCompletion);
    _completer->setCaseSensitivity(Qt::CaseInsensitive);
    QObject::connect(_completer, SIGNAL(activated(QString)), this, SLOT(insertCompletion(QString)));
}

QCompleter *CodeEditor::completer() const {
    return _completer;
}

void CodeEditor::insertCompletion(const QString &completion) {
    if (_completer->widget() != this) {
        return;
    }

    QTextCursor tc = textCursor();
    int extra = completion.length() - _completer->completionPrefix().length();
    tc.movePosition(QTextCursor::Left);
    tc.movePosition(QTextCursor::EndOfWord);
    tc.insertText(completion.right(extra));
    setTextCursor(tc);
}

QString CodeEditor::textUnderCursor() const {
    QTextCursor tc = textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

QString CodeEditor::textLeftOfCursor(QTextCursor tc, bool start) const {
    if (start) {
        while (!tc.atBlockStart()) {
            tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        }
        return tc.selectedText();
    } else {
        tc.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        return tc.selectedText();
    }
}

QString CodeEditor::textRightOfCursor(QTextCursor tc, bool end) const {
    if (end) {
        while (!tc.atBlockEnd()) {
            tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        return tc.selectedText();
    } else {
        tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        return tc.selectedText();
    }
}

QString CodeEditor::textLeftOfCursor(QTextCursor *tc, bool start) const {
    if (start) {
        while (!tc->atBlockStart()) {
            tc->movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        }
        return tc->selectedText();
    } else {
        tc->movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        return tc->selectedText();
    }
}

QString CodeEditor::textRightOfCursor(QTextCursor *tc, bool end) const {
    if (end) {
        while (!tc->atBlockEnd()) {
            tc->movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        }
        return tc->selectedText();
    } else {
        tc->movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        return tc->selectedText();
    }
}

QString CodeEditor::textLeftOfCursor(bool start) const {
    return textLeftOfCursor(textCursor(), start);
}

QString CodeEditor::textRightOfCursor(bool end) const {
    return textRightOfCursor(textCursor(), end);
}

void CodeEditor::focusInEvent(QFocusEvent *e) {
    if (_completer != nullptr) {
        _completer->setWidget(this);
    }

    QPlainTextEdit::focusInEvent(e);
}

void CodeEditor::keyPressEvent(QKeyEvent *e) {
    if (_completer != nullptr && _completer->popup()->isVisible()) {
        switch (e->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
        case Qt::Key_Escape:
        //case Qt::Key_Tab:
        case Qt::Key_Backtab:
            e->ignore();
            return;
        default:
            break;
        }
    }
    
    _selectedText = textCursor().selectedText();
    
    switch (e->key()) {
    case Qt::Key_Apostrophe:
    case Qt::Key_QuoteDbl:
        //TODO: check settings
        completeQuotes(e->text());
        return;
    case Qt::Key_BraceLeft:
    case Qt::Key_BracketLeft:
    case Qt::Key_ParenLeft:
    case Qt::Key_BraceRight:
    case Qt::Key_BracketRight:
    case Qt::Key_ParenRight:
        completeBraces(e->text());
        return;
    case Qt::Key_Tab:
        insertIndentation();
        return;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        autoCompleteEnter();
        return;
    case Qt::Key_Backspace:
        autoCompleteBackspace();
        return;
    case Qt::Key_L:
        autoCompleteL(e->text());
        return;
    default:
        break;
    }

    bool isShortcut = ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_E); //command+E or control+E
    if (_completer == nullptr || !isShortcut) {
        QPlainTextEdit::keyPressEvent(e);
        //_miniMap->textCursor().setPosition(textCursor().position());
        //_miniMap->textCursor().insertText(e->text());
        //_miniMap->keyEvent(e);
    }
    
    if (Qt::ControlModifier && e->key() == Qt::Key_V) {
        updateMiniMapText();
    }
    
    if (e->key() == Qt::Key_Tab && _completer != nullptr && _completer->popup()->isVisible()) {
        _completer->popup()->hide();
    }

    const bool ctrlOrShift = e->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
    if (_completer == nullptr || (ctrlOrShift && e->text().isEmpty())) {
        return;
    }

    static QString eow("~!@#$%^&*()_+{}|:\"<>?,./;'[]\\-="); // end of word
    bool hasModifier = (e->modifiers() != Qt::NoModifier) && !ctrlOrShift;
    QString completionPrefix = textUnderCursor();
    
    if (!isShortcut && (hasModifier || e->text().isEmpty()|| completionPrefix.length() < 3 || eow.contains(e->text().right(1)))) {
        _completer->popup()->hide();
        return;
    }
    
    if (completionPrefix != _completer->completionPrefix()) {
        _completer->setCompletionPrefix(completionPrefix);
        _completer->popup()->setCurrentIndex(_completer->completionModel()->index(0, 0));
    }
    
    QRect cr = cursorRect();
    cr.setWidth(_completer->popup()->sizeHintForColumn(0) + _completer->popup()->verticalScrollBar()->sizeHint().width());
    _completer->complete(cr);
}

void CodeEditor::mouseMoveEvent(QMouseEvent *e) {
    QPoint pos = e->pos();
    QTextCursor cursor = cursorForPosition(pos);
    
    highlightJumpToDefinition(cursor, e->modifiers() & Qt::ControlModifier);
    
    QPlainTextEdit::mouseMoveEvent(e);
}

void CodeEditor::mousePressEvent(QMouseEvent *e) {
    QPlainTextEdit::mousePressEvent(e);
}

void CodeEditor::mouseReleaseEvent(QMouseEvent *e) {
    QPlainTextEdit::mouseReleaseEvent(e);
    
    if (e->button() == Qt::LeftButton) {
        //highlightSelectedWord();
    }
}

void CodeEditor::completeBraces(QString text) {
    QString t;
    QString txt = textCursor().block().text();
    
    bool insert = true;
    
    if (text == "{") {
        t = "}";
    } else if (text == "[" && !txt.contains("--")) {
        t = "]";
    } else if (text == "(") {
        t = ")";
    } else if (text == "}") {
        if (textRightOfCursor() == "}") {
            moveCursor(QTextCursor::Right);
            insert = false;
            return;
        }
    } else if (text == "]") {
        if (textRightOfCursor() == "]") {
            moveCursor(QTextCursor::Right);
            insert = false;
            return;
        }
    } else if (text == ")") {
        if (textRightOfCursor() == ")") {
            moveCursor(QTextCursor::Right);
            insert = false;
            return;
        }
    }
    
    if (t.isEmpty() && insert) {
        textCursor().insertText(text);
        return;
    }
    
    if (_selectedText.length() > 0) {
        textCursor().insertText(text);        
        textCursor().insertText(_selectedText);
        textCursor().insertText(t);
        moveCursor(QTextCursor::Left);
    } else {
        textCursor().insertText(text);        
        textCursor().insertText(t);
        moveCursor(QTextCursor::Left);
    }
}

void CodeEditor::completeQuotes(QString text) {
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    
    if (textLeftOfCursor() == ")") {
        textCursor().insertText(";");
        return;
    }
    
    if (textCursor().block().text().endsWith("return") || textCursor().block().text().endsWith("true") || textCursor().block().text().endsWith("false") 
            || textCursor().block().text().endsWith("NULL")) {
        textCursor().insertText(";");
        return;
    }
    
    if (text == "\"") {
        if (textRightOfCursor() == "\"" && textLeftOfCursor() != "\\") {
            moveCursor(QTextCursor::Right);
            return;
        }
    } else if (text == "'") {
        if (textRightOfCursor() == "'" && textLeftOfCursor() != "\\") {
            moveCursor(QTextCursor::Right);
            return;
        }
    }
    
    if (textLeftOfCursor() == "\\") {
        textCursor().insertText(text);
        return;
    }
    
    if (_selectedText.length() > 0) {
        textCursor().insertText(text);
        textCursor().insertText(_selectedText);
        textCursor().insertText(text);
        moveCursor(QTextCursor::Left);
        //textCursor().select(QTextCursor::WordUnderCursor);
    } else {
        textCursor().insertText(text);
        textCursor().insertText(text);
        moveCursor(QTextCursor::Left);
    }
}

void CodeEditor::autoCompleteL(QString txt) {
    if (textLeftOfCursor() == ")") {
        textCursor().insertText(";");
        return;
    }
    
    if (textCursor().block().text().endsWith("return") || textCursor().block().text().endsWith("true") || textCursor().block().text().endsWith("false") 
            || textCursor().block().text().endsWith("NULL") || textCursor().block().text().endsWith("\"") || textCursor().block().text().endsWith("'")) {
        textCursor().insertText(";");
        return;
    }
    
    textCursor().insertText(txt);
}

void CodeEditor::insertIndentation() {
    if (textCursor().hasSelection()) {
        indentMore();
    }
    
    if (!_useTabs) {
        for (int i = 0; i < 4; ++i) {
            textCursor().insertText(" ");
        }
    } else {
        textCursor().insertText("\t");
    }
}

void CodeEditor::indentMore() {
    QTextCursor cursor = textCursor();
    QTextBlock block = document()->findBlock(cursor.selectionStart());
    QTextBlock end = document()->findBlock(cursor.selectionStart()).next();
    
    cursor.beginEditBlock();
    
    cursor.setPosition(block.position());
    while (block != end) {
        cursor.setPosition(block.position());
        if (!_useTabs) {
            for (int i = 0; i < 4; ++i) {
                textCursor().insertText(" ");
            }
        } else {
            textCursor().insertText("\t");
        }
        block= block.next();
    }
    cursor.endEditBlock();
}

void CodeEditor::autoCompleteEnter() {
    bool autoIndent = false, addEnd = false, 
            addClosingCurlyBrace = false, 
            addClosingParen = false, 
            addClosingBracket = false,
            ch = false;
    QString str = textCursor().block().text();
    QString strl = textLeftOfCursor(true);
    QString indentation = getIndentation(str);
    QString tab = "";
    if (_useTabs) {
        tab = "\t";
    } else {
        tab = "    ";
    }
    
    //TODO: finish checking for other position in the line
    if (textCursor().atBlockEnd()) {
        if (str.contains("function") && (str.endsWith(')') || str.endsWith(") "))) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (str.contains("if") && str.contains("then") && (str.endsWith('n') || str.endsWith("n "))) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (str.contains("else") && (str.endsWith('e') || str.endsWith("e "))) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (str.contains("do") && (str.endsWith('o') || str.endsWith("o "))) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } else if (str.endsWith('{') || str.endsWith("{ ")) {
            addClosingCurlyBrace = true;
        } else if (str.endsWith('(') || str.endsWith("( ")) {
            addClosingParen = true;
        } else if (str.endsWith('[') || str.endsWith("[ ")) {
            addClosingBracket = true;
        }
    } else {
        if (strl.contains("function")) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (strl.contains("if") && strl.contains("then")) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (strl.contains("else")) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        } if (strl.contains("do")) {
            if (checkNextLine(indentation)) {
                addEnd = true;
            } else {
                autoIndent = true;
            }
        }
    }
    
    if (str.contains("{}") && str.endsWith("}") && textLeftOfCursor() == "{") {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation);
        moveCursor(QTextCursor::Up);
        ch = true;
    }
    
    if (str.contains("()") && str.endsWith(")") && textLeftOfCursor() == "(") {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation);
        moveCursor(QTextCursor::Up);
        ch = true;
    }
    
    if (str.contains("[]") && str.endsWith("]") && textLeftOfCursor() == "[") {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation);
        moveCursor(QTextCursor::Up);
        ch = true;
    }
    
    textCursor().insertText("\n");
    
    if (autoIndent) {
        textCursor().insertText(indentation);
        insertIndentation();
    }
    
    if (addEnd) {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation + "end");
        moveCursor(QTextCursor::Up);
    }
    
    if (addClosingCurlyBrace) {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation + "}");
        moveCursor(QTextCursor::Up);
    }
    
    if (addClosingParen) {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation + ")");
        moveCursor(QTextCursor::Up);
    }
    
    if (addClosingBracket) {
        textCursor().insertText(tab + "\n");
        textCursor().insertText(indentation + "]");
        moveCursor(QTextCursor::Up);
    }
    
    if (indentation.length() > 0 && !autoIndent) {
        textCursor().insertText(indentation);
    }
    
    if (ch) {
        for (int i = 0; i < 4; ++ i) {
            moveCursor(QTextCursor::NextCharacter);
        }
    }
}

void CodeEditor::autoCompleteBackspace() {
    if (isInBPBQ()) {
        moveCursor(QTextCursor::Right);
        textCursor().deletePreviousChar();
        textCursor().deletePreviousChar();
        return;
    }
    
    textCursor().deletePreviousChar();
}

bool CodeEditor::isInBPBQ() {
    if (textLeftOfCursor() == "(" && textRightOfCursor() == ")") {
        return true;
    }
    
    if (textLeftOfCursor() == "{" && textRightOfCursor() == "}") {
        return true;
    }
    
    if (textLeftOfCursor() == "[" && textRightOfCursor() == "]") {
        return true;
    }
    
    if (textLeftOfCursor() == "\"" && textRightOfCursor() == "\"") {
        return true;
    }
    
    if (textLeftOfCursor() == "'" && textRightOfCursor() == "'") {
        return true;
    }
    
    return false;
}

bool CodeEditor::checkNextLine(QString &txt) {
    //true add line and end
    //false auto indent
    //txt is current line indentation
    QTextCursor cursor = textCursor();
    cursor.movePosition(QTextCursor::NextBlock);
    QString temp = cursor.block().text();
    if (temp.isEmpty()) {
        return true;
    }
    
    QString tempIndentation = getIndentation(temp);
    if (tempIndentation == txt) {
        return true;
    }
    
    if (_useTabs) {
        if (txt + '\t' == tempIndentation) {
            return false;
        }
    } else {
        if (txt + "    " == tempIndentation) {
            return false;
        }
    }
    
    return false;
}

void CodeEditor::highlightJumpToDefinition(QTextCursor &cursor, bool pressed) {
    //TODO: add function to find variables, make string table
    //build var table
    
    if (pressed) {
        cursor.select(QTextCursor::WordUnderCursor);
        cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
        QString txt = cursor.selectedText();
        if (txt.isEmpty())
            return;
        
        if (txt.endsWith("(") || txt.endsWith(".") || txt.endsWith(":")) {
            cursor.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            QTextEdit::ExtraSelection selection;
            QColor lineColor = QColor(25, 184, 230);
            selection.format.setForeground(lineColor);
            selection.format.setFontUnderline(true);
            selection.cursor = cursor;
            if (_extraSelections.count() == 2) {
                _extraSelections.replace(1, selection);
            } else  {
                _extraSelections.append(selection);
            }
        } else {
            _extraSelections.removeAt(1);
        }
    } else {
        _extraSelections.removeAt(1);
    }
    
    setExtraSelections(_extraSelections);
}

void CodeEditor::copyLine() {
    QTextCursor c = textCursor();
    c.movePosition(QTextCursor::StartOfLine);
    QString tmp = textRightOfCursor(&c);
    c.clearSelection();
    
    while (tmp == " " || tmp == "\t") {
        tmp = textRightOfCursor(&c);
        c.clearSelection();
    }
    
    c.movePosition(QTextCursor::Left);
    textRightOfCursor(&c, true);
    QApplication::clipboard()->setText(c.selectedText());
}

void CodeEditor::gotoDefinition(QTextCursor *cursor) {
    if (!cursor) {
        *cursor = textCursor();
    }
    
    cursor->select(QTextCursor::WordUnderCursor);
    cursor->movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
    if (cursor->selectedText().endsWith("(")) {
        cursor->movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        emit locateFunction(cursor->selectedText(), false);
    } else if (cursor->selectedText().endsWith(".") || cursor->selectedText().endsWith(":")) {
        cursor->movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        emit locateFunction(cursor->selectedText(), true);
    } else {
        emit locateFunction(cursor->selectedText(), true);
    }
}

QString CodeEditor::getIndentation(const QString &text) {
    QString indentation = "";
    int spaceCount = 0;
    for (int i = 0; i < text.length(); ++i) {
        QChar ch = text.at(i);
        
        if (_useTabs) {
            if (spaceCount % 4 == 0 && spaceCount > 0) {
                indentation += "\t";
            }
        }
        
        if (ch == '\t') {
            indentation += "\t";
        } else if (ch == ' ') {
            if (_useTabs) {
                spaceCount++;
            } else {
                indentation += " ";
            }
        } else {
            break;
        }
    }
    
    //qDebug().nospace() << "i" << indentation << indentation.length();
    
    return indentation;
}

QChar CodeEditor::lastNonWhitespaceChar(QString str) {
    for (int i = str.length() - 1; i > 0; i--) {
        QChar c = str.at(i);
        if (c != ' ' && c != '\t' && c != '\n') {
            return c;
        }
    }
    
    return '\0';
}

QString CodeEditor::reverseSelectTextPortionFromOffset(int begin, int end) {
    QTextCursor cur = textCursor();
    int curPos = cur.position();
    cur.setPosition(curPos + begin);
    while ((cur.position() == curPos) && begin > 0) {
        begin -= 1;
        cur.setPosition(curPos + begin);
    }
    cur.setPosition(curPos - end, QTextCursor::KeepAnchor);
    QString text = cur.selectedText();
    return text;
}

void CodeEditor::buildVarTable() {
    QTextCursor cursor;
    cursor.setPosition(0);
    
    while (!cursor.atEnd()) {
        if (textRightOfCursor(cursor) != " ") {
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor);
            continue;
        } else {
            QString txt = textRightOfCursor(cursor);
            cursor.movePosition(QTextCursor::Right);
        }
        
        cursor.movePosition(QTextCursor::Right);
    }
}

bool CodeEditor::insideComment(int lineNumber) {
    QTextBlock startBlock;
    if (lineNumber > 0) {
        startBlock = document()->findBlockByLineNumber(lineNumber - 1);
    } else {
        startBlock = document()->findBlockByLineNumber(lineNumber);
    }
    
    QTextBlock blk = startBlock;
    blk = blk.next();
    
    QString txt = blk.text();
    if (txt.contains("//")) {
        return true;
    }
    
    bool foundTop = false;
    bool foundBottom = false;
    bool inside = false;
    
    blk = blk.previous();
    while (blk.isValid() && !foundTop) {
        txt = blk.text();
        if (txt.contains("/*")) {
            foundTop = true;
        }
        
        blk = blk.previous();
    }
    
    blk = startBlock;
    blk = blk.next();
    
    blk = blk.next();
    while (blk.isValid() && !foundBottom) {
        txt = blk.text();
        if (txt.contains("*/") && foundTop) {
            foundBottom = true;
        }
        
        blk = blk.next();
    }
    
    if (foundTop && foundBottom) {
        inside = true;
    }
    
    return inside;
}

void CodeEditor::findText() {

}

bool CodeEditor::saveToFile(QString &path) {
    QString fileName = path + '/' + _name;
    if (!fileName.contains(".zs")) {
        fileName += ".zs";
    }
    
    //qDebug() << "script filename" << fileName;
    
    QFile file(fileName);
    if (file.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream out(&file);
        
        out << toPlainText();
        
        file.flush();
        file.close();
        
        _undoStack->setClean();
        
        return true;
    }
    
    return false;
}

bool CodeEditor::openFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "Error", file.errorString());
        return false;
    }
    
    QTextStream in(&file);
    setPlainText(in.readAll());
    
    file.close();
    
    if (_miniMap != nullptr) {
        updateMiniMapText();
    }
    
    _undoStack->setClean();
    
    return true;
}

void CodeEditor::setModified() {
    _isModified = true;
    _undoStack->push(new AddText());
}

void CodeEditor::setUseMiniMap(bool mini) {
    if (mini) {
        setUpMiniMap();
    } else {
        tearDownMiniMap();
    }
}

void CodeEditor::setUseTabs(bool tabs) {
    _useTabs = tabs;
}

void CodeEditor::setShowTabsSpaces(bool tabsSpaces) {
    if (tabsSpaces) {
        QTextOption opt = document()->defaultTextOption();
        opt.setFlags(opt.flags() | QTextOption::ShowTabsAndSpaces);
        document()->setDefaultTextOption(opt);
    } else {
        QTextOption opt = document()->defaultTextOption();
        opt.setFlags(0x0);
        document()->setDefaultTextOption(opt);
    }
}

void CodeEditor::setUseWordWrap(bool wrap) {
    if (wrap) {
        setWordWrapMode(QTextOption::WordWrap);
    } else {
        setWordWrapMode(QTextOption::NoWrap);
    }
}

void CodeEditor::cleanStateChanged(bool state) {
    emit modifiedChanged(state);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void LineNumberArea::mousePressEvent(QMouseEvent *e) {
    int xofs = width() - _codeEditor->_foldArea;
    int fh = _codeEditor->fontMetrics().lineSpacing();
    float ys = e->pos().y();
    //int lineNumber = 0;
    if (e->pos().x() > xofs) {
        QTextBlock block = _codeEditor->editorFirstVisibleBlock();
        int pageBottom = _codeEditor->viewport()->height();
        QPointF viewportOffset = _codeEditor->editorContentOffset();

        while (block.isValid()) {
            QPointF position = _codeEditor->editorBlockBoundingGeometry(block).topLeft() + viewportOffset;
            if (position.y() > pageBottom) {
                break;
            }

            QString txt = block.text();
            int line = block.blockNumber();
            
            if (position.y() < ys && (position.y() + fh) > ys && e->button() == Qt::LeftButton) {
                
                if ((txt.contains("{") || (txt.contains("if") && txt.contains("then"))) && !txt.contains("elseif")  && !_codeEditor->insideComment(line)) {
                    if (!_codeEditor->_foldedLines.contains(block.blockNumber())) {
                        fold(block.blockNumber());
                    } else {
                        unfold(block.blockNumber());
                    }
                    break;
                }
                
                if (txt.contains("/*")) {
                    if (!_codeEditor->_foldedLines.contains(block.blockNumber())) {
                        fold(block.blockNumber(), true);
                    } else {
                        unfold(block.blockNumber(), true);
                    }
                    break;
                }
                
                
                if (txt.contains("//") && txt.contains("TODO"))
                    break;
                
                if (_codeEditor->_foldedLines.contains(line)) {
                    break;
                }
                
                update();
                break;
            } else if (position.y() < ys && (position.y() + fh) > ys && e->button() == Qt::RightButton) {
                
                if ((txt.contains("{") || (txt.contains("if") && txt.contains("{"))) && !txt.contains("elseif")  && !_codeEditor->insideComment(line)) {
                    if (!_codeEditor->_foldedLines.contains(block.blockNumber())) {
                        fold(block.blockNumber());
                    } else {
                        unfold(block.blockNumber());
                    }
                    break;
                }
                
                if (txt.contains("/*")) {
                    if (!_codeEditor->_foldedLines.contains(block.blockNumber())) {
                        fold(block.blockNumber(), true);
                    } else {
                        unfold(block.blockNumber(), true);
                    }
                    break;
                }
                
                if (txt.contains("//") && txt.contains("TODO"))
                    break;
                
                if (_codeEditor->_foldedLines.contains(line)) {
                    break;
                }
                
                update();
                break;
            }
            block = block.next();
        }
    }
    if (e->pos().x() < xofs) {
        _codeEditor->lineNumberAreaMousePressEvent(e);
    }
}

void LineNumberArea::mouseMoveEvent(QMouseEvent *e) {
    _codeEditor->lineNumberAreaMouseMoveEvent(e);
}

void LineNumberArea::wheelEvent(QWheelEvent *e) {
    _codeEditor->lineNumberAreaWheelEvent(e);
}

int LineNumberArea::findClosing(QTextBlock block, bool comment) {
    QTextBlock blk = block.next();
    QString txt = blk.text();
    //qDebug() << "Block number" << blk.blockNumber();
    QString indentation = _codeEditor->getIndentation(txt);
    QString tmp;
    int line;
    bool found = false;
    
    blk = blk.next();
    while (blk.isValid() && !found) {
        txt = blk.text();
        if (!comment) {
            if (_codeEditor->lastNonWhitespaceChar(txt) == '}') {
                tmp = _codeEditor->getIndentation(txt);
                if (tmp == indentation) {
                    found = true;
                    line = blk.blockNumber();
                }
            }
        } else {
            if (txt.contains("*/")) {
                tmp = _codeEditor->getIndentation(txt);
                if (tmp == indentation) {
                    found = true;
                    line = blk.blockNumber();
                }
            }
        }
        
        blk = blk.next();
    }
    
    return line;
}

void LineNumberArea::fold(int lineNumber, bool comment) {
    //qDebug() << "Line number" << lineNumber;
    QTextBlock startBlock;
    if (lineNumber > 0) {
        startBlock = _codeEditor->document()->findBlockByLineNumber(lineNumber - 1);
    } else {
        startBlock = _codeEditor->document()->findBlockByLineNumber(lineNumber);
    }
    int endPos = findClosing(startBlock, comment);
    QTextBlock endBlock = _codeEditor->document()->findBlockByLineNumber(endPos);
    
    QTextBlock block = startBlock.next();
    block = block.next();
    while (block.isValid() && block != endBlock) {
        block.setVisible(false);
        block.setLineCount(0);
        block = block.next();
    }
    
    //add fold eher
    _codeEditor->_foldedLines.append(startBlock.blockNumber() + 1);
    _codeEditor->document()->markContentsDirty(startBlock.position(), endPos);
}

void LineNumberArea::unfold(int lineNumber, bool comment) {
    //qDebug() << "Line number" << lineNumber;
    QTextBlock startBlock;
    if (lineNumber > 0) {
        startBlock = _codeEditor->document()->findBlockByLineNumber(lineNumber - 1);
    } else {
        startBlock = _codeEditor->document()->findBlockByLineNumber(lineNumber);
    }
    int endPos = findClosing(startBlock, comment);
    QTextBlock endBlock = _codeEditor->document()->findBlockByLineNumber(endPos);
    
    QTextBlock block = startBlock.next();
    block = block.next();
    while (block.isValid() && block != endBlock) {
        block.setVisible(true);
        block.setLineCount(block.layout()->lineCount());
        endPos = block.position() + block.length();
        if (_codeEditor->_foldedLines.contains(block.blockNumber())) {
            int close = findClosing(block, comment);
            block = _codeEditor->document()->findBlockByLineNumber(close);
        } else {
            block = block.next();
        }
    }

    
    _codeEditor->_foldedLines.removeAt(_codeEditor->_foldedLines.indexOf(startBlock.blockNumber() + 1));
    _codeEditor->document()->markContentsDirty(startBlock.position(), endPos);
}
