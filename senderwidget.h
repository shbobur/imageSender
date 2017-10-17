#ifndef SENDERWIDGET_H
#define SENDERWIDGET_H

#include <QWidget>
#include <QAbstractSocket>

class QTcpSocket;

namespace Ui {
class SenderWidget;
}

class SenderWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SenderWidget(QWidget *parent = 0);
    ~SenderWidget();

private slots:
    void fileOpenClicked();
    void socketConnected();
    void socketDisconnected();
    void socketError(QAbstractSocket::SocketError error);
    void onSend();
    void tryToConnect();

private:
    Ui::SenderWidget *ui;
    QTcpSocket *tcpSocket;

    QStringList fileNamesList;
    QStringList filesList;
    QStringList getFileNames();
    void loadFiles();

};

#endif // SENDERWIDGET_H
