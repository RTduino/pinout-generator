#include "add.h"
#include "ui_add.h"
#include <QDebug>

add::add(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::add)
{
    ui->setupUi(this);
    display_add_edit(false,false);
    setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Pinout Configuration");
}

add::~add()
{
    delete ui;
}

void add::on_addbtn_clicked()
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

    emit send_add_data(list);
    close();
}


void add::on_cancebtn_clicked()
{
    close();
    ui->namelabel->setText("添加引脚");
}

void add::on_funcbox_currentIndexChanged(const QString &arg1)
{
    if(arg1 == "GPIO")
    {
        display_add_edit(false,false);
        ui->devnameedit->setPlaceholderText("");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "UART") {
        display_add_edit(true,false);
        ui->devnameedit->setPlaceholderText("uart1");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "PWM") {
        display_add_edit(true,true);
        ui->devnameedit->setPlaceholderText("pwm1");
        ui->channeledit->setPlaceholderText("2");
    }
    else if (arg1 == "ADC") {
        display_add_edit(true,true);
        ui->devnameedit->setPlaceholderText("adc1");
        ui->channeledit->setPlaceholderText("3");
    }
    else if (arg1 == "DAC") {
        display_add_edit(true,true);
        ui->devnameedit->setPlaceholderText("dac1");
        ui->channeledit->setPlaceholderText("1");
    }
    else if (arg1 == "SPI") {
        display_add_edit(true,false);
        ui->devnameedit->setPlaceholderText("spi2");
        ui->channeledit->setPlaceholderText("");
    }
    else if (arg1 == "I2C") {
        ui->devnameedit->setPlaceholderText("i2c1");
        ui->channeledit->setPlaceholderText("");
        display_add_edit(true,false);
    }
    else /*USB*/
    {
        ui->devnameedit->setPlaceholderText("usb");
        ui->channeledit->setPlaceholderText("");
        display_add_edit(true,false);
    }
}

void add::display_add_edit(bool name, bool channel)
{
    if(name == true)
    {
        ui->devnameedit->setVisible(true);
        ui->devnamelabel->setVisible(true);
    }
    else
    {
        ui->devnameedit->setVisible(false);
        ui->devnamelabel->setVisible(false);
    }
    if(channel == true)
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

void add::set_widget_info(QStringList &list)
{
    ui->funcbox->setCurrentText(list.at(2));
    ui->rttpinedit->setText(list.at(1));
    ui->devnameedit->setText(list.at(3));
    ui->channeledit->setText(list.at(4));
    ui->namelabel->setText("修改引脚");
}
