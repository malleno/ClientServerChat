#pragma once

#include <QWidget>
#include <QTcpSocket>
#include <QTextEdit>
#include <bitset>


QT_BEGIN_NAMESPACE
namespace Ui { class ClientForMessenger; }
QT_END_NAMESPACE

class ClientForMessenger : public QWidget
{
    Q_OBJECT

public:
    ClientForMessenger(QWidget *parent = nullptr);
    ~ClientForMessenger();
public slots:
    void connectionEstabilished();
    void readMessege();
    void socketError(QAbstractSocket::SocketError);
    void sentMessegeToServer();
    void connectToHost();
private:
    Ui::ClientForMessenger *ui;
    QTcpSocket* tcpSocket_;
    qint16 nextBlockBytes_;

    QTextEdit* GetMessegeDisplay();
    void DisplayMessege(QString);
    void executeCommandFromServer(QTcpSocket*, std::bitset<8>, QString);



};
