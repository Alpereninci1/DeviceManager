#ifndef DEVICE_H
#define DEVICE_H

#include <QString>
#include <QFileDialog>
#include <QCheckBox>
#include <QLabel>
#include <QDebug>
#include <QLayout>
#include <QTextBrowser>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QMutex>
#include <QThread>
#include <thread>
#include <QModbusDataUnit>
#include <QModbusDevice>
#include <QDialogButtonBox>
#include <QModbusReply>
#include <QObject>
#include <QStatusBar>


class QModbusClient;
class QModbusReply;


namespace Ui {
class MainWindow;

}

QT_END_NAMESPACE

using namespace std;

struct DeviceType {
    int page_index; /*1 device add/remove, 2 device manage*/
    QThread * run = NULL;
    QString location;
    QString ip;
    QString port;
    QString slave_id;
    QString manage_port;
    QString uptime;
    QHBoxLayout* layout;
    QCheckBox *cb_select;
    QLabel *label_location;
    QLabel *label_ip;
    QLabel *label_port;
    QLabel *label_slave_id;
    QLabel *label_manage_port;
    QLabel *label_uptime;
    QLabel *label_status;
    QModbusClient *modbusDevice=nullptr;
    QString command_response;
};

class Device
{
public:
    Device();
    bool createNewGroup(QString file);
    bool parseDevices(QString file);
    bool addDevice(QString location, QString ip, QString port, QString slave_id,QString manage_port);
    bool removeDevices(void);
    void reset();



    //QModbusClient *modbusDevice=nullptr;
    QTextBrowser textBrowser;
    vector<DeviceType*> devices;
    unsigned size;


private:
    QString file;

};

#endif // DEVICE_H
