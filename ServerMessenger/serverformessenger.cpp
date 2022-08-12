#include "serverformessenger.h"
#include "ui_serverformessenger.h"
#include <QTime>

ServerForMessenger::ServerForMessenger(int port, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServerForMessenger)
    , tcpServer_(new QTcpServer(this))
    , nextBlockSize_(0)
    , sessionPeopleSockets_()
    , clientSocketsQueue_()
    , nextFreeClient_(nullptr)
{
    ui->setupUi(this);
    if(!tcpServer_->listen(QHostAddress::Any, port)){
        qDebug() << "Server not started";
    }
    connect(tcpServer_, SIGNAL(newConnection()), this, SLOT(NewConnection()));
    displayMessege("Server Started");

}



ServerForMessenger::~ServerForMessenger()
{
    delete ui;
}



void ServerForMessenger::NewConnection()
{
    QTcpSocket* clientSocket = tcpServer_->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ReadClient()));
    sendToClient(clientSocket, "Server Connected");
    addToQueueClient(clientSocket);
    displayMessege("New user connected");
    makeDialogWithFreeClient();
}



bool ServerForMessenger::isSessionWithClient(QTcpSocket * client)
{
    return sessionPeopleSockets_.contains(client);
}



void ServerForMessenger::addToQueueClient(QTcpSocket * client)
{
    clientSocketsQueue_.enqueue(client);
}



void ServerForMessenger::removeSessionWithClient(QTcpSocket * client)
{
    QTcpSocket* clientThatWasInSessionWithDisconnectedUser = sessionPeopleSockets_.take(client);
    sessionPeopleSockets_.remove(clientThatWasInSessionWithDisconnectedUser);
    addToQueueClient(clientThatWasInSessionWithDisconnectedUser);
}



void ServerForMessenger::Disconnected()
{
    QTcpSocket* disconnectedUser = dynamic_cast<QTcpSocket*>(sender());
    if(isSessionWithClient(disconnectedUser)){
        removeSessionWithClient(disconnectedUser); //also adding to the Queue another client
    }
            qDebug() << "FASda";
    makeDialogWithFreeClient();
    displayMessege("User disconnected");
}



void ServerForMessenger::createSessionWithTwoClients(QTcpSocket * lClient, QTcpSocket * rClient)
{
    sessionPeopleSockets_[lClient] = rClient;
    sessionPeopleSockets_[rClient] = lClient;
}



QTcpSocket* ServerForMessenger::getSessionSecondClient(QTcpSocket * firstClientInSession)
{
    return sessionPeopleSockets_.value(firstClientInSession);
}



void ServerForMessenger::makeDialogWithFreeClient()
{

    QTcpSocket* nextFreeClient = nullptr;
    while(!clientSocketsQueue_.isEmpty()){
        if(clientSocketsQueue_.head() != nullptr){
            nextFreeClient = clientSocketsQueue_.dequeue();
            break;
        }
    }

    if(nextFreeClient == nullptr){
        displayMessege("No user for session");
        return;
    }

    while(!clientSocketsQueue_.isEmpty()){
        if(clientSocketsQueue_.head() != nullptr){
            QTcpSocket* secondFreeClient = clientSocketsQueue_.dequeue();
            createSessionWithTwoClients(nextFreeClient, secondFreeClient);
            displayMessege("Session were made");
            return;
        }
    }

    if(nextFreeClient->state() == QAbstractSocket::SocketState::ConnectedState){
        addToQueueClient(nextFreeClient);
    }
    displayMessege("No second user for session");
}



void ServerForMessenger::ReadClient()
{
    QString messege;
    qint8 command;
    QTcpSocket* clientSocket = dynamic_cast<QTcpSocket*>(sender());
    QDataStream in(clientSocket);
    while(true){
        if(nextBlockSize_ == 0){
            if(clientSocket->bytesAvailable() < int(sizeof(qint16))){
                break;
            }
            in >> nextBlockSize_;
        }
        if(clientSocket->bytesAvailable() < nextBlockSize_){
            break;
        }else{
            nextBlockSize_ = 0;
        }
        in >> command >> messege;
        executeCommandFromClient(clientSocket, command, messege);
    }

}



void ServerForMessenger::sendCommandToClient(QTcpSocket * client, qint8 command, QString messege)
{
    QByteArray rawMessege;
    QDataStream out(&rawMessege, QIODevice::WriteOnly);
    out <<qint16(0) << command << messege;
    out.device()->seek(0);
    out << quint16(rawMessege.size() - sizeof(qint16));
    client->write(rawMessege);
}



void ServerForMessenger::sendToClient(QTcpSocket *socket, QString messege)
{
    qint8 command(0);
    sendCommandToClient(socket, command, messege);
}



void ServerForMessenger::displayMessege(QString messege)
{
    ui->textBrowser->append(messege);
    ui->textBrowser->append("QUEQE SIZE: " + QString("(%1)").arg(clientSocketsQueue_.size()));
    ui->textBrowser->append("HASH SIZE: " + QString("(%1)").arg(sessionPeopleSockets_.size()));
    //ui->textBrowser->append("NEXT FREE CLIENT " + QString("(%1)").arg(nextFreeClient_ != nullptr));

}



void ServerForMessenger::executeCommandFromClient(QTcpSocket* client, qint8 command, QString messege)
{
    int commandId = command;
    switch (commandId) {

    case 0:
        sentMessegeForClientsInSession(client, messege);
        break;

    default:
        displayMessege("Unknown Command");
    }
}



void ServerForMessenger::sentMessegeForClientsInSession(QTcpSocket* client, QString messege)
{
    if(sessionPeopleSockets_.find(client) == sessionPeopleSockets_.end()){
        sendToClient(client, "Wait for finding person...");
        return;
    }
    sendToClient(client, "You: " + messege);
    sendToClient(sessionPeopleSockets_.value(client), "Stranger :" + messege);

}
