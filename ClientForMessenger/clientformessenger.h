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
    void sentCommandToServer(qint8, QString);
    void connectToHost();

private:
    QTextEdit* GetMessegeDisplay();
    void DisplayMessege(QString);
    void executeCommandFromServer(qint8, QString);

private:
    Ui::ClientForMessenger *ui;
    QTcpSocket* tcpSocket_;
    qint16 nextBlockBytes_;
};
