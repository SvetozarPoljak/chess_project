/***************************************************************************
  **
  ** Copyright (C) 2014 BlackBerry Limited. All rights reserved.
  ** Copyright (C) 2015 The Qt Company Ltd.
  ** Contact: http://www.qt.io/licensing/
  **
  ** This file is part of the examples of the QtBluetooth module of the Qt Toolkit.
  **
  ** $QT_BEGIN_LICENSE:BSD$
  ** You may use this file under the terms of the BSD license as follows:
  **
  ** "Redistribution and use in source and binary forms, with or without
  ** modification, are permitted provided that the following conditions are
  ** met:
  **   * Redistributions of source code must retain the above copyright
  **     notice, this list of conditions and the following disclaimer.
  **   * Redistributions in binary form must reproduce the above copyright
  **     notice, this list of conditions and the following disclaimer in
  **     the documentation and/or other materials provided with the
  **     distribution.
  **   * Neither the name of The Qt Company Ltd nor the names of its
  **     contributors may be used to endorse or promote products derived
  **     from this software without specific prior written permission.
  **
  **
  ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
  **
  ** $QT_END_LICENSE$
  **
  ****************************************************************************/

#ifndef BOARDSCANNER_H
#define BOARDSCANNER_H

#include "deviceinfo.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>
#include <QLowEnergyService>

QT_USE_NAMESPACE

class BoardScanner : public QObject
{
    Q_OBJECT

public:
    explicit BoardScanner();
    ~BoardScanner();

    void setMessage(QString message);
    QString message() const;

public slots:
    void deviceSearch();
    void connectToService(const QString &address);
    void disconnectService();

private slots:
    // QBluetoothDeviceDiscoveryAgent
    void addDevice(const QBluetoothDeviceInfo &device);
    void scanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);

    // QLowEnergyController
    void serviceDiscovered(const QBluetoothUuid &gatt);
    void serviceScanDone();
    void controllerError(QLowEnergyController::Error);
    void deviceConnected();
    void deviceDisconnected();

    // QLowEnergyService
    void serviceStateChanged(QLowEnergyService::ServiceState state);
    void updateBoardState(const QLowEnergyCharacteristic &c,
                          const QByteArray &value);
    void confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                  const QByteArray &value);
    void serviceError(QLowEnergyService::ServiceError e);

signals:
    void messageChanged();
    void nameChanged();
    void boardState(const int *new_state);

private:
    DeviceInfo m_currentDevice;
    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    QLowEnergyController *m_control;
    QLowEnergyService *m_service;
    QLowEnergyDescriptor m_notificationDesc;

    QList<QObject*> m_devices;

    QString m_info;
    bool foundBoardScannerService;

    int new_state[64];
};

#endif // BOARDSCANNER_H
