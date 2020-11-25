#include "widget123.h"
#include "ui_widget123.h"

widget123::widget123(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::widget123)
{
    ui->setupUi(this);
}

widget123::~widget123()
{
    delete ui;
}
