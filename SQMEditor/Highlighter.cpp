#include <QtGui>

#include "Highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    _parent = parent;

    HighlightingRule rule;

    _keywordFormat.setForeground(QColor(216, 68, 51));
    //_keywordFormat.setFontWeight(QFont::setItalic(true));
    QStringList keywordPatterns;
    keywordPatterns << "\\band\\b" << "\\bbool\\b" << "\\bbreak\\b" << "\\bcase\\b" << "\\bcast\\b" << "\\bclass\\b"
                    << "\\bconst\\b" << "\\bcontinue\\b" << "\\bdefault\\b" << "\\bdo\\b" << "\\bdouble\\b" << "\\belse\\b"
                    << "\\benum\\b" << "\\bfalse\\b" << "\\bfinal\\b" << "\\bfloat\\b" << "\\bfor\\b" << "\\bfrom\\b" << "\\bfuncdef\\b"
                    << "\\bget\\b" << "\\bif\\b" << "\\bimport\\b" << "\\bin\\b" << "\\binout\\b" << "\\bint\\b" << "\\binterface\\b"
                    << "\\bint8\\b" << "\\bint16\\b" << "\\bint32\\b" << "\\bint64\\b" << "\\bis\\b" << "\\bmixin\\b"
                    << "\\bnamespace\\b" << "\\bnot\\b" << "\\bnull\\b" << "\\bor\\b" << "\\bout\\b" << "\\boverride\\b"
                    << "\\bprivate\\b" << "\\breturn\\b" << "\\bset\\b" << "\\bshared\\b" << "\\bsuper\\b" << "\\bswitch\\b"
                    << "\\bthis\\b" << "\\btrue\\b" << "\\btypedef\\b" << "\\buint\\b" << "\\buint8\\b" << "\\buint16\\b"
                    << "\\buint32\\b" << "\\buint64\\b" << "\\bvoid\\b" << "\\bwhile\\b" << "\\bxor\\b" << "\\bstring\\b"
                    << "\\+" << "-" << "\\+\\+" << "--" << "\\=" << "\\+\\=" << "-\\=" << "/" << "\\*" << "/\\=" << "\\*\\=" 
                    << "%" << "%\\=" << "<" << ">" << "<\\=" << ">\\=" << ":" << "\\?" << "!" << "!\\=" << "\\*\\*" 
                    << "\\*\\*\\=" << "\\|\\|" << "&&" << "~" << "&" << "\\|" << "\\^" << "<<" << ">>" << "<<\\=" 
                    << ">>\\=" << ">>>" << ">>>\\=" << "@" << "\\=\\=" << "&\\=" << "\\|\\=" << "\\^\\=" << "\\^\\^" << "::";
    foreach (const QString &pattern, keywordPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _keywordFormat;
        _highlightingRules.append(rule);
    }
    
    _varFormat.setForeground(QColor(160, 226, 45));
    rule.pattern = QRegExp("(?=\\.)+\\b[A-Za-z0-9_]+\\b");
    rule.format = _varFormat;
    //_highlightingRules.append(rule);
    
    _objFormat.setForeground(QColor(120, 149, 137));
    QStringList objPatterns;
    objPatterns << "\\b[A-Za-z0-9_]+(?=:)" << "\\b[A-Za-z0-9_]+(?=\\.)";
    foreach (const QString &pattern, objPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _objFormat;
        _highlightingRules.append(rule);
    }
    
    //_functionFormat.setForeground(QColor("#b8bb26"));
    _functionFormat.setForeground(QColor(184, 187, 38));
    rule.pattern = QRegExp("\\b[a-zA-Z_]{1,}[A-Za-z0-9_]+(?=\\()");
    rule.format = _functionFormat;
    _highlightingRules.append(rule);
    
    _objFormat.setForeground(QColor(0xfe, 0xa6, 0x01));
    _objFormat.setFontItalic(true);
    
    //_classFormat.setForeground(QColor(0, 160, 190));//new 2ECCFA
    _classFormat.setForeground(QColor(244, 185, 47));
    QStringList classPatterns;
    classPatterns << "Qubit";
    //rule.pattern = QRegExp("\\bQ[A-Za-z]+\\b");
    foreach (const QString &pattern, classPatterns) {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format = _classFormat;
        _highlightingRules.append(rule);
    }
    
    _builtInFormat.setForeground(QColor(120, 149, 137));
    QStringList builtInPatterns;
    builtInPatterns << "Zero" << "One";
    foreach (const QString &pattern, builtInPatterns) {
        rule.pattern = QRegExp("\\b" + pattern + "\\b");
        rule.format = _builtInFormat;
        _highlightingRules.append(rule);
    }

    _numberFormat.setForeground(QColor("#b16286"));
    QStringList numberPatterns;
    numberPatterns << "\\b[0-9]+\\b" << "\\bnull\\b" << "\\btrue\\b" << "\\bfalse\\b" 
                   << "\\b[A-Z_]{1,}[A-Z0-9_]+\\b" << "\\b0[xX][0-9a-fA-F]+\\b"
                   << "\\b[0-9]{1,}[lLfFuU]+\\b";
    foreach (const QString &pattern, numberPatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _numberFormat;
        _highlightingRules.append(rule);
    }
    
    _quotationFormat.setForeground(QColor(187, 187, 38));
    rule.pattern = QRegExp("\"[^\"]*(\\.[^\"]*)*\"");
    rule.format = _quotationFormat;
    _highlightingRules.append(rule);

    _singleQuotationFormat.setForeground(QColor(187, 187, 38));
    rule.pattern = QRegExp("\'[^\']*(\\.[^\']*)*\'");
    rule.format = _singleQuotationFormat;
    _highlightingRules.append(rule);
    
    _doubleBracketFormat.setForeground(QColor(187, 187, 38));
    
    _doubleBracketStart = QRegExp("\\[\\[");
    _doubleBracketEnd = QRegExp("\\]\\]");
    
    _escapeFormat.setForeground(QColor(216, 68, 51));
    QStringList escapePatterns;
    escapePatterns << "\\\\n" << "\\\\t";
    foreach (const QString &pattern, escapePatterns) {
        rule.pattern = QRegExp(pattern);
        rule.format = _escapeFormat;
        _highlightingRules.append(rule);
    }

    _singleLineCommentFormat.setFontItalic(true);
    _singleLineCommentFormat.setForeground(QColor(146, 131, 116));
    rule.pattern = QRegExp("//[^\n]*");
    rule.format = _singleLineCommentFormat;
    _highlightingRules.append(rule);

    _multiLineCommentFormat.setFontItalic(true);
    _multiLineCommentFormat.setForeground(QColor(146, 131, 116));

    _commentStartExpression = QRegExp("/\\*");
    _commentEndExpression = QRegExp("\\*/");

    //_selectedWordFormat.setForeground(Qt::darkGreen);
    _selectedWordFormat.setBackground(QColor(Qt::darkGreen).darker(180));
    //rule.pattern = QRegExp("\".*\"");
    rule.format = _selectedWordFormat;
    //_highlightingRules.append(rule);
    _selectedWordPattern = QRegExp();
}

