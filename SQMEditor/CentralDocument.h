#ifndef CENTRALDOCUMENT_H
#define CENTRALDOCUMENT_H

#include "common.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>

class Titlebar : public QWidget {
    Q_OBJECT
    
public:
    Titlebar(const QString &title, QWidget *parent = 0);
    
    void setName(const QString &name);
    void switchTo(const QString &name);
    
public slots:
    void closeClicked();
    
signals:
    void closeCurrent();
    void documentSelected(QString);
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    QString _title;
    QHBoxLayout *_layout;
    QToolBar *_toolbar;
    QComboBox *_box;
};

class CentralDocument : public QWidget {
    Q_OBJECT
    
public:
    CentralDocument(QWidget *parent = 0);
    
    void addWidget(QWidget *widget, const QString &name);
    void currentIndexSet(int index);
    keep int currentIndexGet() const;
    keep QWidget *currentWidgetGet() const;
    keep QString currentWidgetGetName() const;
    keep QStackedWidget *getStack();
    
    void setName(const QString &name);
    
    keep bool alreadyOpen(const QString &name) const;
    void switchTo(const QString &name);
    
public slots:
    void indexChanged(int index);
    void closeCutrrentClicked();
    void documentSelected(QString name);
    
signals:
    void currentChanged(int);
    void closeCurrent(int);
    
private:
    Titlebar *_titlebar;
    QStackedWidget *_stack;
    QStringList _names;
    QVBoxLayout *_layout;
    
    bool _titlebarAdded;
    bool _titlebarHidden;
};

#endif // CENTRALDOCUMENT_H
