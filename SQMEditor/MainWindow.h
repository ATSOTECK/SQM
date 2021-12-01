#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QToolBar>

#include "CentralDocument.h"
#include "CentralWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    
public slots:
    void updateStatusInfoLabel(QString status);
    
private:
    Ui::MainWindow *_ui;
    
    CentralWidget *_centralWidget;
    CentralDocument *_centralDocument;
    
    QToolBar *_toolBar;
    QLabel *_statusLabel;
};

#endif // MAINWINDOW_H
