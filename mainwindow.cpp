#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QModbusTcpClient>
#include <QToolTip>
#include <QObject>
#include <QDebug>
#include <QLabel>
#include <QWidget>
#include <QLayout>
#include <QInputDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QSplitter>
#include "device.h"
#include <QUrl>
#include <QDebug>
#include <QModbusDataUnit>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->cbAllDev->hide();
    ui->stackedWidget->setCurrentWidget(ui->page_group);

    ui->splitter->setStretchFactor(0, 2);
    ui->splitter->setStretchFactor(1, 1);

    ui->tbBack->setVisible(false);


}


void MainWindow::initActions(){

    connect(ui->pbConnectDevices, &QPushButton::clicked,
            this, &MainWindow::on_pbConnectDevices_clicked);
}

MainWindow::~MainWindow()
{

    delete ui;
}


void MainWindow::showDevices(){//cihazları göster

    if(device.devices.size() > 1 )
        ui->cbAllDev->show();
    else
        ui->cbAllDev->hide();

    for(size_t i = 0; i < device.devices.size(); i++){
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->cb_select);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_location);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_ip);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_slave_id);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_manage_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_status);

        ui->vlDevices->insertLayout(ui->vlDevices->count(), device.devices.at(i)->layout);
    }

    ui->tbBack->setVisible(true);

}

void MainWindow::on_pbCreateGroup_clicked()//cihaz grubu oluştur
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Yeni Grup"),
                                             tr("Grup Adı:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty()){
        device.createNewGroup(text + ".json") ;
        ui->stackedWidget->setCurrentIndex(1);
    }
}


void MainWindow::on_pbOpenGroup_clicked()//cihaz grubu seç
{
    QString file = QFileDialog::getOpenFileName(this,
                                                tr("Grup seç"),
                                                "./",
                                                tr("JSON(*.json);; All files (*.*)"));
    if (file.isNull()) {
        return;
    }

    device.parseDevices(file);
    ui->stackedWidget->setCurrentIndex(1);
    showDevices();
    onConnect();

}


void MainWindow::on_tbBack_clicked()//geri git
{
    qDebug() << "ui->stackedWidget->currentIndex(): " << ui->stackedWidget->currentIndex();
    switch (ui->stackedWidget->currentIndex()) {
    case 0:
        ui->tbBack->setVisible(false);
        break;

    case 1:
        backToDeviceGroup();
        break;

    case 2:
        backToDeviceList();
        break;
    }

}


void MainWindow::on_cbAllDev_clicked()//tüm cihazları seç all
{
    if(ui->cbAllDev->isChecked() == true){
        for(size_t i = 0; i < device.devices.size(); i++){
            device.devices.at(i)->cb_select->setChecked(true);
        }
    }else{
        for(size_t i = 0; i < device.devices.size(); i++){
            device.devices.at(i)->cb_select->setChecked(false);
        }
    }
}


void MainWindow::on_pbAddDevice_clicked()//cihaz ekle
{

    QDialog dialog;
    QFormLayout *formLayout = new QFormLayout(&dialog);
    QDialogButtonBox* buttonBox =new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    /* These are for dialog, not to add devices vector, widget are added at addDevice fuction */
    QLineEdit *location = new QLineEdit(&dialog);
    QLineEdit *ip_addr = new QLineEdit(&dialog);
    QLineEdit *port = new QLineEdit(&dialog);
    QLineEdit *slave_id = new QLineEdit(&dialog);
    QLineEdit *manage=new QLineEdit(&dialog);

    formLayout->addRow(tr("&Konum:"), location);
    formLayout->addRow(tr("&IP Adresi:"), ip_addr);
    formLayout->addRow(tr("&Port:"), port);
    formLayout->addRow(tr("&Slave ID:"), slave_id);
    formLayout->addRow(tr("&Manage Port"),manage);
    formLayout->addRow(buttonBox);
    QObject::connect(buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));

    if (dialog.exec() == QDialog::Accepted) {
        device.addDevice(location->text(), ip_addr->text(), port->text(), slave_id->text(),manage->text());

        int i =  device.devices.size() - 1;
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->cb_select);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_location);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_ip);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_slave_id);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_manage_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_status);

        ui->vlDevices->insertLayout(ui->vlDevices->count(), device.devices.at(i)->layout);

        if(device.devices.size() > 1 )
            ui->cbAllDev->show();
        else
            ui->cbAllDev->hide();


    }

    delete location;
    delete ip_addr;
    delete port;
    delete slave_id;
    delete manage;
    delete buttonBox;
    delete formLayout;
}


