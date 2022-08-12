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
    void sendToClient(QTcpSocket*, QString);
    void makeDialogWithFreeClient();
    void displayMessege(QString);
    void executeCommandFromClient(QTcpSocket*, qint8, QString);
    void sentMessegeForClientsInSession(QTcpSocket*, QString);
    void sendCommandToClient(QTcpSocket *, qint8 , QString );

    bool isSessionWithClient(QTcpSocket*);
    void removeSessionWithClient(QTcpSocket*);
    void createSessionWithTwoClients(QTcpSocket*, QTcpSocket*);
    QTcpSocket* getSessionSecondClient(QTcpSocket*);

    void addToQueueClient(QTcpSocket*);

private:
    Ui::ServerForMessenger *ui;
    QTcpServer* tcpServer_;
    quint16 nextBlockSize_;
    QHash<QTcpSocket*, QTcpSocket*> sessionPeopleSockets_;
    QQueue<QTcpSocket*> clientSocketsQueue_;
    QTcpSocket* nextFreeClient_;
};
#endif // SERVERFORMESSENGER_H
