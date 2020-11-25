#ifndef BPDIALOG_H
#define BPDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QTextCodec>
namespace Ui {
class BPDialog;
}

class BPDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit BPDialog(QWidget *parent = 0);
    ~BPDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void TCPConnect();//用于建立服务端与客户之间的连接函数

    void revData(); //接收来自服务端的数据

    void displayError(QAbstractSocket::SocketError);
private:
    Ui::BPDialog *ui;
    QTcpSocket *tcpSocket;
};

#endif // BPDIALOG_H
