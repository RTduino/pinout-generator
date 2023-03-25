#include "pininfoui.h"
#include "ui_pininfoui.h"

PininfoUI::PininfoUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PininfoUI)
{
    ui->setupUi(this);
    show_name_or_channel(false,false);
    show_pin_function(false);
}

PininfoUI::~PininfoUI()
{
    delete ui;
}

void PininfoUI::show_name_or_channel(bool nameshow, bool channelshow)
{
    if(nameshow == true)
    {
        ui->devnameedit->setVisible(true);
        ui->devnamelabel->setVisible(true);
    }
    else
    {
        ui->devnameedit->setVisible(false);
        ui->devnamelabel->setVisible(false);
    }
    if(channelshow == true)
    {
        ui->channellabel->setVisible(true);
        ui->channeledit->setVisible(true);
    }
    else
    {
        ui->channellabel->setVisible(false);
        ui->channeledit->setVisible(false);
    }

    ui->channeledit->clear();
    ui->devnameedit->clear();
}

void PininfoUI::show_pin_function(bool pinshow)
{
    if(pinshow == true)
    {
        ui->pinfucbox->setVisible(true);
        ui->pinfuclabel->setVisible(true);
    }
    else
    {
        ui->pinfucbox->setVisible(false);
        ui->pinfuclabel->setVisible(false);
    }
    ui->pinfucbox->clear();
    pinfunclist.clear();
}

void PininfoUI::show_change_ui_info(QStringList &list)
{
    ui->funcbox->setCurrentText(list.at(2));
    ui->pinfucbox->setCurrentText(list.at(5));
    ui->rttpinedit->setText(list.at(1));
    ui->devnameedit->setText(list.at(3));
    ui->channeledit->setText(list.at(4));
    ui->namelabel->setText("修改引脚");
    curpinstate = "change";
}

void PininfoUI::show_insert_ui_info()
{
    ui->namelabel->setText("插入引脚");
    curpinstate = "insert";
}

void PininfoUI::show_add_ui_info()
{
    ui->namelabel->setText("添加引脚");
    curpinstate = "add";
}

void PininfoUI::on_addbtn_clicked()
{
    QStringList list;
    if(ui->rttpinedit->text().isEmpty())
    {
        ui->rttpinedit->setText(ui->rttpinedit->placeholderText().mid(0,12));
    }
    if(ui->devnameedit->text().isEmpty())
    {
        ui->devnameedit->setText(ui->devnameedit->placeholderText());
    }
    if(ui->channeledit->text().isEmpty())
    {
        ui->channeledit->setText(ui->channeledit->placeholderText());
    }
    list << ui->funcbox->currentText();
    list << ui->rttpinedit->text();
    list << ui->devnameedit->text();
    list << ui->channeledit->text();
    if(ui->funcbox->currentText() == "ADC")
        list << ui->pinfucbox->currentText().mid(0,6);
    else
        list << ui->pinfucbox->currentText();
    list << curpinstate;

    emit send_pininfo_data(list);
    close();
}

void PininfoUI::on_cancebtn_clicked()
{
    close();
}

void PininfoUI::on_funcbox_currentIndexChanged(const QString &arg1)
{
    ui->pinfuclabel->setText("引脚功能");
    if(arg1 == "GPIO")
    {
        show_name_or_channel(false,false);
        show_pin_function(false);
        ui->devnameedit->setPlaceholderText("");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "UART") {
        show_name_or_channel(true,false);
        show_pin_function(true);
        pinfunclist << "TX" << "RX";
        ui->pinfucbox->addItems(pinfunclist);
        ui->devnameedit->setPlaceholderText("uart1");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "PWM") {
        show_name_or_channel(true,true);
        show_pin_function(false);
        ui->devnameedit->setPlaceholderText("pwm1");
        ui->channeledit->setPlaceholderText("2");
    }
    else if (arg1 == "ADC") {
        show_name_or_channel(true,true);
        show_pin_function(true);
        ui->pinfuclabel->setText("通道功能");
        pinfunclist << "EXTVOL(外部电压)" << "INTVOL(内部电压)" << "INTTEP(内部温度)";
        ui->pinfucbox->addItems(pinfunclist);
        ui->devnameedit->setPlaceholderText("adc1");
        ui->channeledit->setPlaceholderText("3");
    }
    else if (arg1 == "DAC") {
        show_name_or_channel(true,true);
        show_pin_function(false);
        ui->devnameedit->setPlaceholderText("dac1");
        ui->channeledit->setPlaceholderText("1");
    }
    else if (arg1 == "SPI") {
        show_name_or_channel(true,false);
        show_pin_function(true);
        pinfunclist << "SCK" << "MISO" << "MOSI";
        ui->pinfucbox->addItems(pinfunclist);
        ui->devnameedit->setPlaceholderText("spi2");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "I2C") {
        show_name_or_channel(true,false);
        show_pin_function(true);
        pinfunclist << "SCL" << "SDA";
        ui->pinfucbox->addItems(pinfunclist);
        ui->devnameedit->setPlaceholderText("i2c1");
        ui->channeledit->setPlaceholderText("");
    }
    else /*USB*/
    {
        show_name_or_channel(true,false);
        show_pin_function(false);
        show_pin_function(true);
        pinfunclist << "DP" << "DM";
        ui->pinfucbox->addItems(pinfunclist);
        ui->devnameedit->setPlaceholderText("usb");
        ui->channeledit->setPlaceholderText("");
    }
}

/*回车事件，绑定到确定按钮*/
void PininfoUI::on_rttpinedit_returnPressed()
{
    on_addbtn_clicked();
}

void PininfoUI::on_devnameedit_returnPressed()
{
    on_addbtn_clicked();
}

void PininfoUI::on_channeledit_returnPressed()
{
    on_addbtn_clicked();
}

void PininfoUI::on_pinfucbox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "INTVOL" || arg1 == "INTTEP")
    {
        ui->rttpinedit->setText("RT_NULL");
    }
//    else
//        ui->rttpinedit->clear();
}

