#include "pinmap.h"
#include <QDebug>
#include <QByteArray>
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
        pinmap->io_notes = "/* I2C-"+ pinmap->pin_func +" (Wire) */";
    else if(pinmap->io_name.mid(0,3) == "usb")
        pinmap->io_notes = "/* SerialUSB */";
    else if(pinmap->io_name.mid(0,4) == "uart")
        pinmap->io_notes = "/* Serial-"+ pinmap->pin_func +" */";
    else if(pinmap->io_name.mid(0,3) == "adc")
    {
        pinmap->io_notes = "/* ADC */";
    }
    else if(pinmap->io_name.mid(0,3) == "dac")
        pinmap->io_notes = "/* DAC */";
    else if(pinmap->io_name.mid(0,3) == "spi")
        pinmap->io_notes = "/* SPI-"+ pinmap->pin_func +" */";
    else
        pinmap->io_notes = "";
}

void Pinmaplist::update_pinmap_notes()
{
    foreach(auto i,this->Allpinlist)
    {
        fill_pinmap_notes(i);
    }
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
            pinitem->pin_func = pinmap.at(4);
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
    pmap->pin_func = pinmap.at(4);
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
            pmap->pin_func = pinmap.at(4);
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

bool Pinmaplist::check_device_isexist(QString name,QList<Pindevice *> devlist)
{
    foreach(auto pinitem,devlist)
    {
        if(pinitem->device_name == name)
        {
            return true;
        }
    }
    return false;
}

bool Pinmaplist::check_device_ch_isexist(QString channel,QStringList channellist)
{
    foreach(auto channelitem,channellist)
    {
        if(channelitem == channel)
            return true;
    }
    return false;
}
bool sort_rules(Pindevice *p1,Pindevice *p2)
{
    return p1->device_name.mid(3,p1->device_name.size()-3).toInt() < p2->device_name.mid(3,p2->device_name.size()-3).toInt();
}
bool sort_rules_channel(QString &p1,QString &p2)
{
    return p1.toInt() < p2.toInt();
}
void printf_list(QList<Pindevice *> devlist)
{
    foreach(auto pinitem,devlist)
    {
        qDebug()<< pinitem->device_name << pinitem->device_channels;
    }
}
void Pinmaplist::update_uartpinlist()
{
    uartpinlist.clear();
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_name.mid(0,4) == "uart")
        {
            if(!check_device_isexist(pinitem->io_name,uartpinlist))
            {
                auto uartdev = new Pindevice;
                uartdev->device_name = pinitem->io_name;
                uartpinlist.append(uartdev);
            }
        }
    }
    qSort(uartpinlist.begin(),uartpinlist.end(),sort_rules);
    printf_list(uartpinlist);
}

void Pinmaplist::update_pwmpinlist()
{
    pwmpinlist.clear();
    foreach(auto deviceitem,Allpinlist)
    {
        if(deviceitem->io_name.mid(0,3) == "pwm")
        {
            if(!check_device_isexist(deviceitem->io_name,pwmpinlist))
            {
                auto pwmdev = new Pindevice;
                pwmdev->device_name = deviceitem->io_name;
                pwmpinlist.append(pwmdev);
            }
        }
    }

    foreach(auto pwmitem,pwmpinlist)
    {
        foreach(auto deviceitem,Allpinlist)
        {
            if(pwmitem->device_name == deviceitem ->io_name)
            {
                if(!check_device_ch_isexist(deviceitem ->io_channel,pwmitem->device_channels))
                {
                    deviceitem ->io_channel = QString::number(qAbs(deviceitem ->io_channel.toInt()));
                    pwmitem->device_channels.append(deviceitem ->io_channel);
                }
            }
        }
        qSort(pwmitem->device_channels.begin(),pwmitem->device_channels.end(),sort_rules_channel);
    }
    qSort(pwmpinlist.begin(),pwmpinlist.end(),sort_rules);
    printf_list(pwmpinlist);
}

void Pinmaplist::update_adcpinlist()
{
    adcpinlist.clear();
    foreach(auto deviceitem,Allpinlist)
    {
        if(deviceitem->io_name.mid(0,3) == "adc")
        {
            if(!check_device_isexist(deviceitem->io_name,adcpinlist))
            {
                auto pwmdev = new Pindevice;
                pwmdev->device_name = deviceitem->io_name;
                adcpinlist.append(pwmdev);
            }
        }
    }

    foreach(auto adctem,adcpinlist)
    {
        foreach(auto deviceitem,Allpinlist)
        {
            if(adctem->device_name == deviceitem ->io_name)
            {
                if(!check_device_ch_isexist(deviceitem ->io_channel,adctem->device_channels))
                {
                    adctem->device_channels.append(deviceitem ->io_channel);
                }
            }
        }
        qSort(adctem->device_channels.begin(),adctem->device_channels.end(),sort_rules_channel);
    }
    qSort(adcpinlist.begin(),adcpinlist.end(),sort_rules);
    printf_list(adcpinlist);
}

void Pinmaplist::update_dacpinlist()
{
    dacpinlist.clear();
    foreach(auto deviceitem,Allpinlist)
    {
        if(deviceitem->io_name.mid(0,3) == "dac")
        {
            if(!check_device_isexist(deviceitem->io_name,dacpinlist))
            {
                auto pwmdev = new Pindevice;
                pwmdev->device_name = deviceitem->io_name;
                dacpinlist.append(pwmdev);
            }
        }
    }

    foreach(auto dacitem,dacpinlist)
    {
        foreach(auto deviceitem,Allpinlist)
        {
            if(dacitem->device_name == deviceitem ->io_name)
            {
                if(!check_device_ch_isexist(deviceitem ->io_channel,dacitem->device_channels))
                {
                    dacitem->device_channels.append(deviceitem ->io_channel);
                }
            }
        }
        qSort(dacitem->device_channels.begin(),dacitem->device_channels.end(),sort_rules_channel);
    }
    qSort(dacpinlist.begin(),dacpinlist.end(),sort_rules);
    printf_list(dacpinlist);
}

void Pinmaplist::update_spipinlist()
{
    spipinlist.clear();
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_name.mid(0,3) == "spi")
        {
            if(!check_device_isexist(pinitem->io_name,spipinlist))
            {
                auto uartdev = new Pindevice;
                uartdev->device_name = pinitem->io_name;
                spipinlist.append(uartdev);
            }
        }
    }
    qSort(spipinlist.begin(),spipinlist.end(),sort_rules);
    printf_list(spipinlist);
}

void Pinmaplist::update_i2cpinlist()
{
    i2cpinlist.clear();
    foreach(auto pinitem,Allpinlist)
    {
        if(pinitem->io_name.mid(0,3) == "i2c")
        {
            if(!check_device_isexist(pinitem->io_name,i2cpinlist))
            {
                auto uartdev = new Pindevice;
                uartdev->device_name = pinitem->io_name;
                i2cpinlist.append(uartdev);
            }
        }
    }
    qSort(i2cpinlist.begin(),i2cpinlist.end(),sort_rules);
    printf_list(i2cpinlist);
}

#define PIN_NUM(port, no) (((((port) & 0xFu) << 4) | ((no) & 0xFu)))
int Pinmaplist::count_num_by_rttpin(QString rtt)
{
    int hw_port_num, hw_pin_num = 0;
    QByteArray rttarr = rtt.toUtf8();
    char * rttStr = rttarr.data();
    hw_port_num =  rttStr[0] - 'A';
    hw_pin_num = rtt.mid(1,rtt.size()-1).toInt();
    return PIN_NUM(hw_port_num, hw_pin_num);
}
