#ifndef CONSOLEDOCK_H
#define CONSOLEDOCK_H

#include <QDockWidget>
#include <QPlainTextEdit>

class ConsoleDock : public QDockWidget {
    Q_OBJECT
public:
    ConsoleDock(QWidget *parent = nullptr);
    
    void clear();
    void setText(const QString &txt);
    void addText(const QString &txt);
    
private:
    QPlainTextEdit *_text;
};

#endif // CONSOLEDOCK_H
