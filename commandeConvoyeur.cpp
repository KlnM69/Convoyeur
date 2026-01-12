#include "commandeConvoyeur.h"
#include "ui_commandeConvoyeur.h"

commandeConvoyeur::commandeConvoyeur(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::commandeConvoyeur)
{
    ui->setupUi(this);

    // Initialiser le client Modbus TCP
    modbusClient = new QModbusTcpClient(this);
    modbusClient->setConnectionParameter(
        QModbusDevice::NetworkAddressParameter, "192.168.1.2"); // IP de ton X332
    modbusClient->setConnectionParameter(
        QModbusDevice::NetworkPortParameter, 502);  // Port Modbus par défaut

    modbusClient->setTimeout(1000);   // Timeout 1 seconde
    modbusClient->setNumberOfRetries(3);  // Nombre de tentatives
    modbusClient->connectDevice();   // Connecter le client Modbus

    connect(ui->btnOn, &QPushButton::clicked,this, [=]() { writeBit(true); });
    connect(ui->btnOff, &QPushButton::clicked,this, [=]() { writeBit(false); });
    connect(ui->btnReadBit, &QPushButton::clicked,this, &commandeConvoyeur::readBit);
    connect(ui->btnWriteReg, &QPushButton::clicked,this, &commandeConvoyeur::writeRegister);
}

commandeConvoyeur::~commandeConvoyeur()
{
    delete ui;
}

// Fonction pour écrire un bit (relai ON/OFF)
void commandeConvoyeur::writeBit(bool value)
{
    QModbusDataUnit unit(
        QModbusDataUnit::Coils,  // Type de données (Coils = relais)
        0,                       // Adresse du relais (00001)
        1);                      // Nombre de relais à écrire (ici 1)

    unit.setValue(0, value);  // Valeur : true = ON, false = OFF

    if (auto *reply = modbusClient->sendWriteRequest(unit, 1)) {
        connect(reply, &QModbusReply::finished, this, [=]() {
            if (reply->error() == QModbusDevice::NoError)
                ui->labelStatus->setText("Relais commandé avec succès !");
            else
                ui->labelStatus->setText("Erreur : " + reply->errorString());
            reply->deleteLater();
        });
    }
}

// Fonction pour lire un bit (état du relais)
void commandeConvoyeur::readBit()
{
    QModbusDataUnit unit(
        QModbusDataUnit::Coils,  // Type de données (Coils = relais)
        0,                       // Adresse du relais
        1);                      // Nombre de relais à lire (ici 1)

    if (auto *reply = modbusClient->sendReadRequest(unit, 1)) {
        connect(reply, &QModbusReply::finished, this, [=]() {
            if (reply->error() == QModbusDevice::NoError) {
                bool etat = reply->result().value(0);
                ui->labelStatus->setText(
                    etat ? "Relais allumé" : "Relais éteint");
            } else {
                ui->labelStatus->setText("Erreur : " + reply->errorString());
            }
            reply->deleteLater();
        });
    }
}

// Fonction pour écrire un registre (ex : vitesse du convoyeur)
void commandeConvoyeur::writeRegister()
{
    QModbusDataUnit unit(
        QModbusDataUnit::HoldingRegisters, // Type de données (Holding Registers)
        0,                                 // Adresse du registre (40001)
        1);                                // Nombre de registres à écrire (1)

    unit.setValue(0, 100);  // Valeur à écrire (par exemple 100 pour la vitesse)

    if (auto *reply = modbusClient->sendWriteRequest(unit, 1)) {
        connect(reply, &QModbusReply::finished, this, [=]() {
            if (reply->error() == QModbusDevice::NoError)
                ui->labelStatus->setText("Registre écrit avec succès !");
            else
                ui->labelStatus->setText("Erreur : " + reply->errorString());
            reply->deleteLater();
        });
    }
}
