#include "funcwidget.h"
#include "ui_funcwidget.h"
#include "QDebug"

FuncWidget::FuncWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FuncWidget)
{
    ui->setupUi(this);
    ui->comboBox_serial2->setVisible(false);
    ui->comboBox_serial3->setVisible(false);
    ui->comboBox_timer->setVisible(false);
}

FuncWidget::~FuncWidget()
{
    delete ui;
}

void FuncWidget::loadUi()
{
    // 0. 生成必要数据
    QStringList pinArduino;
    foreach (auto pinItem, McuInfo::getInstance()->nowMcuSeries->pinInfoList) {
        if(pinItem.pinDevFunc == "GPIO")
        {
            pinArduino << pinItem.pinArduino;
        }
    }

    QStringList codeDemo;
    codeDemo << "Hello Arduino" << "LED Blinking";

    // 1. 清空所有组件
    ui->comboBox_serial2->clear();
    ui->comboBox_serial3->clear();
    ui->comboBox_timer->clear();
    ui->comboBox_spi->clear();
    ui->comboBox_spi_ss->clear();
    ui->comboBox_led->clear();
    ui->comboBox_i2c->clear();
    ui->comboBox_code->clear();
    // 1.1 pwmtospi
    ui->lineEdit_spiname->clear();
    ui->comboBox_sckport->clear();
    ui->comboBox_sckpin->clear();
    ui->comboBox_misoport->clear();
    ui->comboBox_misopin->clear();
    ui->comboBox_mosiport->clear();
    ui->comboBox_mosipin->clear();

    ui->comboBox_serial2->setVisible(false);
    ui->comboBox_serial3->setVisible(false);
    ui->comboBox_timer->setVisible(false);
    ui->checkBox_serial2->setChecked(false);
    ui->checkBox_serial3->setChecked(false);
    ui->checkBox_timer->setChecked(false);
    ui->checkBox_spi->setChecked(false);

    // 2. 添加Items并且设置默认值
    if(McuInfo::getInstance()->nowMcuSeries->uartAllDev.contains(McuInfo::getInstance()->nowMcuSeries->serial2Dev))
    {
        ui->checkBox_serial2->setChecked(true);
        ui->comboBox_serial2->setVisible(true);
        ui->comboBox_serial2->addItems(McuInfo::getInstance()->nowMcuSeries->uartAllDev);
        ui->comboBox_serial2->setCurrentText(McuInfo::getInstance()->nowMcuSeries->serial2Dev);
    }
    if(McuInfo::getInstance()->nowMcuSeries->uartAllDev.contains(McuInfo::getInstance()->nowMcuSeries->serial3Dev))
    {
        ui->checkBox_serial3->setChecked(true);
        ui->comboBox_serial3->setVisible(true);
        ui->comboBox_serial3->addItems(McuInfo::getInstance()->nowMcuSeries->uartAllDev);
        ui->comboBox_serial3->setCurrentText(McuInfo::getInstance()->nowMcuSeries->serial3Dev);
    }
    if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.timer.contains(McuInfo::getInstance()->nowMcuSeries->timerDev))
    {
        ui->checkBox_timer->setChecked(true);
        ui->comboBox_timer->setVisible(true);
        ui->comboBox_timer->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.timer);
        ui->comboBox_timer->setCurrentText(McuInfo::getInstance()->nowMcuSeries->timerDev);
    }
    ui->comboBox_spi->addItems(McuInfo::getInstance()->nowMcuSeries->spiAllDev);
    ui->comboBox_spi->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiDev);
    ui->comboBox_i2c->addItems(McuInfo::getInstance()->nowMcuSeries->i2cAllDev);
    ui->comboBox_i2c->setCurrentText(McuInfo::getInstance()->nowMcuSeries->i2cDev);
    ui->comboBox_spi_ss->addItems(pinArduino);
    ui->comboBox_spi_ss->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spissPin);
    ui->comboBox_led->addItems(pinArduino);
    ui->comboBox_led->setCurrentText(McuInfo::getInstance()->nowMcuSeries->ledPin);
    ui->comboBox_code->addItems(codeDemo);
    ui->comboBox_code->setCurrentText(McuInfo::getInstance()->nowMcuSeries->codeDemo);
    if(!McuInfo::getInstance()->nowMcuSeries->authorInfo.isEmpty())
    {
        ui->lineEdit_author->setText(McuInfo::getInstance()->nowMcuSeries->authorInfo);
    }
    if(!McuInfo::getInstance()->nowMcuSeries->frequency.isEmpty())
    {
        ui->lineEdit_freq->setText(McuInfo::getInstance()->nowMcuSeries->frequency);
    }
    qDebug() << McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber;
    if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
    {
        ui->checkBox_useNumber->setChecked(true);
    }
    else
    {
        ui->checkBox_useNumber->setChecked(false);
    }

    if(McuInfo::getInstance()->nowMcuSeries->pwmTospi == false)
    {
        ui->groupBox_spi->setVisible(false);
    }
    else
    {
        if(McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty() ||
                    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty() ||
                    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty() ||
                    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty() ||
                    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty() ||
                    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port.isEmpty())
        {
            ui->checkBox_spi->setChecked(false);
            ui->lineEdit_spiname->setVisible(false);
            ui->comboBox_sckport->setVisible(false);
            ui->comboBox_misoport->setVisible(false);
            ui->comboBox_mosiport->setVisible(false);
            ui->comboBox_sckpin->setVisible(false);
            ui->comboBox_misopin->setVisible(false);
            ui->comboBox_mosipin->setVisible(false);
        }
        else
        {
            ui->checkBox_spi->setChecked(true);
            ui->lineEdit_spiname->setVisible(true);
            ui->comboBox_sckport->setVisible(true);
            ui->comboBox_misoport->setVisible(true);
            ui->comboBox_mosiport->setVisible(true);
            ui->comboBox_sckpin->setVisible(true);
            ui->comboBox_misopin->setVisible(true);
            ui->comboBox_mosipin->setVisible(true);

            ui->lineEdit_spiname->setText(McuInfo::getInstance()->nowMcuSeries->spiPins.spi_name);
            ui->comboBox_sckport->clear();
            ui->comboBox_sckport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
            ui->comboBox_sckport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Port);
            ui->comboBox_sckpin->clear();
            ui->comboBox_sckpin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
            ui->comboBox_sckpin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Pin);

            ui->comboBox_mosiport->clear();
            ui->comboBox_mosiport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
            ui->comboBox_mosiport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port);
            ui->comboBox_mosipin->clear();
            ui->comboBox_mosipin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
            ui->comboBox_mosipin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Pin);

            ui->comboBox_misoport->clear();
            ui->comboBox_misoport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
            ui->comboBox_misoport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Port);
            ui->comboBox_misopin->clear();
            ui->comboBox_misopin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
            ui->comboBox_misopin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Pin);
        }
    }
}