void MainWindow::on_pbRemoveDevice_clicked()//cihaz sil
{
    for(size_t i = 0; i < device.devices.size(); i++){
        if(device.devices.at(i)->cb_select->isChecked() == true){
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->cb_select);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_location);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_ip);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_port);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_slave_id);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_manage_port);
            device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_status);

            ui->vlDevices->removeItem(device.devices.at(i)->layout);
        }
    }
    device.removeDevices();
}



void MainWindow::backToDeviceGroup(){

    device.reset();

    ui->stackedWidget->setCurrentIndex(0);
    ui->tbBack->setVisible(false);

}

void MainWindow::backToDeviceList(){


    for(size_t i = 0; i < device.devices.size(); i++){

        device.devices.at(i)->page_index = 1;
        device.devices.at(i)->cb_select->setChecked(false);

        // remove parent layout
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->cb_select);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_location);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_ip);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_port);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_slave_id);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_manage_port);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_uptime);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_status);
        ui->vlManage->removeItem(device.devices.at(i)->layout);

        // re-init
        device.devices.at(i)->label_location->setVisible(true);
        device.devices.at(i)->label_ip->setVisible(true);
        device.devices.at(i)->label_port->setVisible(true);
        device.devices.at(i)->label_slave_id->setVisible(true);
        device.devices.at(i)->label_manage_port->setVisible(true);
        device.devices.at(i)->label_uptime->setVisible(true);
        device.devices.at(i)->label_status->setVisible(true);

        if(isConnected()==true && device.devices.at(i)->cb_select->isChecked()==true){

             device.devices.at(i)->label_status->setText("<font color='green'>Connected</font>");

        }
        else{
            device.devices.at(i)->label_status->setText("<font color='red'>Not Connected</font>");
        }
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->cb_select);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_location);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_ip);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_slave_id);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_manage_port);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_uptime);
        device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_status);
        ui->vlDevices->insertLayout(ui->vlDevices->count(), device.devices.at(i)->layout);

        if(device.devices.at(i)->cb_select->isChecked()==true){
        device.devices.at(i)->run = QThread::create([i,this] () {
            device.devices.at(i)->modbusDevice->disconnectDevice();
            QThread::msleep(1000);
        });
        device.devices.at(i)->run->start();

        }
    }

    ui->cbAllDev->setChecked(false);
    ui->stackedWidget->setCurrentIndex(1);
    ui->tbBack->setVisible(true);
    statusBar()->clearMessage();

}


void MainWindow::on_cbAllMan_clicked()
{

    if(ui->cbAllMan ->isChecked() == true){
        for(size_t i = 0; i < device.devices.size(); i++){
            device.devices.at(i)->cb_select->setChecked(true);
        }
    }else{
        for(size_t i = 0; i < device.devices.size(); i++){
            device.devices.at(i)->cb_select->setChecked(false);
        }
    }
}


void MainWindow::on_pbSoftware_clicked(){

    bool any_selected=false;
    for (auto & d: device.devices){
        if((d->page_index == 2) &&  d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->pbSoftware->mapToGlobal(QPoint()), "Cihaz seçin.", ui->pbSoftware);
        return;
    }

       QString filename;
       filename = QFileDialog::getOpenFileName(this, "Dosya seç", "./");
       if (filename.isNull()) {
          return;
       }
       for(size_t i = 0; i < device.devices.size(); i++){
           if((device.devices.at(i)->page_index == 2) &&  device.devices.at(i)->cb_select->isChecked() == true){
               device.devices.at(i)->run->wait();
               device.devices.at(i)->run = QThread::create([this, i, filename] () {
                   u.softwareUpdate(device.devices.at(i), filename);
               });
               device.devices.at(i)->run->start();
           }
       }

}


void MainWindow::on_pbConnectDevices_clicked()
{
    bool any_selected=false;

    for (auto & d: device.devices){
        if(d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->pbConnectDevices->mapToGlobal(QPoint()), "Cihaz seçin.", ui->pbConnectDevices);
        return;
    }

    for(size_t i=0;i<device.devices.size();i++){

        if(!device.devices.at(i)->modbusDevice){
            statusBar()->showMessage("cihaz yok");
            ui->pbConnectDevices->setDisabled(true);
            return;
        }


        if(device.devices.at(i)->modbusDevice->state()!=QModbusDevice::ConnectedState){

            for (auto & d: device.devices){
                if(d->cb_select->isChecked() == true){

                    device.devices.at(i)->modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, d->label_port->text());
                    device.devices.at(i)->modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, d->label_ip->text());

                }
            }
            if (device.devices.at(i)->modbusDevice->connectDevice()) {

                goToManagePage();

            }
            else{
                statusBar()->showMessage(tr("Connect failed: ") + device.devices.at(i)->modbusDevice->errorString(), 5000);
            }


        }else{

            device.devices.at(i)->modbusDevice->disconnectDevice();
        }



    }

}

