#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QGroupBox>
#include <QMessageBox>
#include <QTimer>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothDeviceInfo>
#include <QBluetoothAddress>
#include <QBluetoothUuid>
#include <QBluetoothServiceInfo>
#include <QBluetoothSocket>
#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent)
    {
        setWindowTitle("BT-Lumina - Bluetooth Smart Device Controller");
        setMinimumSize(800, 600);
        
        // Create central widget
        QWidget *centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        // Create main layout
        QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
        
        // Create device discovery group
        QGroupBox *discoveryGroup = new QGroupBox("Device Discovery", centralWidget);
        QVBoxLayout *discoveryLayout = new QVBoxLayout(discoveryGroup);
        
        // Create scan button and status
        QHBoxLayout *scanLayout = new QHBoxLayout();
        m_scanButton = new QPushButton("Scan for Bluetooth Devices", discoveryGroup);
        m_statusLabel = new QLabel("Status: Ready", discoveryGroup);
        scanLayout->addWidget(m_scanButton);
        scanLayout->addWidget(m_statusLabel);
        scanLayout->addStretch();
        discoveryLayout->addLayout(scanLayout);
        
        // Create device list
        m_deviceList = new QListWidget(discoveryGroup);
        discoveryLayout->addWidget(m_deviceList);
        
        mainLayout->addWidget(discoveryGroup);
        
        // Create control group
        QGroupBox *controlGroup = new QGroupBox("Device Control", centralWidget);
        QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
        
        m_connectButton = new QPushButton("Connect to Selected Device", controlGroup);
        m_connectButton->setEnabled(false);
        controlLayout->addWidget(m_connectButton);
        
        m_deviceInfoLabel = new QLabel("No device selected", controlGroup);
        controlLayout->addWidget(m_deviceInfoLabel);
        
        mainLayout->addWidget(controlGroup);
        
        // Connect signals
        connect(m_scanButton, &QPushButton::clicked, this, &MainWindow::startDeviceDiscovery);
        connect(m_connectButton, &QPushButton::clicked, this, &MainWindow::connectToDevice);
        connect(m_deviceList, &QListWidget::itemSelectionChanged, this, &MainWindow::onDeviceSelectionChanged);
        
        // Initialize Bluetooth
        initializeBluetooth();
    }

