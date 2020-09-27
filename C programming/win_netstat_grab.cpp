#include "mainwindow.h"
#include <QDebug>
#include <QApplication>

#include <QHostAddress>
#include <QProcess>

// Runs a simple command in cmd using qt api
QString getWindowsCommandOutput(QString command,QStringList args ){

    QProcess WindowsProcess;
    WindowsProcess.start(command, args, QIODevice::ReadOnly); //Starts execution of command
    WindowsProcess.waitForFinished();

    QString StdOut      =   WindowsProcess.readAllStandardOutput();
    QString StdError    =   WindowsProcess.readAllStandardError();

    WindowsProcess.close();

    if (StdError.isEmpty() && !StdOut.isEmpty())
        return StdOut;

    qDebug() << "COMMAND: " << command << " ERROR: " <<StdError;

    return QString();

}

// This function uses "netstat -ano" to get connections and their owner pid
QList<QStringList> getValidIpConnectionsWindows(){

    QList<QStringList> valid_connections;
    QStringList Netstat_output = getWindowsCommandOutput("netstat", QStringList() << "-ano").split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
    foreach (const QString netstat, Netstat_output) {

        QStringList listening_row =  netstat.simplified().split(" ");
        if (listening_row.length() > 3){

            QHostAddress my_connection_ip(listening_row[1].split(":")[0]);

            if (QAbstractSocket::IPv4Protocol == my_connection_ip.protocol())
            {
                QHostAddress connectedTo_ip(listening_row[2].split(":")[0]);

                if (QAbstractSocket::IPv4Protocol == connectedTo_ip.protocol())
                {
                    valid_connections.append(listening_row);
                    //qDebug("Both send and receive IPv4 address are valid.");
                }


            }

        }

    }
    return valid_connections;
}

int main(int argc, char *argv[])
{

    foreach (const QStringList q, getValidIpConnectionsWindows()){

        qDebug() << q;
    }

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
