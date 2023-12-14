#include "funcwidget.h"
#include "ui_funcwidget.h"

FuncWidget::FuncWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FuncWidget)
{
    ui->setupUi(this);
    ui->comboBox_code->addItem("Hello Arduino");
    ui->comboBox_code->addItem("LED Blinking");
    rtduino = RTduinoConfig::getInstance();
}

FuncWidget::~FuncWidget()
{
    delete ui;
}

void FuncWidget::loadUi()
{
    if(rtduino->getAllDeviceName("UART").contains(rtduino->getFunctionInfo()->serial2))
    {
        ui->checkBox_serial2->setChecked(true);
    }
    else
    {
        ui->checkBox_serial2->setChecked(false);
    }

    if(rtduino->getAllDeviceName("UART").contains(rtduino->getFunctionInfo()->serial2))
    {
        ui->checkBox_serial2->setChecked(true);
    }
    else
    {
        ui->checkBox_serial2->setChecked(false);
    }

    if(rtduino->getAllDeviceName("UART").contains(rtduino->getFunctionInfo()->serial3))
    {
        ui->checkBox_serial3->setChecked(true);
    }
    else
    {
        ui->checkBox_serial3->setChecked(false);
    }

    if(rtduino->getNowMcuSeries().timers.contains(rtduino->getFunctionInfo()->timer))
    {
        ui->checkBox_timer->setChecked(true);
    }
    else
    {
        ui->checkBox_timer->setChecked(false);
    }

    if(!rtduino->getFunctionInfo()->pwmtospi.spi_name.isEmpty())
    {
        ui->checkBox_spi->setChecked(true);
    }
    else
    {
        ui->checkBox_spi->setChecked(false);
    }

    //==========================================================================//
    ui->comboBox_spi->clear();
    ui->comboBox_spi->addItems(rtduino->getAllDeviceName("SPI"));
    ui->comboBox_spi->setCurrentText(rtduino->getFunctionInfo()->spi);

    ui->comboBox_spi_ss->clear();
    ui->comboBox_spi_ss->addItems(rtduino->getArduinoDigital());
    ui->comboBox_spi_ss->setCurrentText(rtduino->getFunctionInfo()->spiss);

    ui->comboBox_led->clear();
    ui->comboBox_led->addItems(rtduino->getArduinoDigital());
    ui->comboBox_led->setCurrentText(rtduino->getFunctionInfo()->ledpin);

    ui->comboBox_i2c->clear();
    ui->comboBox_i2c->addItems(rtduino->getAllDeviceName("I2C"));
    ui->comboBox_i2c->setCurrentText(rtduino->getFunctionInfo()->i2c);

    ui->comboBox_code->setCurrentText(rtduino->getFunctionInfo()->codedemo);
    ui->lineEdit_author->setText(rtduino->getProjectInfo()->pinfo_author);
    ui->lineEdit_freq->setText(rtduino->getFunctionInfo()->frequency);
}

void FuncWidget::refreshUi()
{

}

void FuncWidget::quitUi()
{
    rtduino->getFunctionInfo()->serial2 = ui->comboBox_serial2->currentText();
    rtduino->getFunctionInfo()->serial3 = ui->comboBox_serial3->currentText();
    QStringList serials = rtduino->getAllDeviceName("UART");
    serials.removeOne(rtduino->getFunctionInfo()->serial2);
    serials.removeOne(rtduino->getFunctionInfo()->serial3);
    if (!serials.isEmpty())
    {
        rtduino->getFunctionInfo()->serial1 = serials.at(0);
    }
    else
    {
        rtduino->getFunctionInfo()->serial1.clear();
    }
    rtduino->getFunctionInfo()->timer = ui->comboBox_timer->currentText();
    rtduino->getFunctionInfo()->spi = ui->comboBox_spi->currentText();
    rtduino->getFunctionInfo()->spiss = ui->comboBox_spi_ss->currentText();
    rtduino->getFunctionInfo()->ledpin = ui->comboBox_led->currentText();
    rtduino->getFunctionInfo()->i2c = ui->comboBox_i2c->currentText();
    rtduino->getFunctionInfo()->codedemo = ui->comboBox_code->currentText();
    rtduino->getFunctionInfo()->frequency = ui->lineEdit_freq->text();
    rtduino->getFunctionInfo()->pwmtospi.spi_name = ui->comboBox_spiname->currentText();
    rtduino->getFunctionInfo()->pwmtospi.sck_port = ui->comboBox_sckport->currentText();
    rtduino->getFunctionInfo()->pwmtospi.sck_pin = ui->comboBox_sckpin->currentText();
    rtduino->getFunctionInfo()->pwmtospi.miso_port = ui->comboBox_misoport->currentText();
    rtduino->getFunctionInfo()->pwmtospi.miso_pin = ui->comboBox_misopin->currentText();
    rtduino->getFunctionInfo()->pwmtospi.mosi_port = ui->comboBox_mosiport->currentText();
    rtduino->getFunctionInfo()->pwmtospi.mosi_pin = ui->comboBox_mosipin->currentText();
    rtduino->getProjectInfo()->pinfo_author = ui->lineEdit_author->text();

    ui->checkBox_serial2->setChecked(false);
    ui->checkBox_serial3->setChecked(false);
    ui->checkBox_timer->setChecked(false);
    ui->checkBox_spi->setChecked(false);
}

