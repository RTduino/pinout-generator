#include "pinmap.h"
#include <QDebug>

/*
 *填充pin的arduino_pin和io_name的值
 */
void Pinmaplist::fill_pinmap_arduino_and_name()
{
    int Anum = 0,Dnum = 0,DACnum = 0;
    if(Allpinlist.isEmpty())
        return;
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_function == "ADC")
        {
            pinitem->arduino_pin = "A"+QString::number(Anum++);
        }
        else
        {
            if(pinitem->io_function == "DAC")
            {
                pinitem->arduino_pin = "DAC" +QString::number(DACnum++);
            }
            else
            {
                pinitem->arduino_pin = "D" + QString::number(Dnum++);
            }
        }
    }
}

/*
 *填充pin的注释信息
 */
void Pinmaplist::fill_pinmap_notes(Pinmap *pinmap)
{
    if(pinmap->io_name.mid(0,3) == "pwm")
        pinmap->io_notes = "/* PWM */";
    else if(pinmap->io_name.mid(0,3) == "i2c")
        pinmap->io_notes = "/* I2C(Wire) */";
    else if(pinmap->io_name.mid(0,3) == "usb")
        pinmap->io_notes = "/* SerialUSB */";
    else if(pinmap->io_name.mid(0,4) == "uart")
        pinmap->io_notes = "/* Serial */";
    else if(pinmap->io_name.mid(0,3) == "adc")
    {
        if(pinmap->rtthread_pin == "RT_NULL"&&pinmap->io_channel == "16")
            pinmap->io_notes = "/* ADC, On-Chip: internal temperature sensor, ADC_CHANNEL_TEMPSENSOR */";
        else if(pinmap->rtthread_pin == "RT_NULL"&&pinmap->io_channel == "17")
            pinmap->io_notes = "/* ADC, On-Chip: internal reference voltage, ADC_CHANNEL_VREFINT */";
        else
             pinmap->io_notes = "/* ADC */";
    }
    else if(pinmap->io_name.mid(0,3) == "dac")
        pinmap->io_notes = "/* DAC */";
    else if(pinmap->io_name.mid(0,3) == "spi")
        pinmap->io_notes = "/* SPI */";
    else
        pinmap->io_function = "";
}

/*
 *调整编号顺序，达成Dx->Ax->DACx
 */
void Pinmaplist::adjust_order_to_keep_arduino()
{
    /* 把Ax移动到Dx的后面 */
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_function == "ADC")
        {
            Allpinlist.move(Allpinlist.indexOf(pinitem),Allpinlist.size()-1);
        }
    }
    /* 把DACx移动到Ax的后面 */
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_function == "DAC")
        {
            Allpinlist.move(Allpinlist.indexOf(pinitem),Allpinlist.size()-1);
        }
    }
}

/*
 *删除一个元素，通过arduino_pin属性
 */
void Pinmaplist::remove_item_by_arduinopin(QString &ardpin)
{
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->arduino_pin == ardpin)
        {
            delete pinitem;
            Allpinlist.removeOne(pinitem);
            fill_pinmap_arduino_and_name();
            break;
        }
    }
}

/*
 *修改信息
 */
QString Pinmaplist::change_item_by_arduinopin(QString &ardpin,QStringList &pinmap)
{
    QString curardpin;
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->arduino_pin == ardpin)
        {
            pinitem->io_function = pinmap.at(0);
            pinitem->rtthread_pin = pinmap.at(1);
            pinitem->io_name = pinmap.at(2);
            pinitem->io_channel = pinmap.at(3);
            /* 因为修改了pin的信息，如果从GPIO换成了ADC功能，那么arduino_pin这些也要相应改变！ */
            fill_pinmap_arduino_and_name();
            fill_pinmap_notes(pinitem);
            /* 返回更改后的arduino_pin */
            curardpin = pinitem->arduino_pin;
            break;
        }
    }
    return curardpin;
}

QString Pinmaplist::add_pinmap_info_to_list(QStringList &pinmap)
{
    auto pmap = new Pinmap;
    pmap->io_function = pinmap.at(0);
    pmap->rtthread_pin = pinmap.at(1);
    pmap->io_name = pinmap.at(2);
    pmap->io_channel = pinmap.at(3);
    Allpinlist.push_back(pmap);
    fill_pinmap_arduino_and_name();
    fill_pinmap_notes(pmap);
    return pmap->arduino_pin;
}

QString Pinmaplist::insert_item_back_arduinopin(QString &ardpin, QStringList &pinmap)
{
    QString curardpin;
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->arduino_pin == ardpin)
        {
            auto pmap = new Pinmap;
            pmap->io_function = pinmap.at(0);
            pmap->rtthread_pin = pinmap.at(1);
            pmap->io_name = pinmap.at(2);
            pmap->io_channel = pinmap.at(3);
            Allpinlist.insert(Allpinlist.indexOf(pinitem)+1, pmap);
            fill_pinmap_arduino_and_name();
            fill_pinmap_notes(pmap);
            curardpin = pmap->arduino_pin;
            break;
        }
    }
    return curardpin;
}

int Pinmaplist::find_item_index_by_arduinopin(QString &ardpin)
{
    int index = -1;
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->arduino_pin == ardpin)
        {
            index = Allpinlist.indexOf(pinitem);
            break;
        }
    }
    return index;
}
