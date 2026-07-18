#include "boardscanner.h"

// Standard UUIDs for AT-09 (CC2541) serial service and characteristic
const QUuid SERIAL_SERVICE_UUID(QString("0000ffe0-0000-1000-8000-00805f9b34fb"));
const QUuid TX_RX_CHARACTERISTIC_UUID(QString("0000ffe1-0000-1000-8000-00805f9b34fb"));

BoardScanner::BoardScanner(QObject *parent)
    : QObject(parent), running(true), m_controller(nullptr), m_service(nullptr)
{
    m_moduleAddress = QBluetoothAddress("98:7B:F3:6B:BF:A9");

    m_controller = new QLowEnergyController(m_moduleAddress, this);

    connect(m_controller, &QLowEnergyController::connected, this, [this]() {
        m_controller->discoverServices();
    });

    connect(m_controller, &QLowEnergyController::discoveryFinished, this, [this]() {
        // Check whether module has wanted serial service (FFE0)
        if (m_controller->services().contains(SERIAL_SERVICE_UUID)) {
            m_service = m_controller->createServiceObject(SERIAL_SERVICE_UUID, this);
            
            if (m_service) {
                connect(m_service, &QLowEnergyService::characteristicChanged, 
                        this, &BoardScanner::onCharacteristicChange);
                
                connect(m_service, &QLowEnergyService::stateChanged, this, [this](QLowEnergyService::ServiceState state) {
                    if (state == QLowEnergyService::ServiceDiscovered) {
					// Finding TX/RX characteristic (FFE1) and subscribing on its notifications
                        QLowEnergyCharacteristic characteristic = m_service->characteristic(TX_RX_CHARACTERISTIC_UUID);
                        if (characteristic.isValid()) {
                            QLowEnergyDescriptor descriptor = characteristic.descriptor(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration);
                            if (descriptor.isValid()) {
                                m_service->writeDescriptor(descriptor, QByteArray::fromHex("0100")); // turn on notify listening mode
                            }
                        }
                    }
                });
                m_service->discoverDetails();
            }
        }
    });

    for(int i = 0; i < 64; i++){
        new_state[i] = 0;
        if(i > 15 && i < 47)
            old_state[i] = 1;
        else
            old_state[i] = 0;
    }
}

BoardScanner::~BoardScanner()
{
    stop();
}

void BoardScanner::stop()
{
    running = false;
    if (m_controller) {
        m_controller->disconnectFromDevice();
    }
}

void BoardScanner::process()
{
    if(m_controller){ 
        m_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
        m_controller->connectToDevice();
    }
}

void BoardScanner::onCharacteristicChange(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
    if (c.uuid() == TX_RX_CHARACTERISTIC_UUID) {
        if (value.size() == 8) {
            int idx = 0;
            
            for (int i = 0; i < 8; ++i) {
                uint8_t byte = static_cast<uint8_t>(value[i]);
                
                for (int j = 0; j < 8; ++j) {
                    new_state[idx++] = (byte >> j) & 0x01;
                }
            }
            emit boardState(new_state);
        }
    }
}
