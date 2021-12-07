#include "HelpDialog.h"
#include "ui_HelpDialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::HelpDialog)
{
    _ui->setupUi(this);
    
    _ui->plainTextEdit->setReadOnly(true);
    setLayout(_ui->verticalLayout);
    
    connect(_ui->pushButton, SIGNAL(clicked()), this, SLOT(accept()));
}

HelpDialog::~HelpDialog() {
    delete _ui;
}
