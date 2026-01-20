#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QLabel>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnConnecter_clicked();
    void on_btnMarche_clicked();
    void on_btnArret_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket socket;
    QTimer *timerEntrees;

    bool tapisEnMarche;

    void ecrire1Bit(int adresse, bool valeur);
    void lireEntrees(int adresseDepart, int nombre);

    void avancerTapis();
    void arreterTapis();

    void setLed(QLabel *led, bool etat);
    void ajouterLog(const QString &message);
};

#endif
