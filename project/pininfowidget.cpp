#include "pininfowidget.h"
#include "ui_pininfowidget.h"
#include <QDebug>
#include "mcuinfo.h"

PinInfoWidget::PinInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinInfoWidget)
{
    ui->setupUi(this);

    // 初始化UI的外设下拉框
    for (int i = 0; i < McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.pinFuncList.size(); i++) {
        ui->comboBox_devfunc->addItem(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.pinFuncList[i].devFunc);
    }
}

PinInfoWidget::~PinInfoWidget()
{
    delete ui;
}

void PinInfoWidget::initInfoUi(QString type)
{
    qDebug() << type;
    typeInfo = type;

    ui->comboBox_devfunc->clear();

    for (int i = 0; i < McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.pinFuncList.size(); i++) {
        ui->comboBox_devfunc->addItem(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.pinFuncList[i].devFunc);
    }

    if(type == "add")
    {
        ui->label_title->setText("添加引脚");
        uiRefresh("GPIO"); //默认加载的是GPIO页面
        ui->pushButton_save->setText("添加");
    }
    else if(type == "insert")
    {
        ui->label_title->setText("插入引脚");
        uiRefresh("GPIO"); //默认加载的是GPIO页面
        ui->pushButton_save->setText("插入");
    }
    else if(type == "change")
    {
        ui->label_title->setText("修改引脚");
        if(McuInfo::getInstance()->nowMcuSeries->selectPin == nullptr)
        {
            return;
        }

        show();
        uiChangeRefresh();
        ui->pushButton_save->setText("修改");

        return;
    }
    else
    {
        return;
    }
    show();
}

void PinInfoWidget::uiRefresh(QString devFunc)
{
    struct pinInfoConfig uiCfg = McuInfo::getInstance()->nowMcuSeries->pinInfoCfg;

    struct pinFunc *pinfunc = McuInfo::getInstance()->nowMcuSeries->findPinFunc(devFunc);

    if(pinfunc == nullptr)
        return;

    ui->comboBox_devfunc->setCurrentText(devFunc);

    if(!pinfunc->devChnl.isEmpty())
    {
        ui->label_devchnl->setVisible(true);
        ui->comboBox_devchnl->setVisible(true);
        ui->comboBox_devchnl->clear();
        ui->comboBox_devchnl->addItems(pinfunc->devChnl);
    }
    else
    {
        ui->label_devchnl->setVisible(false);
        ui->comboBox_devchnl->setVisible(false);
        ui->comboBox_devchnl->clear();
    }

    if(!pinfunc->devName.isEmpty())
    {
        ui->label_devname->setVisible(true);
        ui->comboBox_devname->setVisible(true);
        ui->comboBox_devname->clear();
        ui->comboBox_devname->addItems(pinfunc->devName);
    }
    else
    {
        ui->label_devname->setVisible(false);
        ui->comboBox_devname->setVisible(false);
        ui->comboBox_devname->clear();
    }

    if(!pinfunc->useFunc.isEmpty())
    {
        ui->label_usefunc->setVisible(true);
        ui->comboBox_usefunc->setVisible(true);
        ui->comboBox_usefunc->clear();
        ui->comboBox_usefunc->addItems(pinfunc->useFunc);
    }
    else
    {
        ui->label_usefunc->setVisible(false);
        ui->comboBox_usefunc->setVisible(false);
        ui->comboBox_usefunc->clear();
    }

    if(uiCfg.useNumber == true) // 判断是使用引脚编号函数宏定义的方式
    {
        ui->comboBox_rttnumber->setVisible(true);
        ui->label_rttnumber->setVisible(true);

        ui->comboBox_rttnumber->clear();
        ui->comboBox_rttnumber->addItems(uiCfg.rttNumber);

        ui->comboBox_rttport->setVisible(false);
        ui->comboBox_rttpin->setVisible(false);
        ui->label_rttportpin->setVisible(false);
    }
    else
    {
        ui->comboBox_rttport->setVisible(true);
        ui->comboBox_rttpin->setVisible(true);
        ui->label_rttportpin->setVisible(true);

        ui->comboBox_rttport->clear();
        ui->comboBox_rttport->addItems(uiCfg.rttPort);

        ui->comboBox_rttpin->clear();
        ui->comboBox_rttpin->addItems(uiCfg.rttPin);

        ui->comboBox_rttnumber->setVisible(false);
        ui->label_rttnumber->setVisible(false);
    }
}

