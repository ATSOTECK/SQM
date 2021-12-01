#include "MiniMap.h"
#include "CodeEditor.h"

#include <QScrollBar>

namespace  {
    float minOpacity = 0.4f;
    float maxOpacity = 0.8f;
}

MiniMap::MiniMap(QPlainTextEdit *parent, Highlighter *h, CodeEditor *c):
    QPlainTextEdit(parent)
{
    setWordWrapMode(QTextOption::NoWrap);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setReadOnly(true);
    //setCenterOnScroll(true);
    setMouseTracking(true);
    viewport()->setCursor(Qt::PointingHandCursor);
    setTextInteractionFlags(Qt::NoTextInteraction);
    //grabKeyboard();

    _parent = parent;
    _linesCount = 0;
    setSliderAreaLinesCount();
    _highlighter = h;
    _codeEditor = c;

#ifdef Q_OS__mac
    _mac = true;
#else
    _mac = false;
#endif

    if (!_mac) {
        _goe = new QGraphicsOpacityEffect();
        setGraphicsEffect(_goe);
        _goe->setOpacity(minOpacity);
        _animation = new QPropertyAnimation(_goe, "opacity");
    } else {
        _goe = 0;
        _animation = 0;
    }

    setFrameStyle(0);
    setTabStopDistance(2);

    _slider = new SliderArea(this, this);
    _slider->show();
}

MiniMap::~MiniMap() {

}

void MiniMap::calculateMax() {
    int lineHeight = _parent->cursorRect().height();
    if (lineHeight > 0) {
        _linesCount = _parent->viewport()->height() / lineHeight;
        setSliderAreaLinesCount();
    }
    _slider->updatePosition();
    updateVisibleArea();
}

void MiniMap::updateVisibleArea() {
    if (!_slider->isPressed()) {
        int lineNumber = _codeEditor->editorFirstVisibleBlockNumber();
        QTextBlock block = document()->findBlockByLineNumber(lineNumber);
        _cursor = textCursor();
        _cursor.setPosition(block.position());
        QRect rect = cursorRect(_cursor);
        setTextCursor(_cursor);
        _slider->moveSlider(rect.y());
    }
}

void MiniMap::setCode(const QString &source) {
    setPlainText(source);
    //setDocument(source);
    calculateMax();
}

void MiniMap::adjustToParent() {
    setFixedHeight(_parent->height());
    setFixedWidth(_parent->width() / 10);
    int x = _parent->width() - width();
    move(x, 0);
    int fontSize = (int) width() / 80;
    if (fontSize < 1)
        fontSize = 1;
    QFont font = document()->defaultFont();
    font.setPointSize(fontSize);
    font.setStyleStrategy(QFont::PreferAntialias);
    this->setFont(font);
    calculateMax();
}

void MiniMap::enterEvent(QEvent *) {
    if (_mac)
        return;

    _animation->setDuration(300);
    _animation->setStartValue(minOpacity);
    _animation->setEndValue(maxOpacity);
    _animation->start();
}

void MiniMap::leaveEvent(QEvent *) {
    if (_mac)
        return;

    _animation->setDuration(300);
    _animation->setStartValue(maxOpacity);
    _animation->setEndValue(minOpacity);
    _animation->start();
}

void MiniMap::mousePressEvent(QMouseEvent *e) {
    _cursor = cursorForPosition(e->pos());
    QTextCursor c = _parent->textCursor();
    c.setPosition(_cursor.position());
    _parent->setTextCursor(c);
    verticalScrollBar()->setValue(_parent->verticalScrollBar()->value());
}

void MiniMap::resizeEvent(QResizeEvent *) {
    _slider->updatePosition();
}

void MiniMap::scrollArea(QPointF parentPos, QPointF sliderPos) {
    QPointF p = parentPos;
    p.setY(parentPos.y() - sliderPos.y());
    QTextCursor c = cursorForPosition(p.toPoint());
    _parent->verticalScrollBar()->setValue(c.blockNumber());
}

void MiniMap::wheelEvent(QWheelEvent *e) {
    /*
    int degrees = e->delta() / 8;
    int steps = degrees / 4;
    if (e->orientation() == Qt::Vertical) {
        verticalScrollBar()->setValue(verticalScrollBar()->value() - steps);
        _parent->verticalScrollBar()->setValue(_parent->verticalScrollBar()->value() - steps);
    }
    */
    _codeEditor->lineNumberAreaWheelEvent(e);
}

void MiniMap::setSliderAreaLinesCount() {
    _slider->setLinesCount(_linesCount);
}

void MiniMap::keyEvent(QKeyEvent *e) {
    keyPressEvent(e);
}

void MiniMap::keyPressEvent(QKeyEvent *e) {
    QPlainTextEdit::keyPressEvent(e);
}

void MiniMap::sliderAreaWheelEvent(QWheelEvent *e) {
    wheelEvent(e);
}

/////////////////////////////////////////////////////////////////////////////////////////

SliderArea::SliderArea(QFrame *parent, MiniMap *m):
    QFrame(parent),
    _parent(parent),
    _mini(m)
{
    setMouseTracking(true);
    setCursor(Qt::OpenHandCursor);
    _color = QColor(68, 68, 66, 128);
    setStyleSheet("background: " + _color.name());

#ifdef Q_OS_MAC
    _mac = true;
#else
    _mac = false;
#endif

    if (!_mac) {
        _goe = new QGraphicsOpacityEffect();
        setGraphicsEffect(_goe);
        _goe->setOpacity(maxOpacity);
    } else {
        _goe = 0;
        setStyleSheet("background: transparent;");
    }
    _pressed = false;
    _countLines = 57;
    _scrollMargins[0] = 0;
    _scrollMargins[1] = 0;
}

void SliderArea::paintEvent(QPaintEvent *event) {
    QPainter *painter = new QPainter();
    painter->begin(this);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->fillRect(event->rect(), QBrush(QColor(68, 68, 66, 128)));
    painter->setPen(QPen(Qt::NoPen));
    painter->end();
}

void SliderArea::updatePosition() {
    int fontSize = QFontMetrics(_parent->font()).height();
    int height = _countLines * fontSize;
    setFixedHeight(height);
    setFixedWidth(_parent->width());
    _scrollMargins[0] = height;
    _scrollMargins[1] = (_parent->height() - height);
}

void SliderArea::moveSlider(float y) {
    this->move(0, y);
}

bool SliderArea::isPressed() const {
    return _pressed;
}

void SliderArea::mousePressEvent(QMouseEvent *) {
    _pressed = true;
    setCursor(Qt::ClosedHandCursor);
}

void SliderArea::mouseReleaseEvent(QMouseEvent *) {
    _pressed = false;
    setCursor(Qt::OpenHandCursor);
}

void SliderArea::mouseMoveEvent(QMouseEvent *event) {
    if (_pressed) {
        QPointF pos = mapToParent(event->pos());
        int y = pos.y() - (height() / 2);
        if (y < 0)
            y = 0;
        if (y < _scrollMargins[0]) {
            _mini->verticalScrollBar()->setSliderPosition(_mini->verticalScrollBar()->sliderPosition() - 2);
        } else if (y > _scrollMargins[1]) {
            _mini->verticalScrollBar()->setSliderPosition(_mini->verticalScrollBar()->sliderPosition() + 2);
        }
        moveSlider(y);
        _mini->scrollArea(pos, event->pos());
    }
}

void SliderArea::setLinesCount(int /*lines*/) {
    //_countLines = lines;
}

void SliderArea::wheelEvent(QWheelEvent *e) {
    _mini->sliderAreaWheelEvent(e);
}