void MainWindow::goToManagePage()
{

    for(size_t i = 0; i < device.devices.size(); i++){
        /* Add selected widgets to page_manage and deselect*/
        if(device.devices.at(i)->cb_select->isChecked() == true){
            device.devices.at(i)->page_index = 2;
            device.devices.at(i)->cb_select->setChecked(false);
        }
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->cb_select);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_location);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_ip);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_port);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_slave_id);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_manage_port);
        device.devices.at(i)->layout->removeWidget(device.devices.at(i)->label_status);
        ui->vlDevices->removeItem(device.devices.at(i)->layout);

        if(device.devices.at(i)->page_index == 2){
            device.devices.at(i)->label_port->setVisible(true);
            device.devices.at(i)->label_slave_id->setVisible(true);
            device.devices.at(i)->label_status->setVisible(true);
            if(isConnected()==true){
                device.devices.at(i)->label_status->setText("<font color='green'>Connected</font>");
            }
            else{
                device.devices.at(i)->label_status->setText("<font color='red'>Not Connected</font>");
            }
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->cb_select);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_location);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_ip);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_port);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_slave_id);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_manage_port);
            device.devices.at(i)->layout->addWidget(device.devices.at(i)->label_status);

            ui->vlManage->insertLayout(ui->vlManage->count(), device.devices.at(i)->layout);

            if(device.devices.at(i)->cb_select->isChecked()==true){
            device.devices.at(i)->run = QThread::create([i,this] {
                device.devices.at(i)->modbusDevice->connectDevice();
                qDebug()<<"thread " << i <<QThread::currentThreadId();
                QThread::msleep(1000);

            });

            device.devices.at(i)->run->start();
           }

        }

        ui->cbAllMan->setChecked(false);
        ui->stackedWidget->setCurrentIndex(2);
        ui->tbBack->setVisible(true);

    }
}




void MainWindow::onConnect(){


    for(size_t i=0;i<device.devices.size();i++){

        if (device.devices.at(i)->modbusDevice) {
            device.devices.at(i)->modbusDevice->disconnectDevice();
            delete device.devices.at(i)->modbusDevice;
            device.devices.at(i)->modbusDevice = nullptr;
        }

        device.devices.at(i)->modbusDevice = new QModbusTcpClient(this);

        connect(device.devices.at(i)->modbusDevice, &QModbusClient::errorOccurred, [i,this](QModbusDevice::Error) {

            statusBar()->showMessage(device.devices.at(i)->modbusDevice->errorString(), 5000);

        });
    }


}

/*
void MainWindow::on_disconnect_device_clicked()
{
    bool any_selected=false;

    for (auto & d: device.devices){
        if(d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->disconnect_device->mapToGlobal(QPoint()), "Cihaz seçin.", ui->disconnect_device);
        return;
    }

    for(size_t i = 0; i < device.devices.size(); i++){
       if (device.devices.at(i)->modbusDevice && device.devices.at(i)->cb_select->isChecked()==true){

           //device.devices.at(i)->modbusDevice->disconnectDevice();
           backToDeviceList();
           statusBar()->showMessage("bağlantı kesildi.");

        }


   }
}

*/
void MainWindow::on_resetButton_clicked()
{

    bool any_selected=false;

    for (auto & d: device.devices){
        if((d->page_index == 2) &&  d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->resetButton->mapToGlobal(QPoint()), "Cihaz seçin.", ui->resetButton);
        return;
    }

    statusBar()->clearMessage();

    for(size_t i =0 ;i<device.devices.size();i++){

        QModbusDataUnit writeUnit = writeRequest();

        for (int i = 0, total = int(writeUnit.valueCount()); i < total; ++i) {

                writeUnit.setValue(1,1);


        if (auto *reply = device.devices.at(i)->modbusDevice->sendWriteRequest(writeUnit, 1)) {
            if (!reply->isFinished()) {
                connect(reply, &QModbusReply::finished, this, [this, reply]() {
                    if (reply->error() == QModbusDevice::ProtocolError) {
                        statusBar()->showMessage(tr("Write response error: %1 (Mobus exception: 0x%2)")
                            .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1, 16),
                            5000);
                    } else if (reply->error() != QModbusDevice::NoError) {
                        statusBar()->showMessage(tr("Write response error: %1 (code: 0x%2)").
                            arg(reply->errorString()).arg(reply->error(), -1, 16), 5000);
                    }
                    reply->deleteLater();
                });
            } else {
                // broadcast replies return immediately
                reply->deleteLater();
            }
        } else {
            statusBar()->showMessage(tr("Write error: ") + device.devices.at(i)->modbusDevice->errorString(), 5000);
        }

      }

    }

}



