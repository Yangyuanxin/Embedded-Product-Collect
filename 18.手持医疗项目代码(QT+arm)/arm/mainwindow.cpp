#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doctordialog.h"
#include "patientdialog.h"
#include"floormapdialog.h"
#include<QScrollBar>
#include "bpdialog.h"
#include "bsdialog.h"
#include"calculatordialog.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_clicked()
{
    DoctorDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}

void MainWindow::on_toolButton_2_clicked()
{
    PatientDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}

void MainWindow::on_toolButton_3_clicked()
{
    FloorMapDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}

void MainWindow::on_toolButton_4_clicked()
{
    BSDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}

void MainWindow::on_toolButton_5_clicked()
{
    BPDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}

void MainWindow::on_toolButton_6_clicked()
{
    CalculatorDialog ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}
