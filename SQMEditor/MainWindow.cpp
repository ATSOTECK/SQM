#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CodeEditor.h"

#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _centralWidget(new CentralWidget(this)),
    _centralDocument(new CentralDocument()),
    _toolBar(new QToolBar(this)),
    _statusLabel(new QLabel),
    _fsDock(new FSDock(this)),
    _consoleDock(new ConsoleDock(this))
{
    _ui->setupUi(this);
    
    QString styleStr;
    QFile *style = new QFile(":/style.css");
    if (style->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(style);
        styleStr = in.readAll();
    }
    style->close();
    delete style;
    setStyleSheet(styleStr);
    
    setUnifiedTitleAndToolBarOnMac(true);
    addToolBar(_toolBar);
    
    setCentralWidget(_centralWidget);
    _centralWidget->addDocument(_centralDocument);
    //CodeEditor *e = new CodeEditor("test.sqm");
    //_centralDocument->addWidget(e, e->getName());
    
    //connect(e, SIGNAL(statusInfoChanged(QString)), this, SLOT(updateStatusInfoLabel(QString)));
    
    _statusLabel->setText("1/1: 0  ");
    _statusLabel->setFont(QFont("Monaco"));
    _statusLabel->setStyleSheet("color:#929292;");
    statusBar()->addPermanentWidget(_statusLabel);
    statusBar()->setStyleSheet("background: #252526;");
    
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _fsDock);
    addDockWidget(Qt::BottomDockWidgetArea, _consoleDock);
    
    connect(_ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(openFolder()));
    
    connect(_fsDock, SIGNAL(selected(const QString &)), this, SLOT(openFile(const QString &)));
}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::updateStatusInfoLabel(QString status) {
    _statusLabel->setText(status);
}

void MainWindow::openFolder() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Folder"), QDir::homePath() + "/Documents");
    _fsDock->setDir(dir);
}

void MainWindow::openFile(const QString &file) {
    QString name = file.right(file.length() - file.lastIndexOf('/') - 1);
    CodeEditor *e = new CodeEditor(name, this);
    e->openFile(file);
    
    _centralDocument->addWidget(e, e->getName());
    connect(e, SIGNAL(statusInfoChanged(QString)), this, SLOT(updateStatusInfoLabel(QString)));
}

void MainWindow::run() {
    _process = new QProcess(this);
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(updateConsoleErr()));
    connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateConsoleOut()));
}

void MainWindow::clearConsole() {
    _consoleDock->clear();
}

void MainWindow::updateConsoleErr() {
    QString text(_process->readAllStandardError());
    
    _consoleDock->addText(text);
}

void MainWindow::updateConsoleOut() {
    QString text(_process->readAllStandardOutput());
    
    _consoleDock->addText(text);
}
