#include "floormapdialog.h"
#include "ui_floormapdialog.h"
#include<QLabel>

int slidestepx = 0;
int slidestepy = 0;
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

void FloorMapDialog::on_verticalScrollBar_sliderMoved(int position)
{
    ui->scrollAreaWidgetContents->move(slidestepx,-position*1.5);
    slidestepy =  -position*1.5;
}



void FloorMapDialog::on_horizontalScrollBar_sliderMoved(int position)
{
     ui->scrollAreaWidgetContents->move(-position*1.5,slidestepy);
     slidestepx =  -position*1.5;
}
