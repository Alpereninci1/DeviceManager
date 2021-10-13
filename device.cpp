#include "device.h"

#include <QMutexLocker>
#include <QStatusBar>
#include <QModbusReply>
#include <QObject>
#include <QStatusBar>


Device::Device()
{

}

bool Device::createNewGroup(QString file)
{
    QJsonArray device_array;
    QJsonObject device_object;


    device_object.insert(QString("devices"), QJsonValue(device_array));

    QJsonDocument json_doc(device_object);

    QFile wFile(file);
    wFile.open(QIODevice::WriteOnly|QIODevice::Text);
    wFile.write(json_doc.toJson());
    wFile.close();

    this->file = file;

    return true;
}

bool Device::parseDevices(QString file)
{
    QFile inFile(file);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }

    QJsonObject rootObj = doc.object();
    QJsonArray ptsArray = rootObj.value("devices").toArray();

    foreach(const QJsonValue &val, ptsArray){
        QString  location = val.toObject().value("location").toString();
        QString ip = val.toObject().value("ip").toString();
        QString port = val.toObject().value("port").toString();
        QString slave_id = val.toObject().value("slave id").toString();
        QString manage_port =val.toObject().value("manage port").toString();
        QString uptime=val.toObject().value("uptime").toString();

        DeviceType* tmp = new DeviceType;
        tmp->page_index = 1;
        tmp->location = location;
        tmp->ip = ip;
        tmp->port=port;
        tmp->slave_id=slave_id;
        tmp->manage_port=manage_port;
        tmp->uptime=uptime;
        tmp->layout = new QHBoxLayout();
        tmp->cb_select = new QCheckBox;
        tmp->label_location = new QLabel;
        tmp->label_ip = new QLabel;
        tmp->label_port = new QLabel;;
        tmp->label_slave_id = new QLabel;
        tmp->label_manage_port=new QLabel;
        tmp->label_uptime = new QLabel;
        tmp->label_status = new QLabel;

        tmp->label_location->setText(location);
        tmp->label_ip->setText(ip);
        tmp->label_port->setText(port);
        tmp->label_slave_id->setText(slave_id);
        tmp->label_manage_port->setText(manage_port);
        tmp->label_uptime->setText(uptime);
        tmp->label_status->setText("Not Connected");
        tmp->label_status->setVisible(false);

        devices.push_back(tmp);
    }

    this->file = file;

    return true;
}

bool Device::addDevice(QString location, QString ip, QString port, QString slave_id,QString manage_port)
{
    { /* Add to device vector */
        DeviceType* tmp = new DeviceType;
        tmp->page_index = 1;
        tmp->location = location;
        tmp->ip = ip;
        tmp->port = port;
        tmp->slave_id =slave_id;
        tmp->manage_port=manage_port;
        tmp->layout = new QHBoxLayout();
        tmp->cb_select = new QCheckBox;
        tmp->label_location = new QLabel;
        tmp->label_ip = new QLabel;
        tmp->label_port = new QLabel;
        tmp->label_slave_id = new QLabel;
        tmp->label_status = new QLabel;
        tmp->label_manage_port=new QLabel;

#if 0
        tmp->cb_select->setStyleSheet("background-color:red;");
        tmp->label_location->setStyleSheet("background-color:green;");
        tmp->label_ip->setStyleSheet("background-color:blue;");
        tmp->label_port->setStyleSheet("background-color:gray;");
        tmp->label_slave_id->setStyleSheet("background-color:yellow;");
        tmp->label_df->setStyleSheet("background-color:orange;");
        tmp->label_status->setStyleSheet("background-color:pink;");

#endif

        tmp->label_location->setText(location);
        tmp->label_ip->setText(ip);
        tmp->label_port->setText(port);
        tmp->label_slave_id->setText(slave_id);
        tmp->label_manage_port->setText(manage_port);
        tmp->label_status->setText("Not Connected");
        tmp->label_status->setVisible(false);

        devices.push_back(tmp);
    }

    { /* Add to json file */
        QFile inFile(file);
        inFile.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray data = inFile.readAll();
        inFile.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull()) {
            qDebug() << "Parse failed";
        }

        QJsonObject rootObj = doc.object();
        QJsonArray ptsArray = rootObj.value("devices").toArray();

        QJsonObject tmp;
        tmp.insert("location", location);
        tmp.insert("ip", ip);
        tmp.insert("port", port);
        tmp.insert("slave id", slave_id);
        tmp.insert("manage port",manage_port);

        ptsArray.append(tmp);
        rootObj.insert("devices", ptsArray);

        QJsonDocument tmpDoc(rootObj);

        QFile wFile(file);
        wFile.open(QIODevice::WriteOnly|QIODevice::Truncate);
        wFile.write(tmpDoc.toJson());
        wFile.close();
    }

    return true;
}

bool Device::removeDevices(void)
{
    QFile inFile(file);
    inFile.open(QIODevice::ReadOnly|QIODevice::Text);
    QByteArray data = inFile.readAll();
    inFile.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "Parse failed";
    }

    QJsonObject rootObj = doc.object();
    QJsonArray ptsArray = rootObj.value("devices").toArray();

    for(size_t i = 0; i < devices.size(); i++){
        if(devices.at(i)->cb_select->isChecked() == true){
            delete devices.at(i)->layout;
            delete devices.at(i)->cb_select;
            delete devices.at(i)->label_location;
            delete devices.at(i)->label_ip;
            delete devices.at(i)->label_port;
            delete devices.at(i)->label_slave_id;
            delete devices.at(i)->label_manage_port;
            delete devices.at(i)->label_uptime;
            delete devices.at(i)->label_status;
            delete devices.at(i);
            devices.erase(devices.begin() + i);
            ptsArray.removeAt(i);

            --i;
        }
    }

    rootObj.insert("devices", ptsArray);

    QJsonDocument tmpDoc(rootObj);

    QFile wFile(file);
    wFile.open(QIODevice::WriteOnly|QIODevice::Truncate);
    wFile.write(tmpDoc.toJson());
    wFile.close();

    return true;
}

void Device::reset()
{
    file.clear();

    while(!devices.empty()) {

        if (devices.back()->run && devices.back()->run->isRunning()) {
            devices.back()->run->terminate();
        }

        delete devices.back()->layout;
        delete devices.back()->cb_select;
        delete devices.back()->label_location;
        delete devices.back()->label_ip;
        delete devices.back()->label_port;
        delete devices.back()->label_slave_id;
        delete devices.back()->label_manage_port;
        delete devices.back()->label_uptime;
        delete devices.back()->label_status;
        delete devices.back();
        devices.pop_back();
    }
}