void MainWindow::on_uptime_clicked()
{
    bool any_selected=false;
    for (auto & d: device.devices){
        if((d->page_index == 2) &&  d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->uptime->mapToGlobal(QPoint()), "Cihaz seçin.", ui->uptime);
        return;
    }

    for(size_t i=0;i<device.devices.size();i++){

        if(!(device.devices.at(i)->page_index == 2 &&  device.devices.at(i)->cb_select->isChecked())){

            continue;
        }

        QVector<quint16> data(1); // One Byte
        QModbusDataUnit adu(QModbusDataUnit::InputRegisters, 2, data);
        if (!device.devices.at(i)->modbusDevice){
            return;
        statusBar()->showMessage("bağlanmadı");
        }

        if (auto *reply = device.devices.at(i)->modbusDevice->sendReadRequest(adu,1)) {

            if (!reply->isFinished())

               connect(reply, &QModbusReply::finished, this, &MainWindow::reading);

            else
               delete reply; // broadcast replies return immediately

           } else {

             statusBar()->showMessage(tr("Read error: ") + device.devices.at(i)->modbusDevice->errorString(), 5000);
           }

    }


}

void MainWindow::on_deviceId_clicked()
{

    bool any_selected=false;


    for (auto & d: device.devices){
        if((d->page_index == 2) &&  d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->deviceId->mapToGlobal(QPoint()), "Cihaz seçin.", ui->deviceId);
        return;
    }

    for(size_t i=0;i<device.devices.size();i++){

        if(!(device.devices.at(i)->page_index == 2 &&  device.devices.at(i)->cb_select->isChecked())){

            continue;
        }


        QVector<quint16> data(1); // One Byte
        QModbusDataUnit adu(QModbusDataUnit::InputRegisters, 3, data);

        if (!device.devices.at(i)->modbusDevice){
            return;
            statusBar()->showMessage("bağlanmadı");
        }
        if (auto *reply =  device.devices.at(i)->modbusDevice->sendReadRequest(adu,1)) {

            if (!reply->isFinished())

                connect(reply, &QModbusReply::finished, this, &MainWindow::reading);
            else

                delete reply;
            // broadcast replies return immediately
        } else {

            statusBar()->showMessage(tr("Read error: ") + device.devices.at(i)->modbusDevice->errorString(), 5000);
        }

    }



}


void MainWindow::reading(){

    qDebug()<<"read";
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (int i = 0, total = int(unit.valueCount()); i < total; ++i) {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + i)
                    .arg(QString::number(unit.value(i),
                                         unit.registerType() <= QModbusDataUnit::InputRegisters ? 10 : 16));

            ui->listWidget->addItem(entry);


        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                 arg(reply->errorString()).
                                 arg(reply->error(), -1, 16), 5000);
    }

    reply->deleteLater();



}



void MainWindow::on_VersionControl_clicked()
{
    bool any_selected=false;
    for (auto & d: device.devices){
        if((d->page_index == 2) &&  d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->VersionControl->mapToGlobal(QPoint()), "Cihaz seçin.", ui->VersionControl);
        return;
    }

     ui->listWidget->addItem("Versiyon kontrol: "+u.getVersion());


}

bool MainWindow::isConnected()
{
     for(size_t i=0;i<device.devices.size();i++){

         if(device.devices.at(i)->modbusDevice->state() == QModbusDevice::UnconnectedState){

              //qDebug()<<"bağlı değil";
               ui->pbConnectDevices->setText(tr("Bağlan"));
              return false;
          }
          else if(device.devices.at(i)->modbusDevice->state() == QModbusDevice::ConnectedState){

              //qDebug()<<"bağlı";
              ui->pbConnectDevices->setText(tr("Bağlandı"));
              return true;
          }


     }

     return true;

}


void MainWindow::on_pushButton_clicked()//clear
{
    ui->listWidget->clear();
}


QModbusDataUnit MainWindow::writeRequest() const
{
    const auto table =
        static_cast<QModbusDataUnit::RegisterType>(Coils);


    return QModbusDataUnit(table, 0, 2);
}

