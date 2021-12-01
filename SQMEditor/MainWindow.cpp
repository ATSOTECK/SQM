#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CodeEditor.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _centralWidget(new CentralWidget(this)),
    _centralDocument(new CentralDocument()),
    _toolBar(new QToolBar(this)),
    _statusLabel(new QLabel)
{
    _ui->setupUi(this);
    
    setUnifiedTitleAndToolBarOnMac(true);
    addToolBar(_toolBar);
    
    setCentralWidget(_centralWidget);
    _centralWidget->addDocument(_centralDocument);
    CodeEditor *e = new CodeEditor("test.sqm");
    _centralDocument->addWidget(e, e->getName());
    
    connect(e, SIGNAL(statusInfoChanged(QString)), this, SLOT(updateStatusInfoLabel(QString)));
    
    _statusLabel->setText("1/1: 0  ");
    _statusLabel->setFont(QFont("Monaco"));
    _statusLabel->setStyleSheet("color:#929292;");
    statusBar()->addPermanentWidget(_statusLabel);
    statusBar()->setStyleSheet("background: #252526;");
}

MainWindow::~MainWindow() {
    delete _ui;
}

void MainWindow::updateStatusInfoLabel(QString status) {
    _statusLabel->setText(status);
}
