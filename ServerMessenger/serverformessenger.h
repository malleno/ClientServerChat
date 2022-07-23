#ifndef SERVERFORMESSENGER_H
#define SERVERFORMESSENGER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QQueue>
#include <bitset>


QT_BEGIN_NAMESPACE
namespace Ui { class ServerForMessenger; }
QT_END_NAMESPACE

class ServerForMessenger : public QWidget
{
    Q_OBJECT

public:
    ServerForMessenger(int port, QWidget *parent = nullptr);
    ~ServerForMessenger();
public slots:
    void NewConnection();
    void ReadClient();
    void Disconnected();

private:
    Ui::ServerForMessenger *ui;
    QTcpServer* tcpServer_;
    quint16 nextBlockSize_;
    QHash<QTcpSocket*, QTcpSocket*> sessionPeopleSockets_;
    QQueue<QTcpSocket*> clientSocketsQueue_;
    QTcpSocket* nextFreeClient_;

   void sendToClient(QTcpSocket*, QString);
   void makeDialogWithFreeClient();
   void displayMessege(QString);
   void executeCommandFromClient(QTcpSocket*, std::bitset<8>, QString);


};
#endif // SERVERFORMESSENGER_H
