#ifndef DOCTORADVICE_H
#define DOCTORADVICE_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>

namespace Ui {
class DoctorAdvice;
}

class DoctorAdvice : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoctorAdvice(QWidget *parent = 0);
    ~DoctorAdvice();
    
private slots:
    void on_pushButton_clicked();
    void TCPConnect();//用于建立服务端与客户之间的连接函数
        void revData(); //接收来自服务端的数据
        void displayError(QAbstractSocket::SocketError);
private:
    Ui::DoctorAdvice *ui;
    QTcpSocket *tcpSocket;
};

#endif // DOCTORADVICE_H
