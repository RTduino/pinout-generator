#include "pininfowidget.h"
#include "ui_pininfowidget.h"

PinInfoWidget::PinInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinInfoWidget)
{
    ui->setupUi(this);
    rtduino = RTduinoConfig::getInstance();

    ui->comboBox_devfunc->clear();
    ui->comboBox_rttport->clear();
    ui->comboBox_rttpin->clear();

    for (int i = 0; i < rtduino->getNowMcuSeries().iofunctions.size(); i++)
    {
        ui->comboBox_devfunc->addItem(rtduino->getNowMcuSeries().iofunctions.at(i).function);
    }

    ui->comboBox_rttport->addItems(rtduino->getNowMcuSeries().iopininfo.ports);
    ui->comboBox_rttpin->addItems(rtduino->getNowMcuSeries().iopininfo.pins);
}

PinInfoWidget::~PinInfoWidget()
{
    delete ui;
}

void PinInfoWidget::initaddWidget()
{
    ui_type = "add";

    ui->label_title->setText("添加引脚");
    ui->pushButton_save->setText("添加");
}

void PinInfoWidget::initinsertWidget(pin_info_t sel_pin)
{
    ui_type = "insert";

    select_pin = sel_pin.pin_arduino;
    
    ui->label_title->setText("插入引脚");
    ui->pushButton_save->setText("插入");
}

void PinInfoWidget::initchangeWidget(pin_info_t sel_pin)
{
    ui_type = "change";

    select_pin = sel_pin.pin_arduino;

    ui->label_title->setText("修改引脚");
    ui->comboBox_devfunc->setCurrentText(sel_pin.pin_function);
    ui->pushButton_save->setText("修改");
    ui->comboBox_usefunc->setCurrentText(sel_pin.pin_iomap);
    ui->comboBox_devchnl->setCurrentText(sel_pin.pin_channel);
    ui->comboBox_devname->setCurrentText(sel_pin.pin_name);
    ui->comboBox_rttpin->setCurrentText(sel_pin.pin_rttpin);
    ui->comboBox_rttport->setCurrentText(sel_pin.pin_rttport);
    ui->comboBox_vol5v->setCurrentText(sel_pin.pin_5vallow ? "Yes" : "No");
}

void PinInfoWidget::setVisibleUi(QString func)
{
    if (func == "")
    {
        qDebug() << "function is none.";
        return;
    }

    io_function_t iofunction;
    iofunction = rtduino->getIoFunctionConfig(func);
    if(!iofunction.channels.isEmpty())
    {
        ui->label_devchnl->setVisible(true);
        ui->comboBox_devchnl->setVisible(true);
        ui->comboBox_devchnl->clear();
        ui->comboBox_devchnl->addItems(iofunction.channels);
    }
    else
    {
        ui->label_devchnl->setVisible(false);
        ui->comboBox_devchnl->setVisible(false);
        ui->comboBox_devchnl->clear();
    }

    if(!iofunction.names.isEmpty())
    {
        ui->label_devname->setVisible(true);
        ui->comboBox_devname->setVisible(true);
        ui->comboBox_devname->clear();
        ui->comboBox_devname->addItems(iofunction.names);
    }
    else
    {
        ui->label_devname->setVisible(false);
        ui->comboBox_devname->setVisible(false);
        ui->comboBox_devname->clear();
    }

    if(!iofunction.ioremaps.isEmpty())
    {
        ui->label_usefunc->setVisible(true);
        ui->comboBox_usefunc->setVisible(true);
        ui->comboBox_usefunc->clear();
        ui->comboBox_usefunc->addItems(iofunction.ioremaps);
    }
    else
    {
        ui->label_usefunc->setVisible(false);
        ui->comboBox_usefunc->setVisible(false);
        ui->comboBox_usefunc->clear();
    }

    ui->comboBox_devfunc->setCurrentText(func);
}

void PinInfoWidget::on_comboBox_devfunc_currentIndexChanged(const QString &arg1)
{
    setVisibleUi(arg1);
}


void PinInfoWidget::on_pushButton_save_clicked()
{
    qDebug() << select_pin;
    pin_info_t _pin_info;
    _pin_info.pin_function = ui->comboBox_devfunc->currentText();
    _pin_info.pin_iomap = ui->comboBox_usefunc->currentText();
    _pin_info.pin_channel = ui->comboBox_devchnl->currentText();
    _pin_info.pin_name = ui->comboBox_devname->currentText();
    _pin_info.pin_rttpin = ui->comboBox_rttpin->currentText();
    _pin_info.pin_rttport = ui->comboBox_rttport->currentText();
    _pin_info.pin_5vallow = (ui->comboBox_vol5v->currentText() == "Yes");

    if (ui_type == "add")
    {
        rtduino->addPinInfo(_pin_info);
    }
    else if (ui_type == "insert")
    {
        rtduino->insertPinInfo(select_pin, _pin_info);
    }
    else
    {
        rtduino->changePinInfo(select_pin, _pin_info);
    }

    emit refreshTreeWidget();

    if(ui_type == "insert" || ui_type == "change")
    {
        close();
    }
}

void PinInfoWidget::on_comboBox_devchnl_currentIndexChanged(const QString &arg1)
{
    if (ui->comboBox_devfunc->currentText() == "ADC")
    {
        if (!QString(arg1).contains(QRegExp("^\\d+$")))
        {
            ui->label_vol5v->setVisible(false);
            ui->comboBox_vol5v->setVisible(false);
            ui->label_rttportpin->setVisible(false);
            ui->comboBox_rttport->setVisible(false);
            ui->comboBox_rttpin->setVisible(false);

            ui->comboBox_vol5v->clear();
            ui->comboBox_rttport->clear();
            ui->comboBox_rttpin->clear();
        }
        else
        {
            ui->label_vol5v->setVisible(true);
            ui->comboBox_vol5v->setVisible(true);
            ui->label_rttportpin->setVisible(true);
            ui->comboBox_rttport->setVisible(true);
            ui->comboBox_rttpin->setVisible(true);

            QString vol5v, port, pin;
            vol5v = ui->comboBox_vol5v->currentText();
            port = ui->comboBox_rttport->currentText();
            pin = ui->comboBox_rttpin->currentText();

            ui->comboBox_vol5v->clear();
            ui->comboBox_rttport->clear();
            ui->comboBox_rttpin->clear();

            ui->comboBox_vol5v->addItem("Yes");
            ui->comboBox_vol5v->addItem("No");
            ui->comboBox_rttport->addItems(rtduino->getNowMcuSeries().iopininfo.ports);
            ui->comboBox_rttpin->addItems(rtduino->getNowMcuSeries().iopininfo.pins);

            ui->comboBox_vol5v->setCurrentText(vol5v);
            ui->comboBox_rttport->setCurrentText(port);
            ui->comboBox_rttpin->setCurrentText(pin);
        }
    }
}
