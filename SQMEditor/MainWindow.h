#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QToolBar>

#include "CentralDocument.h"
#include "CentralWidget.h"
#include "ConsoleDock.h"
#include "FSDock.h"

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
    
    void openFolder();
    void openFile(const QString &file);
    void run();
    
    void clearConsole();
    void updateConsoleErr();
    void updateConsoleOut();
    
private:
    Ui::MainWindow *_ui;
    
    CentralWidget *_centralWidget;
    CentralDocument *_centralDocument;
    
    QToolBar *_toolBar;
    QLabel *_statusLabel;
    FSDock *_fsDock;
    ConsoleDock *_consoleDock;
    QProcess *_process;
};

#endif // MAINWINDOW_H