void FuncWidget::refreshUi()
{
    qDebug() << this->objectName() << "refresh";
}

void FuncWidget::quitUi()
{
    qDebug() << this->objectName() << "quit";
    if(ui->checkBox_serial2->isChecked())
    {
        McuInfo::getInstance()->nowMcuSeries->serial2Dev = ui->comboBox_serial2->currentText();
    }
    else
    {
        McuInfo::getInstance()->nowMcuSeries->serial2Dev = nullptr;
    }
    if(ui->checkBox_serial3->isChecked())
    {
        McuInfo::getInstance()->nowMcuSeries->serial3Dev = ui->comboBox_serial3->currentText();
    }
    else
    {
        McuInfo::getInstance()->nowMcuSeries->serial3Dev = nullptr;
    }
    if(ui->checkBox_timer->isChecked())
    {
        McuInfo::getInstance()->nowMcuSeries->timerDev = ui->comboBox_timer->currentText();
    }
    else
    {
        McuInfo::getInstance()->nowMcuSeries->timerDev = nullptr;
    }

    McuInfo::getInstance()->nowMcuSeries->spiDev = ui->comboBox_spi->currentText();
    McuInfo::getInstance()->nowMcuSeries->spissPin = ui->comboBox_spi_ss->currentText();
    McuInfo::getInstance()->nowMcuSeries->ledPin = ui->comboBox_led->currentText();
    McuInfo::getInstance()->nowMcuSeries->i2cDev = ui->comboBox_i2c->currentText();
    McuInfo::getInstance()->nowMcuSeries->codeDemo = ui->comboBox_code->currentText();

    McuInfo::getInstance()->nowMcuSeries->authorInfo = ui->lineEdit_author->text();
    McuInfo::getInstance()->nowMcuSeries->frequency = ui->lineEdit_freq->text();

    McuInfo::getInstance()->nowMcuSeries->spiPins.spi_name = ui->lineEdit_spiname->text();
    McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Port = ui->comboBox_sckport->currentText();
    McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Pin = ui->comboBox_sckpin->currentText();
    McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Port = ui->comboBox_misoport->currentText();
    McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Pin = ui->comboBox_misopin->currentText();
    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port = ui->comboBox_mosiport->currentText();
    McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Pin = ui->comboBox_mosipin->currentText();
}

