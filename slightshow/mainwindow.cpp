#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>

namespace slightshow
{

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), m_manager(new ProjectorManager)
{
    ui->setupUi(this);
    centralWidget()->setLayout(ui->layout);

    // Enumerate all serial boards and assign to domains
    for (const auto &port : QSerialPortInfo::availablePorts())
    {
        const auto serial = port.serialNumber();
        const auto location = port.systemLocation();
        const auto manufacturer = port.manufacturer();
        qDebug() << manufacturer << serial << location;

        if (location.contains("tty"))
        {
            ui->serialPortList->addItem(QString("%1 (from %2)").arg(location).arg(manufacturer), location);
            // Filter by manufacturer and port name
            if (manufacturer.contains("Prolific"))
            {
                ui->serialPortList->setCurrentIndex(ui->serialPortList->count() - 1);
            }
        }
    }

    connect(ui->serialPortButton, &QPushButton::clicked, this, [this]()
    {
        if (ui->serialPortButton->text() == "Connect") {
            const auto port = ui->serialPortList->currentData().toString();
            m_manager->open(port);
            ui->serialPortList->setEnabled(false);
            ui->serialPortButton->setText("Disconnect");
        }
        else {
            m_manager->close();
            ui->serialPortList->setEnabled(true);
            ui->serialPortButton->setText("Connect");
        }
    });

    connect(ui->limitBrightness, &QSlider::sliderMoved, this, [this](int value)
    {
        m_manager->setMaximumBrightness(value / 99.0);
    });

    connect(ui->forward, &QPushButton::clicked, this, [this]()
    {
        m_manager->forward(1.8);
        ui->forward->setEnabled(false);
    });
    connect(m_manager, &ProjectorManager::forwardFinished, this, [this]()
    {
        ui->forward->setEnabled(true);
    });

    connect(ui->backward, &QPushButton::clicked, this, [this]()
    {
        m_manager->backward(2);
        ui->backward->setEnabled(false);
    });
    connect(m_manager, &ProjectorManager::backwardFinished, this, [this]()
    {
        ui->backward->setEnabled(true);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

}
