#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QTextCodec>
#include<QImage>
#include<QImageReader>
#include <QBuffer>

namespace Ui {
class PatientDialog;
}

class PatientDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PatientDialog(QWidget *parent = 0);
    ~PatientDialog();
    int datasize;
private slots:
    void on_verticalScrollBar_sliderMoved(int position);
    void TCPConnect();//用于建立服务端与客户之间的连接函数
        void revData(); //接收来自服务端的数据
        void displayError(QAbstractSocket::SocketError);
        void displayvideo();

        void on_comboBox_activated(int index);

        void on_pushButton_clicked();

private:
    Ui::PatientDialog *ui;
    QTcpSocket *tcpSocket;
    QImage * image;
};

#endif // PATIENTDIALOG_H