void FuncWidget::on_checkBox_serial2_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->comboBox_serial2->clear();
    }
    else
    {
        ui->comboBox_serial2->clear();
        ui->comboBox_serial2->addItems(rtduino->getAllDeviceName("UART"));
        ui->comboBox_serial2->setCurrentText(rtduino->getFunctionInfo()->serial2);
    }
}


void FuncWidget::on_checkBox_serial3_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->comboBox_serial3->clear();
    }
    else
    {
        ui->comboBox_serial3->clear();
        ui->comboBox_serial3->addItems(rtduino->getAllDeviceName("UART"));
        ui->comboBox_serial3->setCurrentText(rtduino->getFunctionInfo()->serial3);
    }
}


void FuncWidget::on_checkBox_timer_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->comboBox_timer->clear();
    }
    else
    {
        ui->comboBox_timer->clear();
        ui->comboBox_timer->addItems(rtduino->getNowMcuSeries().timers);
        ui->comboBox_timer->setCurrentText(rtduino->getFunctionInfo()->timer);
    }
}

void FuncWidget::on_checkBox_spi_stateChanged(int arg1)
{
    ui->comboBox_sckport->clear();
    ui->comboBox_sckpin->clear();
    ui->comboBox_misoport->clear();
    ui->comboBox_misopin->clear();
    ui->comboBox_mosiport->clear();
    ui->comboBox_mosipin->clear();
    ui->comboBox_spiname->clear();
    if(arg1 != 0)
    {
        qDebug() << rtduino->getIoFunctionConfig("SPI").names;
        ui->comboBox_spiname->clear();
        ui->comboBox_spiname->addItems(rtduino->getIoFunctionConfig("SPI").names);
        ui->comboBox_spiname->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.spi_name);

        ui->comboBox_sckport->clear();
        ui->comboBox_sckport->addItems(rtduino->getNowMcuSeries().iopininfo.ports);
        ui->comboBox_sckport->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.sck_port);
        ui->comboBox_sckpin->clear();
        ui->comboBox_sckpin->addItems(rtduino->getNowMcuSeries().iopininfo.pins);
        ui->comboBox_sckpin->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.sck_pin);

        ui->comboBox_mosiport->clear();
        ui->comboBox_mosiport->addItems(rtduino->getNowMcuSeries().iopininfo.ports);
        ui->comboBox_mosiport->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.mosi_port);
        ui->comboBox_mosipin->clear();
        ui->comboBox_mosipin->addItems(rtduino->getNowMcuSeries().iopininfo.pins);
        ui->comboBox_mosipin->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.mosi_pin);

        ui->comboBox_misoport->clear();
        ui->comboBox_misoport->addItems(rtduino->getNowMcuSeries().iopininfo.ports);
        ui->comboBox_misoport->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.miso_port);
        ui->comboBox_misopin->clear();
        ui->comboBox_misopin->addItems(rtduino->getNowMcuSeries().iopininfo.pins);
        ui->comboBox_misopin->setCurrentText(rtduino->getFunctionInfo()->pwmtospi.mosi_pin);
    }
}
