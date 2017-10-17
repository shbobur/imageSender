#include "senderwidget.h"
#include "ui_senderwidget.h"

#include <QTcpSocket>
#include <QByteArray>
#include <QImage>
#include <QBuffer>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringListModel>
#include <QTcpSocket>
#include <QMessageBox>
#include <QHostAddress>

SenderWidget::SenderWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SenderWidget)
{
    ui->setupUi(this);

    ui->sendButton->setEnabled(false);

    tcpSocket = new QTcpSocket(this);

    connect(tcpSocket, SIGNAL(connected()), SLOT(socketConnected()));
    connect(tcpSocket, SIGNAL(disconnected()), SLOT(socketDisconnected()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError(QAbstractSocket::SocketError)));
    tryToConnect();


    connect(ui->fileOpenButton, SIGNAL(clicked(bool)), SLOT(fileOpenClicked()));
    connect(ui->sendButton, SIGNAL(clicked(bool)), SLOT(onSend()));
    connect(ui->connectButton, SIGNAL(clicked(bool)), SLOT(tryToConnect()));
}

SenderWidget::~SenderWidget()
{
    tcpSocket->disconnect();
    tcpSocket->close();
    tcpSocket->deleteLater();
    delete ui;
}

void SenderWidget::fileOpenClicked()
{
    QStringList list = getFileNames();
    if (list.isEmpty())
        return;

    filesList = list;
    QString filePath;
    foreach (filePath, filesList) {
        QFile file(filePath);
        QFileInfo fileInfo(file);
        fileNamesList.append(fileInfo.fileName());
    }
    loadFiles();
}

void SenderWidget::socketConnected()
{
    ui->stateLabel->setText(tr("Connected"));
    if (!filesList.isEmpty()) {
        ui->sendButton->setEnabled(true);
    }
    ui->connectButton->setEnabled(false);
}

void SenderWidget::socketDisconnected()
{
    ui->stateLabel->setText(tr("Disconnect"));
    ui->sendButton->setEnabled(false);
    ui->connectButton->setEnabled(true);
}

void SenderWidget::socketError(QAbstractSocket::SocketError error)
{
    ui->connectButton->setEnabled(true);
    switch(error)
    {
    case 0:
        ui->stateLabel->setText(tr("The connection was refused by the peer (or timed out)."));
        break;
    case 1:
        ui->stateLabel->setText(tr("The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close "
                                   "notification has been sent."));
        break;
    case 2:
        ui->stateLabel->setText(tr("The host address was not found."));
        break;
    case 3:
        ui->stateLabel->setText(tr("The socket operation failed because the application lacked the required privileges."));
        break;
    case 4:
        ui->stateLabel->setText(tr("The local system ran out of resources (e.g., too many sockets)."));
        break;
    case 5:
        ui->stateLabel->setText(tr("The socket operation timed out."));
        break;
    case 6:
        ui->stateLabel->setText(tr("The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)."));
        break;
    case 7:
        ui->stateLabel->setText(tr("An error occurred with the network (e.g., the network cable was accidentally plugged out)."));
        break;
    case 8:
        ui->stateLabel->setText(tr("The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive."));
        break;
    case 9:
        ui->stateLabel->setText(tr("The address specified to QAbstractSocket::bind() does not belong to the host."));
        break;
    case 10:
        ui->stateLabel->setText(tr("The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)."));
        break;
    case 12:
        ui->stateLabel->setText(tr("The socket is using a proxy, and the proxy requires authentication."));
        break;
    case 13:
        ui->stateLabel->setText(tr("The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)"));
        break;
    case 11:
        ui->stateLabel->setText(tr("Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background)."));
        break;
    case 14:
        ui->stateLabel->setText(tr("Could not contact the proxy server because the connection to that server was denied"));
        break;
    case 15:
        ui->stateLabel->setText(tr("The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)"));
        break;
    case 16:
        ui->stateLabel->setText(tr("The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase."));
        break;
    case 17:
        ui->stateLabel->setText(tr("The proxy address set with setProxy() (or the application proxy) was not found."));
        break;
    case 18:
        ui->stateLabel->setText(tr("The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood."));
        break;
    case 19:
        ui->stateLabel->setText(tr("An operation was attempted while the socket was in a state that did not permit it."));
    case 20:
        ui->stateLabel->setText(tr("The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library."));
    case 21:
        ui->stateLabel->setText(tr("Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library."));
    case 22:
        ui->stateLabel->setText(tr("A temporary error occurred (e.g., operation would block and socket is non-blocking)."));
        break;
    case -1:
        ui->stateLabel->setText(tr("An unidentified error occurred."));
    }

}

void SenderWidget::onSend()
{

    QFile file(filesList.first());

    QDataStream out(tcpSocket);

    if (file.open(QIODevice::ReadOnly))
    {
        QFileInfo fileInfo(file);
        QString fileName(fileInfo.fileName());
        qint64 fileSize = fileInfo.size();

        //out << fileName;
        //out << QString::number(fileSize);

        qDebug() << "Filename: " << fileName;
        qDebug() << "Filesize: " << fileSize;

        QByteArray rawFile;
        rawFile = file.readAll();
        qDebug() << "rawFile size " << rawFile.size();
                    //false size inc
        out << rawFile;

        qDebug() << rawFile.size() << " bytes are sent.";
    //                progressBar->setValue(rawFile.size());
        ui->stateLabel->setText(QString("%1 bytes sent.").arg(rawFile.size()));
      //  out << QString("#END").toLatin1();
    }
}


QStringList SenderWidget::getFileNames()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this, tr("Choose an image file"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation),
                                            tr("Image files (*.png *.jpeg *.jpg *.bmp)"));
    return fileNames;
}

void SenderWidget::loadFiles()
{
    QStringListModel *model = new QStringListModel(this);
    model->setStringList(fileNamesList);
    ui->filesList->setModel(model);

    if (!ui->connectButton->isEnabled()) {
        ui->sendButton->setEnabled(true);
    }
}

void SenderWidget::tryToConnect()
{
    tcpSocket->connectToHost(QHostAddress(ui->IPLineEdit->text()), ui->portLineEdit->text().toInt());
}