void Highlighter::highlightBlock(const QString &text) {
    foreach (const HighlightingRule &rule, _highlightingRules) {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0) {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    
    /*
    QString blk = currentBlock().text();
    if (blk.contains("function") && blk.contains("(")) {
        
    }
    */
    
    setCurrentBlockState(0);

    int startIndex = 0;
    ///*
    if (previousBlockState() != 1) {
        startIndex = _doubleBracketStart.indexIn(text);
    }

    while (startIndex >= 0) {
        int endIndex = _doubleBracketEnd.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + _doubleBracketEnd.matchedLength();
        }
        setFormat(startIndex, commentLength, _doubleBracketFormat);
        startIndex = _doubleBracketStart.indexIn(text, startIndex + commentLength);
    }
    //*/
    
    startIndex = 0;
    if (previousBlockState() != 2) {
        startIndex = _commentStartExpression.indexIn(text);
    }

    while (startIndex >= 0) {
        int endIndex = _commentEndExpression.indexIn(text, startIndex);
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(2);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + _commentEndExpression.matchedLength();
        }
        setFormat(startIndex, commentLength, _multiLineCommentFormat);
        startIndex = _commentStartExpression.indexIn(text, startIndex + commentLength);
    }
}

void Highlighter::highlightSelectedWord(const QString &text) {
    highlightBlock(text);
}

void Highlighter::setSelectedWord(QString &word) {
    if (word.length() > 2) {
        //mSelectedWordPattern = QRegExp("\\b" + sanitize(word) + "\\b");
    } else {
        //mSelectedWordPattern = QRegExp();
    }
}

QString Highlighter::sanitize(QString &word) {
    return word.replace("\\", "\\\\");
}

void Highlighter::rehighlightLines(int line) {
    if (!_parent)
        return;
    
    QTextBlock block = _parent->findBlockByNumber(line);
    rehighlightBlock(block);
}

void Highlighter::_rehighlightLines(QList<int> &/*lines*/) {
    
}

void Highlighter::setPar(QTextDocument *d) {
    _parent = d;
}