private slots:
    void startDeviceDiscovery()
    {
        if (!m_localDevice.isValid()) {
            QMessageBox::warning(this, "Bluetooth Error", 
                               "No Bluetooth adapter found or Bluetooth is disabled.");
            return;
        }
        
        if (m_discoveryAgent && m_discoveryAgent->isActive()) {
            m_discoveryAgent->stop();
            m_scanButton->setText("Scan for Bluetooth Devices");
            m_statusLabel->setText("Status: Ready");
            return;
        }
        
        m_deviceList->clear();
        m_connectButton->setEnabled(false);
        m_deviceInfoLabel->setText("No device selected");
        
        if (!m_discoveryAgent) {
            m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
            
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
                    this, &MainWindow::deviceDiscovered);
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
                    this, &MainWindow::discoveryFinished);
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled,
                    this, &MainWindow::discoveryFinished);
            connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::errorOccurred,
                    this, &MainWindow::discoveryError);
        }
        
        m_scanButton->setText("Stop Scanning");
        m_statusLabel->setText("Status: Scanning...");
        m_discoveryAgent->start();
    }
    
    void deviceDiscovered(const QBluetoothDeviceInfo &device)
    {
        QString deviceInfo = QString("%1 (%2)")
            .arg(device.name().isEmpty() ? "Unknown Device" : device.name())
            .arg(device.address().toString());
        
        QListWidgetItem *item = new QListWidgetItem(deviceInfo, m_deviceList);
        item->setData(Qt::UserRole, QVariant::fromValue(device));
        
        qDebug() << "Device discovered:" << device.name() << device.address().toString();
    }
    
    void discoveryFinished()
    {
        m_scanButton->setText("Scan for Bluetooth Devices");
        m_statusLabel->setText("Status: Scan completed");
        
        if (m_discoveryAgent) {
            m_discoveryAgent->deleteLater();
            m_discoveryAgent = nullptr;
        }
    }
    
    void discoveryError(QBluetoothDeviceDiscoveryAgent::Error error)
    {
        QString errorMsg;
        switch (error) {
            case QBluetoothDeviceDiscoveryAgent::PoweredOffError:
                errorMsg = "Bluetooth adapter is powered off";
                break;
            case QBluetoothDeviceDiscoveryAgent::InputOutputError:
                errorMsg = "Input/output error occurred";
                break;
            case QBluetoothDeviceDiscoveryAgent::InvalidBluetoothAdapterError:
                errorMsg = "Invalid Bluetooth adapter";
                break;
            case QBluetoothDeviceDiscoveryAgent::UnsupportedPlatformError:
                errorMsg = "Unsupported platform";
                break;
            case QBluetoothDeviceDiscoveryAgent::UnsupportedDiscoveryMethod:
                errorMsg = "Unsupported discovery method";
                break;
            default:
                errorMsg = "Unknown error occurred";
                break;
        }
        
        QMessageBox::warning(this, "Discovery Error", errorMsg);
        discoveryFinished();
    }
    
    void onDeviceSelectionChanged()
    {
        QListWidgetItem *currentItem = m_deviceList->currentItem();
        if (currentItem) {
            QBluetoothDeviceInfo device = currentItem->data(Qt::UserRole).value<QBluetoothDeviceInfo>();
            QString info = QString("Name: %1\nAddress: %2\nRSSI: %3")
                .arg(device.name().isEmpty() ? "Unknown" : device.name())
                .arg(device.address().toString())
                .arg(device.rssi());
            m_deviceInfoLabel->setText(info);
            m_connectButton->setEnabled(true);
        } else {
            m_deviceInfoLabel->setText("No device selected");
            m_connectButton->setEnabled(false);
        }
    }
    
    void connectToDevice()
    {
        QListWidgetItem *currentItem = m_deviceList->currentItem();
        if (!currentItem) {
            QMessageBox::warning(this, "Error", "No device selected");
            return;
        }
        
        QBluetoothDeviceInfo device = currentItem->data(Qt::UserRole).value<QBluetoothDeviceInfo>();
        
        // For now, just show a message. In a real implementation, you would:
        // 1. Create a QBluetoothSocket
        // 2. Connect to the device
        // 3. Handle the connection state
        
        QMessageBox::information(this, "Connection", 
                               QString("Attempting to connect to %1 (%2)")
                               .arg(device.name().isEmpty() ? "Unknown Device" : device.name())
                               .arg(device.address().toString()));
        
        // TODO: Implement actual connection logic
        // QBluetoothSocket *socket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        // connect(socket, &QBluetoothSocket::connected, this, &MainWindow::onConnected);
        // connect(socket, &QBluetoothSocket::disconnected, this, &MainWindow::onDisconnected);
        // socket->connectToService(device.address(), QBluetoothUuid::SerialPort);
    }

private:
    void initializeBluetooth()
    {
        if (!m_localDevice.isValid()) {
            QMessageBox::warning(this, "Bluetooth Error", 
                               "No Bluetooth adapter found or Bluetooth is disabled.");
            return;
        }
        
        // Enable Bluetooth if it's not enabled
        if (m_localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            m_localDevice.powerOn();
        }
        
        qDebug() << "Bluetooth initialized. Host mode:" << m_localDevice.hostMode();
    }
    
    QBluetoothLocalDevice m_localDevice;
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;
    QListWidget *m_deviceList = nullptr;
    QPushButton *m_scanButton = nullptr;
    QPushButton *m_connectButton = nullptr;
    QLabel *m_statusLabel = nullptr;
    QLabel *m_deviceInfoLabel = nullptr;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "main.moc" 