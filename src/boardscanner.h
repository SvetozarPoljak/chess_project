#ifndef BOARDSCANNER_H
#define BOARDSCANNER_H

#include <QObject>
#include <QUuid>
#include <QByteArray>
#include <QBluetoothAddress>
#include <QLowEnergyController>
#include <QLowEnergyService>

const int BOARD_SIZE = 4;

class BoardScanner : public QObject
{
    Q_OBJECT
public:
    explicit BoardScanner(QObject *parent = nullptr);
    ~BoardScanner();

    void stop();

public slots:
    void process();

signals:
    void boardState(const int *new_state);

private slots:
    // slot gets asinhronous 8 byte data packet
    void onCharacteristicChange(const QLowEnergyCharacteristic &c, const QByteArray &value);

private:
    bool running;

    int old_state[64];
    int new_state[64];

    QBluetoothAddress m_moduleAddress;
    QLowEnergyController *m_controller;
    QLowEnergyService *m_service;
};

#endif // BOARDSCANNER_H
