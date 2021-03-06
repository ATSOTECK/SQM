#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAction>
#include <QLabel>
#include <QMainWindow>
#include <QProcess>
#include <QToolBar>

#include "CentralDocument.h"
#include "CentralWidget.h"
#include "ConsoleDock.h"
#include "FSDock.h"
#include "ResultsDock.h"

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
    void newFile();
    void run();
    void done();
    
    void save();
    
    void clearConsole();
    void updateConsoleErr();
    void updateConsoleOut();
    void hideConsole();
    void showConsole();
    
    void hideExplorer();
    void showExplorer();
    
    void hideResults();
    void showResults();
    
    void hideConsoleResults();
    
    void showHelp();
    
private:
    Ui::MainWindow *_ui;
    
    CentralWidget *_centralWidget;
    CentralDocument *_centralDocument;
    
    QToolBar *_toolBar;
    QLabel *_statusLabel;
    FSDock *_fsDock;
    ConsoleDock *_consoleDock;
    ResultsDock *_resultsDock;
    QProcess *_process;
    
    QAction *_actionRun;
    QAction *_actionOpen;
    QAction *_actionSave;
};

#endif // MAINWINDOW_H
