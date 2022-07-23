#include "clientformessenger.h"
#include "ui_clientformessenger.h"
#include <QTime>

ClientForMessenger::ClientForMessenger(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientForMessenger)
    , tcpSocket_(new QTcpSocket(this))
    , nextBlockBytes_(0)
{
    ui->setupUi(this);

    connect(ui->connectionButton, SIGNAL(clicked()), SLOT(connectToHost()));
    connect(ui->sendButton, SIGNAL(clicked()), SLOT(sentMessegeToServer()));
}

void ClientForMessenger::connectToHost(){
    const QString host = "localhost";
    int port = 25;
    if(tcpSocket_->state() == QTcpSocket::SocketState::ConnectedState){
        tcpSocket_->disconnect();
    }
    tcpSocket_->connectToHost(host, port);
    connect(tcpSocket_, SIGNAL(connected()), SLOT(connectionEstabilished()));
    connect(tcpSocket_, SIGNAL(readyRead()), SLOT(readMessege()));
    //connect(tcpSocket_, SIGNAL(error(QAbstractSocket::SocketError)), this,  SLOT(socketError(QAbstractSocket::SocketError)));
}

ClientForMessenger::~ClientForMessenger()
{
    delete ui;
}

void ClientForMessenger::connectionEstabilished(){

}

void ClientForMessenger::readMessege(){
    QString messege;
    QDataStream in(tcpSocket_);
    while(true){
        if(nextBlockBytes_ == 0){
            if(tcpSocket_->bytesAvailable() < int(sizeof(qint16))){
                return;
            }
            in >> nextBlockBytes_;
        }
        if(tcpSocket_->bytesAvailable() < nextBlockBytes_){
            return;
        }else{
            nextBlockBytes_ = 0;
        }
        in >> messege;
        DisplayMessege(messege);
    }
}

void ClientForMessenger::socketError(QAbstractSocket::SocketError error){
    DisplayMessege("Error");
}

void ClientForMessenger::sentMessegeToServer(){
    QByteArray rawMessege;
    QDataStream out(&rawMessege, QIODevice::WriteOnly);
    QString messege = ui->messegeInput->document()->toRawText();
    out <<qint16(0) <<  messege;
    out.device()->seek(0);
    out << quint16(rawMessege.size() - sizeof(qint16));
    tcpSocket_->write(rawMessege);
    messege.push_front("You : ");
    DisplayMessege(messege);
    ui->messegeInput->clear();
}

QTextEdit* ClientForMessenger::GetMessegeDisplay(){
    return ui->messegesDisplay;
}

void ClientForMessenger::DisplayMessege(QString messege){
    GetMessegeDisplay()->append(messege);
}

void ClientForMessenger::executeCommandFromServer(QTcpSocket* client, std::bitset<8> command, QString messege){
    int commandId = command._Find_first();
    switch (commandId) {

    case 0:
        DisplayMessege(messege);
        break;

    default:
        DisplayMessege("Unknown Command");
    }
}
