#include "floormapdialog.h"
#include "ui_floormapdialog.h"

FloorMapDialog::FloorMapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FloorMapDialog)
{
    ui->setupUi(this);
}

FloorMapDialog::~FloorMapDialog()
{
    delete ui;
}
