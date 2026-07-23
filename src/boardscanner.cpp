
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

  #include "boardscanner.h"

  BoardScanner::BoardScanner(QWidget *parent):
      m_currentDevice(QBluetoothDeviceInfo()), foundBoardScannerService(false),
      m_control(0), m_service(0)
  {
      m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

      connect(m_deviceDiscoveryAgent, SIGNAL(deviceDiscovered(const QBluetoothDeviceInfo&)),
              this, SLOT(addDevice(const QBluetoothDeviceInfo&)));
      connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
              this, SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));
      connect(m_deviceDiscoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
  }

  BoardScanner::~BoardScanner(){}

  void BoardScanner::deviceSearch()
  {
      qDeleteAll(m_devices);
      m_devices.clear();
      m_deviceDiscoveryAgent->start();
      setMessage("Scanning for devices...");
  }

  void BoardScanner::addDevice(const QBluetoothDeviceInfo &device)
  {
      if (device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) {
          qWarning() << "Discovered LE Device name: " << device.name() << " Address: "
                     << device.address().toString();
          DeviceInfo *dev = new DeviceInfo(device);
          m_devices.append(dev);
      }
  }

  void BoardScanner::scanFinished()
  {
      if (m_devices.isEmpty()){
          setMessage("No Low Energy devices found");
          emit exit();
      }
      else
      {
          for(QObject *obj : m_devices){
              DeviceInfo *dev = static_cast<DeviceInfo*>(obj);
              if(dev->getDevice().name() == "BT05"){
                  connectToService(dev->getAddress()); 
                  return;
              }
          }
          setMessage("Board not found");
          emit exit();
      }
  }

  void BoardScanner::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error error)
  {
      if (error == QBluetoothDeviceDiscoveryAgent::PoweredOffError)
          setMessage("The Bluetooth adaptor is powered off, power it on before doing discovery.");
      else if (error == QBluetoothDeviceDiscoveryAgent::InputOutputError)
          setMessage("Writing or reading from the device resulted in an error.");
      else
          setMessage("An unknown error has occurred.");
      emit exit();
  }

  void BoardScanner::setMessage(QString message)
  {
      m_info = message;
      Q_EMIT messageChanged();
  }

  QString BoardScanner::message() const
  {
      return m_info;
  }

  void BoardScanner::connectToService(const QString &address)
  {

      bool deviceFound = false;
      for (int i = 0; i < m_devices.size(); i++) {
          if (((DeviceInfo*)m_devices.at(i))->getAddress() == address ) {
              m_currentDevice.setDevice(((DeviceInfo*)m_devices.at(i))->getDevice());
              setMessage("Connecting to device...");
              deviceFound = true;
              break;
          }
      }
      if (!deviceFound) {
          setMessage("Device not found.");
          emit exit();
          return;
      }
      
      setMessage("Connecting to device...");
      m_control = new QLowEnergyController(m_currentDevice.getDevice(), this);
     /* QBluetoothDeviceInfo info(QBluetoothAddress(address), QString(), 0);
      info.setCoreConfigurations(QBluetoothDeviceInfo::LowEnergyCoreConfiguration);
      m_control = QLowEnergyController::createCentral(info, this);*/

      connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),
              this, SLOT(serviceDiscovered(QBluetoothUuid)));
      connect(m_control, SIGNAL(discoveryFinished()),
              this, SLOT(serviceScanDone()));
      connect(m_control, SIGNAL(error(QLowEnergyController::Error)),
              this, SLOT(controllerError(QLowEnergyController::Error)));
      connect(m_control, SIGNAL(connected()),
              this, SLOT(deviceConnected()));
      connect(m_control, SIGNAL(disconnected()),
              this, SLOT(deviceDisconnected()));

      m_control->connectToDevice();
  }


  void BoardScanner::deviceConnected()
  {
      m_control->discoverServices();
  }

  void BoardScanner::deviceDisconnected()
  {
      setMessage("BoardScanner service disconnected");
      qWarning() << "Remote device disconnected";

      if(m_service)
      {
          m_service->deleteLater();
          m_service = nullptr;
      }
      if(m_control)
      {
          m_control->deleteLater();
          m_control = nullptr;
      }
      emit exit();
  }


  void BoardScanner::serviceDiscovered(const QBluetoothUuid &gatt)
  {
      if (gatt == QBluetoothUuid(QStringLiteral("0000ffe0-0000-1000-8000-00805f9b34fb"))) {
          setMessage("BoardScanner service discovered. Waiting for service scan to be done...");
          foundBoardScannerService = true;
      }
  }

  void BoardScanner::serviceScanDone()
  {
     
      if (foundBoardScannerService) {
          setMessage("Connecting to service...");
          m_service = m_control->createServiceObject(
                      QBluetoothUuid(QStringLiteral("0000ffe0-0000-1000-8000-00805f9b34fb")), this);
      }

      if (!m_service) {
          setMessage("BoardScanner Service not found.");
          emit exit();
          return;
      }

      connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
              this, SLOT(serviceStateChanged(QLowEnergyService::ServiceState)));
      connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
              this, SLOT(updateBoardState(QLowEnergyCharacteristic,QByteArray)));
      connect(m_service, SIGNAL(descriptorWritten(QLowEnergyDescriptor,QByteArray)),
              this, SLOT(confirmedDescriptorWrite(QLowEnergyDescriptor,QByteArray)));

      m_service->discoverDetails();
  }

  void BoardScanner::disconnectService()
  {
      foundBoardScannerService = false;

      if (m_notificationDesc.isValid() && m_service
          && m_notificationDesc.value() == QByteArray::fromHex("0100"))
      {
          m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0000"));
      } else{
          m_control->disconnectFromDevice();
      }
  }

  void BoardScanner::controllerError(QLowEnergyController::Error error)
  {
      setMessage("Cannot connect to remote device.");
      qWarning() << "Controller Error:" << error;
      emit exit();
  }

  void BoardScanner::serviceStateChanged(QLowEnergyService::ServiceState s)
  {
      switch (s) {
      case QLowEnergyService::ServiceDiscovered:
      {
          const QLowEnergyCharacteristic bsChar = m_service->characteristic(
                      QBluetoothUuid(QStringLiteral("0000ffe1-0000-1000-8000-00805f9b34fb")));
          if (!bsChar.isValid()) {
              setMessage("BS Data not found.");
              emit exit();
              break;
          }

          m_notificationDesc = bsChar.descriptor(
                      QBluetoothUuid::ClientCharacteristicConfiguration);
          if (m_notificationDesc.isValid()) {
              m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));
          }

          break;
      }
      default:
          //nothing for now
          break;
      }
  }

  void BoardScanner::serviceError(QLowEnergyService::ServiceError e)
  {
      switch (e) {
      case QLowEnergyService::DescriptorWriteError:
          setMessage("Cannot obtain BS notifications");
          break;
      default:
          qWarning() << "BS service error:" << e;
      }
      emit exit();
  }

  void BoardScanner::updateBoardState(const QLowEnergyCharacteristic &c,
                                       const QByteArray &value)
  {
      // ignore any other characteristic change -> shouldn't really happen though
      if (c.uuid() != QBluetoothUuid(QStringLiteral("0000ffe1-0000-1000-8000-00805f9b34fb")))
          return;

      if(value.size() == 8){
          int idx = 0;
			
          for(int i = 0; i < 8;  ++i){
              uint8_t byte = static_cast<uint8_t>(value[i]);
              for(int j = 0; j < 8; ++j){
                  new_state[idx++] = (byte >> j) & 0x01;
              }				
          }
          emit boardState(new_state);
      }
  }

  void BoardScanner::confirmedDescriptorWrite(const QLowEnergyDescriptor &d,
                                           const QByteArray &value)
  {
      if (d.isValid() && d == m_notificationDesc && value == QByteArray::fromHex("0000")) {
          //disabled notifications -> assume disconnect intent
          m_control->disconnectFromDevice();
      }
  }












