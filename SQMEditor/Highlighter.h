#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include <QHash>
#include <QTextCharFormat>

class Highlighter : public QSyntaxHighlighter {
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

    void highlightSelectedWord(const QString &text);
    
    void setSelectedWord(QString &word);
    void rehighlightLines(int line);
    
    void setPar(QTextDocument *d);

protected:
    void highlightBlock(const QString &text);

private:
    void _rehighlightLines(QList<int> &lines);
    QString sanitize(QString &word);
    
    struct HighlightingRule {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> _highlightingRules;

    QRegExp _commentStartExpression;
    QRegExp _commentEndExpression;

    QTextCharFormat _keywordFormat;
    QTextCharFormat _classFormat;
    QTextCharFormat _singleLineCommentFormat;
    QTextCharFormat _multiLineCommentFormat;
    QTextCharFormat _quotationFormat;
    QTextCharFormat _singleQuotationFormat;
    QTextCharFormat _functionFormat;
    QTextCharFormat _argFormat;
    QTextCharFormat _varFormat;
    QTextCharFormat _objFormat;
    QTextCharFormat _selectedWordFormat;
    QTextCharFormat _numberFormat;
    QTextCharFormat _escapeFormat;
    QTextCharFormat _doubleBracketFormat;
    QTextCharFormat _builtInFormat;
    
    QRegExp _doubleBracketStart;
    QRegExp _doubleBracketEnd;

    QTextDocument *_parent;
    QString _selectedWord;
    QRegExp _selectedWordPattern;
};

#endif // HIGHLIGHTER_H
