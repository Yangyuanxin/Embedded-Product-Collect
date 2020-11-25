#ifndef DOCTORDIALOG_H
#define DOCTORDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QTextCodec>
#include<QImage>
#include<QImageReader>
#include <QBuffer>
namespace Ui {
class DoctorDialog;
}

class DoctorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoctorDialog(QWidget *parent = 0);
    ~DoctorDialog();
    int datasize;

private slots:
    void on_comboBox_activated(int index);
    void TCPConnect();//用于建立服务端与客户之间的连接函数
        void revData(); //接收来自服务端的数据
        void displayError(QAbstractSocket::SocketError);
        void displayvideo();
private:
    Ui::DoctorDialog *ui;
    QTcpSocket *tcpSocket;
    QImage * image;
};

#endif // DOCTORDIALOG_H
