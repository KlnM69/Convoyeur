#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setLed(ui->ledInput1, false);
    setLed(ui->ledInput2, false);
    setLed(ui->ledInput3, false);
    setLed(ui->ledInput4, false);

    tapisEnMarche = false;

    timerEntrees = new QTimer(this);
    connect(timerEntrees, &QTimer::timeout, this, [this]() {
        lireEntrees(0, 4);
    });

    ajouterLog("Application demarree");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ajouterLog(const QString &message)
{
    QString h = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->txtLogs->append("[" + h + "] " + message);
}

void MainWindow::on_btnConnecter_clicked()
{
    ajouterLog("Connexion au X-332");

    socket.connectToHost("192.168.1.2", 502);

    if (socket.waitForConnected(3000)) {
        ajouterLog("Connexion etablie");
        timerEntrees->start(200);
    } else {
        ajouterLog("Erreur de connexion");
    }
}

void MainWindow::ecrire1Bit(int adresse, bool valeur)
{
    if (socket.state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray trame;
    trame.append(char(0x00)); trame.append(char(0x01));
    trame.append(char(0x00)); trame.append(char(0x00));
    trame.append(char(0x00)); trame.append(char(0x06));
    trame.append(char(0x01));
    trame.append(char(0x05));
    trame.append(char(adresse >> 8));
    trame.append(char(adresse & 0xFF));
    trame.append(valeur ? char(0xFF) : char(0x00));
    trame.append(char(0x00));

    socket.write(trame);
}

void MainWindow::avancerTapis()
{
    if (tapisEnMarche)
        return;

    ecrire1Bit(0, true);
    tapisEnMarche = true;
    ajouterLog("Tapis en marche");
}

void MainWindow::arreterTapis()
{
    if (!tapisEnMarche)
        return;

    ecrire1Bit(0, false);
    tapisEnMarche = false;
    ajouterLog("Tapis arrete");
}

void MainWindow::on_btnMarche_clicked()
{
    avancerTapis();
}

void MainWindow::on_btnArret_clicked()
{
    arreterTapis();
}

void MainWindow::lireEntrees(int adresseDepart, int nombre)
{
    if (socket.state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray trame;
    trame.append(char(0x00)); trame.append(char(0x02));
    trame.append(char(0x00)); trame.append(char(0x00));
    trame.append(char(0x00)); trame.append(char(0x06));
    trame.append(char(0x01));
    trame.append(char(0x02));
    trame.append(char(adresseDepart >> 8));
    trame.append(char(adresseDepart & 0xFF));
    trame.append(char(nombre >> 8));
    trame.append(char(nombre & 0xFF));

    socket.write(trame);
    socket.waitForReadyRead(3000);

    QByteArray reponse = socket.readAll();
    if (reponse.size() < 10)
        return;

    quint8 etats = reponse[9];

    bool input1 = etats & 0x01;
    bool input2 = etats & 0x02;

    setLed(ui->ledInput1, input1);
    setLed(ui->ledInput2, input2);
    setLed(ui->ledInput3, etats & 0x04);
    setLed(ui->ledInput4, etats & 0x08);

    if (input2)
        arreterTapis();
}

void MainWindow::setLed(QLabel *led, bool etat)
{
    if (etat)
        led->setStyleSheet("background-color: green; border-radius: 15px;");
    else
        led->setStyleSheet("background-color: red; border-radius: 15px;");
}
