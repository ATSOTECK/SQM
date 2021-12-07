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
    void scrollToBottom();
    
    QString line(int index) const;
    int lineCount() const;
    
private:
    QPlainTextEdit *_text;
};

#endif // CONSOLEDOCK_H
