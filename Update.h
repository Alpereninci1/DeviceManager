#ifndef UPDATE_H
#define UPDATE_H
#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <device.h>
#include "mt_types.h"
#include <QFile>
#include <QIODevice>
#include <QFileDevice>
#include <QFileDialog>



class Update
{

public:
    Update();
    void softwareUpdate(DeviceType*,QString file);
    void prepare_package(c8* data, u16 data_len, u16 package_id, u8 type);
    QString getVersion();


private:
    u16 package_id = 0;
    c8 data[256];
    u16 data_len;
    c8 read_buffer[9];
    u16 read_len;
    u32 firmware_sent;
    QTcpSocket socket;


};

#endif // UPDATE_H
