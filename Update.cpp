#include "Update.h"
#include "mt_types.h"
#include "device.h"
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <QModbusDataUnit>


#define VERSION_CONTROL     	0x00
#define UPDATE_START        	0x01
#define UPDATE_DATA         	0x02
#define UPDATE_END          	0x03
#define UPDATE_ACK          	0xFF

#define TYPE_OFFSET         	0
#define ID_OFFSET           	1
#define LENGTH_OFFSET       	3
#define DATA_OFFSET         	5

#define PACKET_LEN_HEADER       5
#define PACKET_LEN_VERSION_REQ  5
#define PACKET_LEN_VERSION_RESP 7
#define PACKET_LEN_START_REQ    9
#define PACKET_LEN_MAX_REQ     	261 /* For firmware data; 256(sector size) + 5(header) */
#define PACKET_LEN_MAX_RES    	PACKET_LEN_VERSION_RESP /* Response for version request */
#define PACKET_LEN_END_REQ      5
#define PACKET_LEN_ACK          5



c8 buffer[PACKET_LEN_MAX_REQ];


/** @brief Gonderilecek paketi hazir eder
 *
 *  @param data Var ise payload alani
 *  @param data_len Var ise payload boyu
 *  @param package_id Kontrol amacli package_id, alinacak ACK paketi
 * 		icindeki ile ayni olmali, her gonderimde 1 arttırılabilir.
 *  @param type Paket turu
 *
 */
void Update::prepare_package(c8* data, u16 data_len, u16 package_id, u8 type)
{
    buffer[TYPE_OFFSET] = type;
    buffer[ID_OFFSET] = (package_id >> 8) & 0xFF;
    buffer[ID_OFFSET + 1] = package_id & 0xFF;
    buffer[LENGTH_OFFSET] = (data_len >> 8) & 0xFF;
    buffer[LENGTH_OFFSET + 1] = data_len & 0xFF;
    if(data_len > 0)
        memcpy(&buffer[DATA_OFFSET], data, data_len);
}

QString Update::getVersion()
{
    prepare_package(NULL, 0, package_id, VERSION_CONTROL);
    socket.write(buffer, PACKET_LEN_VERSION_REQ);
    socket.waitForBytesWritten();
    read_len = 0;
    do{

        socket.waitForReadyRead();
        read_len +=socket.read(&read_buffer[read_len], PACKET_LEN_VERSION_RESP - read_len);
    }while(read_len < PACKET_LEN_VERSION_RESP);
    char s= read_buffer[DATA_OFFSET];
    char s2= read_buffer[DATA_OFFSET + 1];

    QString str;

    str=s+s2;

    return str;


}


Update::Update()
{

}

void Update::softwareUpdate(DeviceType *device,QString filename){

    qDebug()<<"ip:"<<device->ip;
    qDebug()<<"Manage port"<<device->manage_port;
    qDebug()<<"filename:"<<filename;


        socket.connectToHost(device->ip,device->manage_port.toInt());
        if (!socket.waitForConnected(30000)) {

            qDebug()<<"Error"<<socket.errorString();
            return;
        }
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)){
            qDebug()<<"file not open";
            return;
        }

        qDebug("File size %lldKB(%lld Bytes)\r\n", (file.size() / 1024), file.size());

        /* Get software version */

        prepare_package(NULL, 0, package_id, VERSION_CONTROL);
        socket.write(buffer, PACKET_LEN_VERSION_REQ);
        socket.waitForBytesWritten();
        read_len = 0;
        do{

            socket.waitForReadyRead();
            read_len +=socket.read(&read_buffer[read_len], PACKET_LEN_VERSION_RESP - read_len);
        }while(read_len < PACKET_LEN_VERSION_RESP);
        qDebug("File Version: %d.%d\r\n", read_buffer[DATA_OFFSET], read_buffer[DATA_OFFSET + 1]);

        /* Start update */
        package_id++;
        data[0] = (file.size() << 24) & 0xFF;
        data[1] = (file.size() << 16) & 0xFF;
        data[2] = (file.size() << 8) & 0xFF;
        data[3] = file.size() & 0xFF;
        prepare_package(data, 4, package_id, UPDATE_START);
        socket.write( buffer, PACKET_LEN_START_REQ);
        socket.waitForBytesWritten();
        qDebug()<<"bbb";
        read_len = 0;
        do{
            socket.waitForReadyRead();
            read_len +=socket.read(&read_buffer[read_len], PACKET_LEN_ACK - read_len);
        }while(read_len < PACKET_LEN_ACK);

        /* Send firmware */
        qDebug()<<"ccccc";
        firmware_sent = 0;
        do{
             qDebug()<<"dddddd";
            data_len = file.read(data, sizeof(data));
            if(data_len > 0){
                package_id++;
                prepare_package(data, data_len, package_id, UPDATE_DATA);
                socket.write( buffer, data_len + PACKET_LEN_HEADER);
                socket.waitForBytesWritten();
                read_len = 0;
                do{
                    socket.waitForReadyRead();
                    read_len +=socket.read(&read_buffer[read_len], PACKET_LEN_ACK - read_len);
                }while(read_len < PACKET_LEN_ACK);
                qDebug()<<"aaaa";
                firmware_sent += data_len;
                qDebug("%lld%%", (100 * firmware_sent) / file.size());
            }
        }while(data_len > 0);

        file.close();
        qDebug("100%%\n");

        package_id++;
        prepare_package(NULL, 0, package_id, UPDATE_END);

        socket.write( buffer,  PACKET_LEN_END_REQ);
        socket.waitForBytesWritten();
        read_len = 0;
        do{
            socket.waitForReadyRead();
            read_len += socket.read(&read_buffer[read_len], PACKET_LEN_ACK - read_len);
        }while(read_len < PACKET_LEN_ACK);
        qDebug("Update finished\n");
        socket.close();
        sleep(3);

}







