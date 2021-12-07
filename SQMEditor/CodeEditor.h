#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QTextDocumentWriter>
#include <QObject>
#include <QCompleter>
#include <QList>
#include <QWheelEvent>
#include <QWidget>

#include "common.h"
#include "Highlighter.h"
#include "MiniMap.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
class QCompleter;
class QTimer;
class QUndoStack;

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT

public:
    CodeEditor(QString name, QWidget *parent = 0);
    ~CodeEditor();

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth() const;

    //void mousePressEvent(QMouseEvent *e);

    void setCompleter(QCompleter *c);
    QCompleter *completer() const;

    int _foldArea;
    QList<int> _foldedLines;

    //protected in QPlainTextEdit
    QTextBlock editorFirstVisibleBlock();
    int editorFirstVisibleBlockNumber();
    QPointF editorContentOffset();
    QRectF editorBlockBoundingGeometry(const QTextBlock &block) const;

    QString getName() const;
    QString getFilePath() const;
    
    bool isModified() const;

    bool saveToFile(QString &path);
    bool saveFile();
    bool openFile(const QString &path);
    
    void lineNumberAreaMousePressEvent(QMouseEvent *e);
    void lineNumberAreaMouseMoveEvent(QMouseEvent *e);
    void lineNumberAreaWheelEvent(QWheelEvent *e);
    
    void findMatch(QString text, bool next = false, bool caseSensitive = false, bool wholeWord = false, int foo = 5);
    
    QUndoStack *undoStack() const;
    
    bool insideComment(int lineNumber);

public slots:
    void findText();
    void updateMiniMapText();
    void updateMiniMapVisibleArea();
    void setModified();
    
    void cleanStateChanged(bool state);
    
    //preferences
    void setUseMiniMap(bool mini);
    void setUseTabs(bool tabs);
    void setShowTabsSpaces(bool tabsSpaces);
    void setUseWordWrap(bool wrap);
    
    void copyLine();

protected:
    void resizeEvent(QResizeEvent *event);

    void keyPressEvent(QKeyEvent *e);
    void focusInEvent(QFocusEvent *e);
    
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    
    void paintEvent(QPaintEvent *e);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void highlightSelectedWord(QString txt = "");
    void updateLineNumberArea(const QRect & rect, int dy);
    void updateMiniMapScrollPos();
    void insertCompletion(const QString &completion);
    void blockOrColumnChanged();
    void updateCorner();
    void highlightJumpToDefinition(QTextCursor &cursor, bool pressed);
    
signals:
    void statusInfoChanged(QString info);
    void locateFunction(QString text, bool);
    
    void modifiedChanged(bool);

private:
    friend class LineNumberArea;
    
    void completeBraces(QString text);
    void completeQuotes(QString text);
    void insertIndentation();
    void indentMore();
    void autoCompleteEnter();
    bool checkNextLine(QString &txt);
    void autoCompleteBackspace();
    void autoCompleteL(QString txt);
    bool isInBPBQ(); //BPBQ Brace, Paran, Braket, Quote
    void setUpMiniMap();
    void tearDownMiniMap();
    void buildVarTable();
    QChar lastNonWhitespaceChar(QString str);
    
    void gotoDefinition(QTextCursor *cursor = 0);
    
    QString getIndentation(const QString &text);
    
    QString reverseSelectTextPortionFromOffset(int begin, int end);
    
    void wheelEvent(QWheelEvent *e);

    QWidget *_lineNumberArea;

    QString textUnderCursor() const;
    QString textLeftOfCursor(QTextCursor cur, bool start = false) const;
    QString textRightOfCursor(QTextCursor cur, bool end  = false) const;
    QString textLeftOfCursor(QTextCursor *cur, bool start = false) const;
    QString textRightOfCursor(QTextCursor *cur, bool end  = false) const;
    QString textLeftOfCursor(bool start = false) const;
    QString textRightOfCursor(bool end  = false) const;

    QCompleter *_completer;

    QString _name;
    QString _filePath;

    QPixmap _rightArrowIcon;
    QPixmap _downArrowIcon;

    Highlighter *_highlighter;
    MiniMap *_miniMap;

    QTimer *_timer;
    
    QString _selectedText;
    
    bool _set;
    bool _isModified;
    bool _useTabs;
    
    QUndoStack *_undoStack;
    
    QList<QString> _varTable;
    QList<QTextEdit::ExtraSelection> _extraSelections;
    bool _initialSelectionAdded;
    
    QWidget *_cornerWidget;
};

class LineNumberArea : public QWidget {
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        _codeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(_codeEditor->lineNumberAreaWidth(), 0);
    }
    
    void codeFoldingEvent(int lineNumber);
    void fold(int lineNumber, bool comment = false);
    void unfold(int lineNumber, bool comment = false);
    void isFolded(int lineNumber);
    
    int findClosing(QTextBlock block, bool comment = false);

protected:
    void paintEvent(QPaintEvent *event) {
        _codeEditor->lineNumberAreaPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

    void wheelEvent(QWheelEvent *event);

private:
    CodeEditor *_codeEditor;
};

#endif // CODEEDITOR_H