void PinInfoWidget::uiChangeRefresh()
{
    struct pinFunc *pinfunc = McuInfo::getInstance()->nowMcuSeries->findPinFunc(McuInfo::getInstance()->nowMcuSeries->selectPin->pinDevFunc);

    uiRefresh(McuInfo::getInstance()->nowMcuSeries->selectPin->pinDevFunc);
    if(ui->comboBox_usefunc->isVisible())
    {
        ui->comboBox_usefunc->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinUseFunc);
    }
    if(ui->comboBox_rttnumber->isVisible())
    {
        ui->comboBox_rttnumber->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinRtthread);
    }
    if(ui->comboBox_rttport->isVisible())
    {
        ui->comboBox_rttport->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinRttPort);
    }
    if(ui->comboBox_rttpin->isVisible())
    {
        ui->comboBox_rttpin->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinRttPin);
    }
    if(ui->comboBox_devname->isVisible())
    {
        ui->comboBox_devname->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinDevName);
    }
    if(ui->comboBox_devchnl->isVisible())
    {
        ui->comboBox_devchnl->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->pinDevChnl);
    }
    if(ui->comboBox_vol5v->isVisible())
    {
        ui->comboBox_vol5v->setCurrentText(McuInfo::getInstance()->nowMcuSeries->selectPin->vol5vAllow);
    }

    if(McuInfo::getInstance()->nowMcuSeries->selectPin->pinDevFunc == "ADC")
    {
        if(McuInfo::getInstance()->nowMcuSeries->selectPin->pinUseFunc.mid(0, 6) == "EXTVOL")
        {
            ui->comboBox_devchnl->clear();
            ui->comboBox_devchnl->addItems(pinfunc->devChnl);
            ui->comboBox_devchnl->setVisible(true);
            ui->label_devchnl->setVisible(true);

            ui->comboBox_vol5v->clear();
            ui->comboBox_vol5v->addItem("Yes");
            ui->comboBox_vol5v->addItem("No");
            ui->comboBox_vol5v->setVisible(true);
            ui->label_vol5v->setVisible(true);

            if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
            {
                ui->comboBox_rttnumber->clear();
                ui->comboBox_rttnumber->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttNumber);
                ui->comboBox_rttnumber->setVisible(true);
                ui->label_rttnumber->setVisible(true);
            }
            else
            {
                ui->comboBox_rttport->clear();
                ui->comboBox_rttpin->clear();
                ui->comboBox_rttport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
                ui->comboBox_rttpin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
                ui->label_rttportpin->setVisible(true);
                ui->comboBox_rttport->setVisible(true);
                ui->comboBox_rttpin->setVisible(true);
            }
        }
        else
        {
            ui->comboBox_devchnl->clear();
            ui->comboBox_devchnl->setVisible(false);
            ui->label_devchnl->setVisible(false);

            ui->comboBox_vol5v->clear();
            ui->comboBox_vol5v->setVisible(false);
            ui->label_vol5v->setVisible(false);

            if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
            {
                ui->comboBox_rttnumber->clear();
                ui->comboBox_rttnumber->setVisible(false);
                ui->label_rttnumber->setVisible(false);
            }
            else
            {
                ui->comboBox_rttport->clear();
                ui->comboBox_rttpin->clear();
                ui->label_rttportpin->setVisible(false);
                ui->comboBox_rttport->setVisible(false);
                ui->comboBox_rttpin->setVisible(false);
            }
        }
    }
}

