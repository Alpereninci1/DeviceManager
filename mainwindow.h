#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QTcpServer>
#include <QTcpSocket>
#include "device.h"
#include "Update.h"
#include <QTimer>
#include <QModbusReply>


QT_BEGIN_NAMESPACE

class QModbusClient;
class QModbusReply;

namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum RegisterType {
        Invalid,
        DiscreteInputs,
        Coils,
        InputRegisters,
        HoldingRegisters
    };
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initActions();
    QModbusDataUnit writeRequest() const;

private slots:
    void showDevices();

    void on_pbCreateGroup_clicked();

    void on_tbBack_clicked();

    void on_pbOpenGroup_clicked();

    void on_cbAllDev_clicked();

    void on_pbAddDevice_clicked();

    void on_pbRemoveDevice_clicked();

    void on_pbConnectDevices_clicked();

    void goToManagePage();

    void backToDeviceList();

    void backToDeviceGroup();

    void on_pbSoftware_clicked();

    void on_cbAllMan_clicked();

    void onConnect();

    //void on_disconnect_device_clicked();

    void on_resetButton_clicked();

    void reading();

    void on_uptime_clicked();

    void on_deviceId_clicked();

    void on_VersionControl_clicked();

    bool isConnected();

    void on_pushButton_clicked();


private:
    Ui::MainWindow *ui;
    Device device;
    Update u;
    QTcpSocket *socket;
    DeviceType *d;




};
#endif // MAINWINDOW_H
