#ifndef BSDIALOG_H
#define BSDIALOG_H

#include <QDialog>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QTextCodec>
namespace Ui {
class BSDialog;
}

class BSDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit BSDialog(QWidget *parent = 0);
    ~BSDialog();
    
private slots:
    void on_pushButton_4_clicked();

    void on_pushButton_clicked();

    void TCPConnect();//用于建立服务端与客户之间的连接函数
        void revData(); //接收来自服务端的数据
        void displayError(QAbstractSocket::SocketError);

        void on_pushButton_3_clicked();

private:
    Ui::BSDialog *ui;

    QTcpSocket *tcpSocket;

};

#endif // BSDIALOG_H