void FuncWidget::on_checkBox_serial2_stateChanged(int arg1)
{
    qDebug() << arg1;
    if(arg1 == 0)
    {
        ui->comboBox_serial2->clear();
        ui->comboBox_serial2->setVisible(false);
    }
    else
    {
        ui->comboBox_serial2->setVisible(true);
        ui->comboBox_serial2->addItems(McuInfo::getInstance()->nowMcuSeries->uartAllDev);
        ui->comboBox_serial2->setCurrentText(McuInfo::getInstance()->nowMcuSeries->serial2Dev);
    }
}


void FuncWidget::on_checkBox_serial3_stateChanged(int arg1)
{
    qDebug() << arg1;
    if(arg1 == 0)
    {
        ui->comboBox_serial3->clear();
        ui->comboBox_serial3->setVisible(false);
    }
    else
    {
        ui->comboBox_serial3->setVisible(true);
        ui->comboBox_serial3->addItems(McuInfo::getInstance()->nowMcuSeries->uartAllDev);
        ui->comboBox_serial3->setCurrentText(McuInfo::getInstance()->nowMcuSeries->serial3Dev);
    }
}


void FuncWidget::on_checkBox_timer_stateChanged(int arg1)
{
    qDebug() << arg1;
    if(arg1 == 0)
    {
        ui->comboBox_timer->clear();
        ui->comboBox_timer->setVisible(false);
    }
    else
    {
        ui->comboBox_timer->setVisible(true);
        ui->comboBox_timer->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.timer);
        ui->comboBox_timer->setCurrentText(McuInfo::getInstance()->nowMcuSeries->timerDev);
    }
}


void FuncWidget::on_checkBox_useNumber_stateChanged(int arg1)
{
    qDebug() << McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber;
    if(arg1 == 0)
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber = false;
    }
    else
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber = true;
    }

    qDebug() << McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber;
}


void FuncWidget::on_checkBox_spi_stateChanged(int arg1)
{
    ui->comboBox_sckport->clear();
    ui->comboBox_sckpin->clear();
    ui->comboBox_misoport->clear();
    ui->comboBox_misopin->clear();
    ui->comboBox_mosiport->clear();
    ui->comboBox_mosipin->clear();
    ui->lineEdit_spiname->clear();
    if(arg1 == 0)
    {
        ui->checkBox_spi->setChecked(false);
        ui->comboBox_sckport->setVisible(false);
        ui->comboBox_misoport->setVisible(false);
        ui->comboBox_mosiport->setVisible(false);
        ui->comboBox_sckpin->setVisible(false);
        ui->comboBox_misopin->setVisible(false);
        ui->comboBox_mosipin->setVisible(false);
        ui->lineEdit_spiname->setVisible(false);
    }
    else
    {
        ui->checkBox_spi->setChecked(true);
        ui->comboBox_sckport->setVisible(true);
        ui->comboBox_misoport->setVisible(true);
        ui->comboBox_mosiport->setVisible(true);
        ui->comboBox_sckpin->setVisible(true);
        ui->comboBox_misopin->setVisible(true);
        ui->comboBox_mosipin->setVisible(true);
        ui->lineEdit_spiname->setVisible(true);

        ui->lineEdit_spiname->setText(McuInfo::getInstance()->nowMcuSeries->spiPins.spi_name);
        ui->comboBox_sckport->clear();
        ui->comboBox_sckport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
        ui->comboBox_sckport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Port);
        ui->comboBox_sckpin->clear();
        ui->comboBox_sckpin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
        ui->comboBox_sckpin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.sck.Pin);

        ui->comboBox_mosiport->clear();
        ui->comboBox_mosiport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
        ui->comboBox_mosiport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Port);
        ui->comboBox_mosipin->clear();
        ui->comboBox_mosipin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
        ui->comboBox_mosipin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.mosi.Pin);

        ui->comboBox_misoport->clear();
        ui->comboBox_misoport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
        ui->comboBox_misoport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Port);
        ui->comboBox_misopin->clear();
        ui->comboBox_misopin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
        ui->comboBox_misopin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->spiPins.miso.Pin);
    }
}

