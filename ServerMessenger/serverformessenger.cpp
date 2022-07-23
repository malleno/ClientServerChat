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

void ServerForMessenger::NewConnection(){
    QTcpSocket* clientSocket = tcpServer_->nextPendingConnection();
    connect(clientSocket, SIGNAL(disconnected()), clientSocket, SLOT(deleteLater()));
    connect(clientSocket, SIGNAL(disconnected()), this, SLOT(Disconnected()));
    connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ReadClient()));
    sendToClient(clientSocket, "Server Connected");
    clientSocketsQueue_.enqueue(clientSocket);
    displayMessege("New user connected");
    makeDialogWithFreeClient();
}

void ServerForMessenger::Disconnected(){
    QTcpSocket* disconnectedUser = dynamic_cast<QTcpSocket*>(sender());
    if(sessionPeopleSockets_.contains(disconnectedUser)){
        QTcpSocket* clientThatWasInSessionWithDisconnectedUser = sessionPeopleSockets_.take(disconnectedUser);
        sessionPeopleSockets_.remove(clientThatWasInSessionWithDisconnectedUser);
        clientSocketsQueue_.enqueue(clientThatWasInSessionWithDisconnectedUser);
    }
    makeDialogWithFreeClient();
    displayMessege("User disconnected");
}

void ServerForMessenger::makeDialogWithFreeClient(){
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
            sessionPeopleSockets_[nextFreeClient] = secondFreeClient;
            sessionPeopleSockets_[secondFreeClient] = nextFreeClient;
            displayMessege("Session were made");
            return;
        }
    }
    clientSocketsQueue_.enqueue(nextFreeClient);
    displayMessege("No second user for session");
}

void ServerForMessenger::ReadClient(){
    QString messege;
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
        in >> messege;
        messege.push_front("Stranger : ");
        sendToClient(sessionPeopleSockets_[clientSocket], messege);
        ui->textBrowser->append("Messege " +messege+ " delivered");
    }

}

void ServerForMessenger::sendToClient(QTcpSocket *socket, QString messege){
    QByteArray rawMessege;
    QDataStream out(&rawMessege, QIODevice::WriteOnly);
    out <<qint16(0) <<  messege;
    out.device()->seek(0);
    out << quint16(rawMessege.size() - sizeof(qint16));
    socket->write(rawMessege);
}

void ServerForMessenger::displayMessege(QString messege){
    ui->textBrowser->append(messege);
    ui->textBrowser->append("QUEQE SIZE: " + QString("(%1)").arg(clientSocketsQueue_.size()));
    ui->textBrowser->append("HASH SIZE: " + QString("(%1)").arg(sessionPeopleSockets_.size()));
    //ui->textBrowser->append("NEXT FREE CLIENT " + QString("(%1)").arg(nextFreeClient_ != nullptr));

}

void ServerForMessenger::executeCommandFromClient(QTcpSocket* client, std::bitset<8> command, QString messege){
    int commandId = command._Find_first();
    switch (commandId) {

    case 0:
        sendToClient(client, messege);
        break;

    default:
        displayMessege("Unknown Command");
    }
}