void PinInfoWidget::on_comboBox_devfunc_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_vol5v->clear();
    ui->comboBox_vol5v->addItem("Yes");
    ui->comboBox_vol5v->addItem("No");
    ui->comboBox_vol5v->setVisible(true);
    ui->label_vol5v->setVisible(true);
    uiRefresh(arg1);
}


void PinInfoWidget::on_pushButton_save_clicked()
{
    struct pinInfo info;
    info.pinDevFunc = ui->comboBox_devfunc->currentText();
    info.pinUseFunc = ui->comboBox_usefunc->currentText();
    info.pinRtthread = ui->comboBox_rttnumber->currentText();
    info.pinRttPort = ui->comboBox_rttport->currentText();
    info.pinRttPin = ui->comboBox_rttpin->currentText();
    info.pinDevName = ui->comboBox_devname->currentText();
    info.pinDevChnl = ui->comboBox_devchnl->currentText();
    info.vol5vAllow = ui->comboBox_vol5v->currentText();

    if(typeInfo == "add")
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoAdd(info);
    }
    else if(typeInfo == "insert")
    {
        if(McuInfo::getInstance()->nowMcuSeries->selectPin != nullptr)
        {
            McuInfo::getInstance()->nowMcuSeries->pinInfoInsert(McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino, info);
        }
        else
        {
            McuInfo::getInstance()->nowMcuSeries->pinInfoAdd(info);
        }
    }
    else if(typeInfo == "change")
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoChange(McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino, info);
    }
    else
    {
        return;
    }

    emit refreshTreeWidget();

    if(typeInfo == "insert" || typeInfo == "change")
    {
        close();
    }
}

void PinInfoWidget::on_comboBox_usefunc_currentIndexChanged(const QString &arg1)
{
    struct pinFunc *pinfunc = McuInfo::getInstance()->nowMcuSeries->findPinFunc(ui->comboBox_devfunc->currentText());

    if(ui->comboBox_devfunc->currentText() == "ADC")
    {
        if(arg1.mid(0, 6) == "EXTVOL")
        {
            ui->comboBox_devchnl->clear();
            ui->comboBox_devchnl->addItems(pinfunc->devChnl);
            ui->comboBox_devchnl->setVisible(true);
            ui->label_devchnl->setVisible(true);

            ui->comboBox_vol5v->clear();
            ui->comboBox_vol5v->addItem("Yes");
            ui->comboBox_vol5v->addItem("No");
            ui->comboBox_vol5v->setVisible(true);
            ui->label_vol5v->setVisible(true);

            if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
            {
                ui->comboBox_rttnumber->clear();
                ui->comboBox_rttnumber->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttNumber);
                ui->comboBox_rttnumber->setVisible(true);
                ui->label_rttnumber->setVisible(true);
            }
            else
            {
                ui->comboBox_rttport->clear();
                ui->comboBox_rttpin->clear();
                ui->comboBox_rttport->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPort);
                ui->comboBox_rttpin->addItems(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.rttPin);
                ui->label_rttportpin->setVisible(true);
                ui->comboBox_rttport->setVisible(true);
                ui->comboBox_rttpin->setVisible(true);
            }
        }
        else
        {
            ui->comboBox_devchnl->clear();
            ui->comboBox_devchnl->setVisible(false);
            ui->label_devchnl->setVisible(false);

            ui->comboBox_vol5v->clear();
            ui->comboBox_vol5v->setVisible(false);
            ui->label_vol5v->setVisible(false);

            if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
            {
                ui->comboBox_rttnumber->clear();
                ui->comboBox_rttnumber->setVisible(false);
                ui->label_rttnumber->setVisible(false);
            }
            else
            {
                ui->comboBox_rttport->clear();
                ui->comboBox_rttpin->clear();
                ui->label_rttportpin->setVisible(false);
                ui->comboBox_rttport->setVisible(false);
                ui->comboBox_rttpin->setVisible(false);
            }
        }
    }
}
