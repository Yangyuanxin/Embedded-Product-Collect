#include "patientdialog.h"
#include "ui_patientdialog.h"
#include<QScrollBar>
#include<QScrollArea>
#include"doctoradvice.h"
extern QString name;
PatientDialog::PatientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientDialog)
{
    ui->setupUi(this);

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    this->tcpSocket = new QTcpSocket(this);
        TCPConnect();//这里我是采用程序启动就自访问服务端（也可根据实际情况采用手动连接手动输入服务端ip的方式。）
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(revData()));
        //readyRead()表示服务端发送数据过来即发动信号，接着revData()进行处理。
        connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(displayError(QAbstractSocket::SocketError)));
        datasize = -1;
}

PatientDialog::~PatientDialog()
{
    delete ui;
}
void PatientDialog::on_verticalScrollBar_sliderMoved(int position)
{
    ui->scrollAreaWidgetContents->move(0,-position*2.5);
}
void PatientDialog::TCPConnect()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.15",6666);
    qDebug()  << "666"<< endl;
}

void PatientDialog::revData()
{
    if(datasize == 0)
    {
        QDataStream in(tcpSocket);
        in.setVersion( QDataStream::Qt_4_6);
        if(tcpSocket->bytesAvailable() < sizeof(quint32))
        {
            return;
        }
        in>>datasize;
    }
    if(datasize > tcpSocket->bytesAvailable())
    {
        return;
    }
    displayvideo();
    QByteArray datas = tcpSocket->readAll();//接收字符串数据。
    qDebug() <<"error"<< datas.data() << endl;
    if(tr(datas.data()) == tr("hello"))
    {
        this->tcpSocket->write("patient");
    }
    else
    {
        qDebug() << "error" << tr(datas.data()) << endl;
        QStringList list =tr(datas.data()).split(" ",QString::SkipEmptyParts);//进行分割
        QStringList::Iterator it = list.begin();
        if(*it != tr("select"))
        {
            ui->comboBox->addItem(*it);
            it++;
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
            it++;
            ui->textEdit_5->setText(*it);
            it++;
            ui->textEdit_6->setText(*it);
            it++;
            ui->textEdit_7->setText(*it);
            it++;
            ui->textEdit_8->setText(*it);
            it++;
            ui->textEdit_9->setText(*it);
            it++;
            ui->textEdit_10->setText(*it);
            it++;
            while(++it != list.end())
            {
                qDebug() << *it << endl;
                ui->comboBox->addItem(*it);
                it += 11;
            }
        }
        else
        {
            it += 2;
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
            it++;
            ui->textEdit_5->setText(*it);
            it++;
            ui->textEdit_6->setText(*it);
            it++;
            ui->textEdit_7->setText(*it);
            it++;
            ui->textEdit_8->setText(*it);
            it++;
            ui->textEdit_9->setText(*it);
            it++;
            ui->textEdit_10->setText(*it);
        }
    }
}
void PatientDialog::displayError(QAbstractSocket::SocketError)
{
   qDebug()<<tcpSocket->errorString();
   tcpSocket->close();
}
void PatientDialog::displayvideo()
{
        QByteArray Data = tcpSocket->read(datasize);
        QBuffer buffer(&Data);
        buffer.open( QIODevice::ReadOnly);

        QImageReader reader(&buffer, "png");
        QImage image = reader.read();

        ui->label->setScaledContents(true);
        ui->label->setPixmap(QPixmap::fromImage(image,Qt::AutoColor));

       // if(datasize != 0) ui->label_2->setText(QString::number(datasize));

        //将datasize图片大小信息重置0为下一接收做准备。

        datasize = 0;
}

void PatientDialog::on_comboBox_activated(int index)
{
    QString temp_index = QString::number(index+1);
    this->tcpSocket->write(temp_index.toLocal8Bit());
}

void PatientDialog::on_pushButton_clicked()    //advice
{
    name = ui->comboBox->currentText();
    int i = ui->comboBox->currentIndex();
    qDebug() << name << endl;
/*    if(i != 0)
    {
        name.remove(0,1);
    }*/
    DoctorAdvice ob;
    ob.setGeometry(0,0,320,240);
    ob.exec();
}
