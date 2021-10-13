bool any_selected=false;
    if(!modbusDevice)
       return;
    for (auto & d: device.devices){
        if( d->cb_select->isChecked()) {
            any_selected = true;
            break;
        }
    }
    if (!any_selected) {
        QToolTip::showText(ui->pbConnectDevices->mapToGlobal(QPoint()), "Cihaz seçin.", ui->pbConnectDevices);
        return;
    }

    if(!modbusDevice){
        statusBar()->showMessage("cihaz yok");
        ui->pbConnectDevices->setDisabled(true);
        return;
    }

    if(modbusDevice->state()!=QModbusDevice::ConnectedState){

        for(auto & d: device.devices){

            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, d->label_port->text());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, d->label_ip->text());

        }

        if (isConnected()==false) {
            statusBar()->showMessage(tr("Connect failed: ") + modbusDevice->errorString(), 5000);
        }


    }
    else{

        modbusDevice->disconnectDevice();
    }

    goToManagePage();



}
