#ifndef COMMANDECONVOYEUR_H
#define COMMANDECONVOYEUR_H

#include <QMainWindow>
#include <QModbusTcpClient>
#include <QModbusReply>

QT_BEGIN_NAMESPACE
namespace Ui { class commandeConvoyeur; }
QT_END_NAMESPACE

class commandeConvoyeur : public QMainWindow
{
    Q_OBJECT

public:
    explicit commandeConvoyeur(QWidget *parent = nullptr);
    ~commandeConvoyeur();

private slots:
    void Ecrire1Bit(bool value);   // Écrire sur un bit (relais)
    void Lire1Bit();              // Lire un bit (état relais)
    void EcrireRegister();        // Écrire un registre (ex : vitesse)

private:
    Ui::commandeConvoyeur *ui;
    QModb *modbusClient;  // Client Modbus TCP
};

#endif // COMMANDECONVOYEUR_H
