#include "widget.h"
#include "ui_widget.h"

void Widget::all_ui_reset()
{
    ui->ledbox->clear();
    ui->spissbox->clear();
    ui->s2box->clear();
    ui->s3box->clear();
    ui->spidevbox->clear();
    ui->i2cdevbox->clear();
    ui->timedit->clear();
    ui->autoredit->clear();
    ui->fcpuedit->clear();
    ui->mainbox->clear();
}
void Widget::all_ui_component_refresh()
{
    table_all_items_refresh();
    serials_add_items();
    spi_add_items();
    i2c_add_items();
    code_add_items();
    led_ss_pin_add_items();
}

void Widget::table_add_item(Pinmap &pin)
{
    QTreeWidgetItem* item=new QTreeWidgetItem();
    item->setText(0,pin.arduino_pin);
    item->setText(1,pin.rtthread_pin);
    item->setText(2,pin.io_function);
    if(!pin.pin_func.isEmpty())
        item->setText(3,pin.io_name+" - "+pin.pin_func);
    else
        item->setText(3,pin.io_name);
    item->setText(4,pin.io_channel);

    ui->treeWidget->addTopLevelItem(item);
}

void Widget::table_all_items_refresh()
{
    ui->treeWidget->clear();
    foreach(auto pinitem,pinmaplist.Allpinlist)
    {
        table_add_item(*pinitem);
    }
}

void Widget::serials_add_items()
{
    QString curs2(ui->s2box->currentText());
    QString curs3(ui->s3box->currentText());
    QStringList uartlist("NULL");
    ui->s2box->clear();
    ui->s3box->clear();
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,4) == "uart")
        {
            uartlist.removeOne(i->io_name);
            uartlist.append(i->io_name);
        }
    }
    ui->s2box->addItems(uartlist);
    ui->s3box->addItems(uartlist);
    ui->s2box->setCurrentText(curs2);
    ui->s3box->setCurrentText(curs3);
}


void Widget::spi_add_items()
{
    QString curspi(ui->spidevbox->currentText());
    QStringList spilist("NULL");
    ui->spidevbox->clear();
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "spi")
        {
            spilist.removeOne(i->io_name);
            spilist.append(i->io_name);
        }
    }
    ui->spidevbox->addItems(spilist);
    ui->spidevbox->setCurrentText(curspi);
}

void Widget::code_add_items()
{
    QString curcode(ui->mainbox->currentText());
    QStringList codelist("Hello Arduino");
    ui->mainbox->clear();
    if(!(ui->ledbox->currentText() == "NULL"))
    {
        codelist.append("LED blinking");
    }
    ui->mainbox->addItems(codelist);
    ui->mainbox->setCurrentText(curcode);
}

void Widget::i2c_add_items()
{
    QString curi2c(ui->i2cdevbox->currentText());
    QStringList i2clist("NULL");
    ui->i2cdevbox->clear();
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "i2c")
        {
            i2clist.removeOne(i->io_name);
            i2clist.append(i->io_name);
        }
    }
    ui->i2cdevbox->addItems(i2clist);
    ui->i2cdevbox->setCurrentText(curi2c);
}


void Widget::led_ss_pin_add_items()
{
    QString led(ui->ledbox->currentText());
    QString spi(ui->spissbox->currentText());
    QStringList ledlist("NULL");
    ui->ledbox->clear();
    ui->spissbox->clear();
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_function != "ADC")
        {
            ledlist.removeOne(i->arduino_pin);
            ledlist.append(i->arduino_pin);
        }
    }
    ui->ledbox->addItems(ledlist);
    ui->spissbox->addItems(ledlist);
    ui->ledbox->setCurrentText(led);
    ui->spissbox->setCurrentText(spi);
}


