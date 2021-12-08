#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "CodeEditor.h"
#include "HelpDialog.h"

#include <cstdlib>
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    _ui(new Ui::MainWindow),
    _centralWidget(new CentralWidget(this)),
    _centralDocument(new CentralDocument()),
    _toolBar(new QToolBar(this)),
    _statusLabel(new QLabel),
    _fsDock(new FSDock(this)),
    _consoleDock(new ConsoleDock(this)),
    _resultsDock(new ResultsDock(this)),
    _actionRun(new QAction(QIcon(":/img/run.png"), "Run", this)),
    _actionOpen(new QAction(QIcon(":/img/folder.png"), "Open", this)),
    _actionSave(new QAction(QIcon(":/img/save.png"), "Save", this))
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
    _toolBar->setMovable(false);
    _toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _toolBar->addAction(_actionOpen);
    _toolBar->addAction(_actionSave);
    _toolBar->addAction(_actionRun);
    
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
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    addDockWidget(Qt::LeftDockWidgetArea, _fsDock);
    addDockWidget(Qt::BottomDockWidgetArea, _consoleDock);
    addDockWidget(Qt::RightDockWidgetArea, _resultsDock);
    
    _consoleDock->hide();
    _resultsDock->hide();
    
    connect(_ui->actionOpen_Folder, SIGNAL(triggered()), this, SLOT(openFolder()));
    connect(_actionOpen, SIGNAL(triggered()), this, SLOT(openFolder()));
    connect(_ui->actionNew_File, SIGNAL(triggered()), this, SLOT(newFile()));
    connect(_ui->actionRun, SIGNAL(triggered()), this, SLOT(run()));
    connect(_actionRun, SIGNAL(triggered()), this, SLOT(run()));
    connect(_ui->actionClear_Console, SIGNAL(triggered()), this, SLOT(clearConsole()));
    connect(_ui->actionHide_Console, SIGNAL(triggered()), this, SLOT(hideConsole()));
    connect(_ui->actionShow_Console, SIGNAL(triggered()), this, SLOT(showConsole()));
    connect(_ui->actionHide_Explorer, SIGNAL(triggered()), this, SLOT(hideExplorer()));
    connect(_ui->actionShow_Explorer, SIGNAL(triggered()), this, SLOT(showExplorer()));
    connect(_ui->actionHide_Results, SIGNAL(triggered()), this, SLOT(hideResults()));
    connect(_ui->actionShow_Results, SIGNAL(triggered()), this, SLOT(showResults()));
    connect(_ui->actionHide_Console_Results, SIGNAL(triggered()), this, SLOT(hideConsoleResults()));
    connect(_ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(_actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(_ui->actionShow_Help, SIGNAL(triggered()), this, SLOT(showHelp()));
    
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
    if (!dir.isEmpty()) {
        _fsDock->setDir(dir);
    }
}

void MainWindow::openFile(const QString &file) {
    QFileInfo fi(file);
    if (!fi.exists() || !fi.isFile()) {
        return;
    }
    
    QString name = file.right(file.length() - file.lastIndexOf('/') - 1);
    
    if (_centralDocument->alreadyOpen(name)) {
        _centralDocument->switchTo(name);
        
        return;
    }
    
    CodeEditor *e = new CodeEditor(name, this);
    e->openFile(file);
    
    _centralDocument->addWidget(e, e->getName());
    connect(e, SIGNAL(statusInfoChanged(QString)), this, SLOT(updateStatusInfoLabel(QString)));
}

void MainWindow::newFile() {
    QString file = QFileDialog::getSaveFileName(this, "New File", QDir::homePath() + "/Documents", "SQM Files (*.sqm)");
    if (file.isEmpty()) {
        return;
    }
    
    QString name = file.right(file.length() - file.lastIndexOf('/') - 1);
    CodeEditor *e = new CodeEditor(name, this);
    e->setFilePath(file);
    e->saveFile();
    _centralDocument->addWidget(e, e->getName());
    
    connect(e, SIGNAL(statusInfoChanged(QString)), this, SLOT(updateStatusInfoLabel(QString)));
}

void MainWindow::run() {
    CodeEditor *e = dynamic_cast<CodeEditor *>(_centralDocument->currentWidgetGet());
    if (e == nullptr) {
        return;
    }
    
    QString filePath = e->getFilePath();
    if (!filePath.endsWith(".sqm")) {
        return;
    }
    
    _resultsDock->show();
    _resultsDock->clear();
    _consoleDock->show();
    _consoleDock->clear();
    _consoleDock->addText("New run on " + QDateTime::currentDateTime().toString("dd/MM") + " at " + 
                          QDateTime::currentDateTime().toString("h:mm:ss ap"));
    _consoleDock->addText("Running file " + filePath + "\n");
    
    QStringList args;
    args << filePath;
    QString program = "qEngine";
    
    _process = new QProcess(this);
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(updateConsoleErr()));
    connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(updateConsoleOut()));
    connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done()));
    
    _process->start(program, args);
}

void MainWindow::done() {
    QString line;
    int lines = _consoleDock->lineCount();
    QVector<double> vec;
    int measure = -1;
    
    for (int i = 0; i < lines; ++i) {
        line = _consoleDock->line(i);
        if (line.contains("probability distribution:")) {
            line = _consoleDock->line(i + 1);
            break;
        }
    }
    
    const QRegularExpression rx("[^\\d+\\.?\\d*]");
    QStringList list = line.split(rx, Qt::SkipEmptyParts);
    for (int j = 0; j < 8; ++j) {
        if (j < list.length()) {
            vec.append(list[j].toDouble());
        } else {
            vec.append(0.0);
        }
    }
    
    for (int i = 0; i < lines; ++i) {
        line = _consoleDock->line(i);
        if (line.contains("measure:")) {
            line = _consoleDock->line(i + 1);
            measure = line.toInt();
            break;
        }
    }
    
    _resultsDock->addData(vec, measure);
    
    _consoleDock->scrollToBottom();
}

void MainWindow::save() {
    CodeEditor *e = dynamic_cast<CodeEditor *>(_centralDocument->currentWidgetGet());
    if (e == nullptr) {
        return;
    }
    
    if (!e->saveFile()) {
        _consoleDock->show();
        _consoleDock->addText("Failed to save '" + e->getFilePath() + "'");
    }
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

void MainWindow::hideConsole() {
    _consoleDock->hide();
}

void MainWindow::showConsole() {
    _consoleDock->show();
}

void MainWindow::hideExplorer() {
    _fsDock->hide();
}

void MainWindow::showExplorer() {
    _fsDock->show();
}

void MainWindow::hideResults() {
    _resultsDock->hide();
}

void MainWindow::showResults() {
    _resultsDock->show();
}

void MainWindow::hideConsoleResults() {
    _consoleDock->hide();
    _resultsDock->hide();
}

void MainWindow::showHelp() {
    HelpDialog hlp(this);
    hlp.exec();
}
