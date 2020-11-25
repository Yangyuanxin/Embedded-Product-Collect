#include "doctoradvice.h"
#include "ui_doctoradvice.h"
#include"patientdialog.h"
QString name;
DoctorAdvice::DoctorAdvice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoctorAdvice)
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
        qDebug() << name << endl;
}

DoctorAdvice::~DoctorAdvice()
{
    delete ui;
}

void DoctorAdvice::on_pushButton_clicked()
{
    this->close();
}
void DoctorAdvice::TCPConnect()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.15",6666);
}
void DoctorAdvice::revData()
{
    QByteArray datas = tcpSocket->readAll();//接收字符串数据。
    if(tr(datas.data()) == tr("hello"))
    {
        qDebug() << "hello" << endl;
        this->tcpSocket->write( (name + " advice").toLocal8Bit());
    }
    else
    {
        ui->plainTextEdit->setPlainText(datas.data());
    }
}
void DoctorAdvice::displayError(QAbstractSocket::SocketError)
{
   qDebug()<<tcpSocket->errorString();
   tcpSocket->close();
}
