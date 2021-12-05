#ifndef MINIMAPC_H
#define MINIMAPC_H

#include <QFrame>
#include <QPlainTextEdit>
#include <QTextOption>
#include <QGraphicsOpacityEffect>
#include <QFontMetrics>
#include <QPainter>
#include <QColor>
#include <QPen>
#include <QBrush>
#include <Qt>
#include <QPropertyAnimation>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>

class Highlighter;
class CodeEditor;
class MiniMap;

class SliderArea : public QFrame {
public:
    friend class MiniMap;
    
    SliderArea(QFrame *parent = 0, MiniMap *m = 0);

    void updatePosition();

    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *e);

    void moveSlider(float y);

    bool isPressed() const;

    void setLinesCount(int lines);

private:
    QFrame *_parent;
    QColor _color;
    QGraphicsOpacityEffect *_goe;
    bool _pressed;
    int _scrollMargins[2];
    int _countLines;

    bool _mac;
    
    MiniMap *_mini;
};

class MiniMap : public QPlainTextEdit {
public:
    MiniMap(QPlainTextEdit *parent = 0, Highlighter *h = 0, CodeEditor *c = 0);
    ~MiniMap();

    void calculateMax();

    void wheelEvent(QWheelEvent *e);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *e);
    void resizeEvent(QResizeEvent *e);
    
    void sliderAreaWheelEvent(QWheelEvent *e);

    void setCode(const QString &source);

    void adjustToParent();

    void scrollArea(QPointF parentPos, QPointF sliderPos);

    void updateVisibleArea();

    void setSliderAreaLinesCount();
    
    void keyEvent(QKeyEvent *e); 

private:
    void keyPressEvent(QKeyEvent *e);
    
    int _linesCount;
    QPlainTextEdit *_parent;
    CodeEditor *_codeEditor;
    QGraphicsOpacityEffect *_goe;
    QPropertyAnimation *_animation;
    SliderArea *_slider;
    Highlighter *_highlighter;
    QTextCursor _cursor;

    bool _mac;
};

#endif // MINIMAPC_H
