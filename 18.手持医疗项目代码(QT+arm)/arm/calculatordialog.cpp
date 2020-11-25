#include "calculatordialog.h"
#include "ui_calculatordialog.h"
#include <QDebug>
#include <QChar>
CalculatorDialog::CalculatorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CalculatorDialog)
{
    ui->setupUi(this);
}

CalculatorDialog::~CalculatorDialog()
{
    delete ui;
}

void CalculatorDialog::on_pushButton_1_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("1");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_2_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("2");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_3_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("3");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_4_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("4");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_5_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("5");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_6_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("6");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_7_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("7");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_8_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("8");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_9_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("9");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_0_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString("0");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_point_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    text += QString(".");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_add_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    QStringList list = text.split(" ");
    QStringList::Iterator it = list.begin();
    qDebug() << list.size() << endl;
    if(list.size() >= 3)
    {
        double data[2];
        data[0] = (*it).toDouble();
        it += 2;
        data[1] = (*it).toDouble();
        it--;
        if(*it == QString("+"))
        {
            data[0] += data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("-"))
        {
            data[0] -= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("*"))
        {
            data[0] *= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("/"))
        {
            data[0] /= data[1];
            text = QString::number(data[0]);
        }
    }
    qDebug() << "error "<< text[text.size() - 1] << endl;
    text += QString(" + ");
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_sub_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    QStringList list = text.split(" ");
    QStringList::Iterator it = list.begin();
    qDebug() << list.size() << endl;
    if(list.size() >= 3)
    {
        double data[2];
        data[0] = (*it).toDouble();
        it += 2;
        data[1] = (*it).toDouble();
        it--;
        if(*it == QString("+"))
        {
            data[0] += data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("-"))
        {
            data[0] -= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("*"))
        {
            data[0] *= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("/"))
        {
            data[0] /= data[1];
            text = QString::number(data[0]);
        }
    }
    qDebug() << "error "<< text[text.size() - 1] << endl;
    text += QString(" - ");
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_multi_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    QStringList list = text.split(" ");
    QStringList::Iterator it = list.begin();
    qDebug() << list.size() << endl;
    if(list.size() >= 3)
    {
        double data[2];
        data[0] = (*it).toDouble();
        it += 2;
        data[1] = (*it).toDouble();
        it--;
        if(*it == QString("+"))
        {
            data[0] += data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("-"))
        {
            data[0] -= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("*"))
        {
            data[0] *= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("/"))
        {
            data[0] /= data[1];
            text = QString::number(data[0]);
        }
    }
    qDebug() << "error "<< text[text.size() - 1] << endl;
    text += QString(" * ");
    ui->textEdit->setText(text);
}


void CalculatorDialog::on_pushButton_del_clicked()
{
    QString text = QString("");
    qDebug() << text << endl;
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_divi_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    QStringList list = text.split(" ");
    QStringList::Iterator it = list.begin();
    qDebug() << list.size() << endl;
    if(list.size() >= 3)
    {
        double data[2];
        data[0] = (*it).toDouble();
        it += 2;
        data[1] = (*it).toDouble();
        it--;
        if(*it == QString("+"))
        {
            data[0] += data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("-"))
        {
            data[0] -= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("*"))
        {
            data[0] *= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("/"))
        {
            data[0] /= data[1];
            text = QString::number(data[0]);
        }
    }
    qDebug() << "error "<< text[text.size() - 1] << endl;
    text += QString(" / ");
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_equal_clicked()
{
    QString text = ui->textEdit->document()->toPlainText();
    QStringList list = text.split(" ");
    QStringList::Iterator it = list.begin();
    qDebug() << list.size() << endl;
        double data[2];
        data[0] = (*it).toDouble();
        it += 2;
        data[1] = (*it).toDouble();
        it--;
        if(*it == QString("+"))
        {
            data[0] += data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("-"))
        {
            data[0] -= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("*"))
        {
            data[0] *= data[1];
            text = QString::number(data[0]);
        }
        else if(*it == QString("/"))
        {
            data[0] /= data[1];
            text = QString::number(data[0]);
        }
    ui->textEdit->setText(text);
}

void CalculatorDialog::on_pushButton_clicked()
{
    this->close();
}
