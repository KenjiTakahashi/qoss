#include "frontend.h"
#include "ui_frontend.h"

frontend::frontend(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::frontend)
{
    ui->setupUi(this);
}

frontend::~frontend()
{
    delete ui;
}
