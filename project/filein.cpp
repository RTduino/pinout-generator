#include "widget.h"
#include "ui_widget.h"

void Widget::prase_pin_item_from_string(QString &pinline)
{
    if(pinline.indexOf(",") < 0)
    {
        return;
    }
    auto pmap = new Pinmap;

    /* 正则表达式 */
    QRegularExpression arduinoreg("([ADC]{1,3}\\d+)");
    QRegularExpression rttpinreg("([ADC]{1,3}\\d+),[ ]?.*?(GET_PIN.*?\\)|\\d+)?[,} ]");
    QRegularExpression namereg("\"(.*)\"");
    QRegularExpression channelreg("[ ,](-?\\d+)[ }]");

    pmap->arduino_pin =  arduinoreg.match(pinline).captured(1);
    pmap->io_channel = channelreg.match(pinline).captured(1);
    pmap->io_name = namereg.match(pinline).captured(0).mid(1,namereg.match(pinline).captured(0).length()-2);
    if(rttpinreg.match(pinline).captured(2) == "")
        pmap->rtthread_pin = "RT_NULL";
    else
        pmap->rtthread_pin = rttpinreg.match(pinline).captured(2);

    pin_set_io_function(pmap);
    pinmaplist.fill_pinmap_notes(pmap);
    pinmaplist.Allpinlist.push_back(pmap);
}

void Widget::pin_set_io_function(Pinmap *pinmap)
{
    if(pinmap->io_name.mid(0,3) == "pwm")
        pinmap->io_function = "PWM";
    else if(pinmap->io_name.mid(0,3) == "i2c")
        pinmap->io_function = "I2C";
    else if(pinmap->io_name.mid(0,3) == "usb")
        pinmap->io_function = "USB";
    else if(pinmap->io_name.mid(0,4) == "uart")
        pinmap->io_function = "UART";
    else if(pinmap->io_name.mid(0,3) == "adc")
        pinmap->io_function = "ADC";
    else if(pinmap->io_name.mid(0,3) == "dac")
        pinmap->io_function = "DAC";
    else if(pinmap->io_name.mid(0,3) == "spi")
        pinmap->io_function = "SPI";
    else
        pinmap->io_function = "GPIO";
}

void Widget::load_data_from_dir()
{
    QDir arduinodir;

    /* clear pinmaplist */
    qDeleteAll(pinmaplist.Allpinlist.begin(),pinmaplist.Allpinlist.end());
    pinmaplist.Allpinlist.clear();

    if(!arduinodir.exists(rttBspdirpath+"/applications/arduino_pinout"))
    {
        return;
    }

    QFile file(rttBspdirpath+"/applications/arduino_pinout/pins_arduino.c");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }
    QTextStream in(&file);
    while(!in.atEnd())
    {
        QString line_date = in.readLine();
        if(line_date.mid(0,1) == "{")
        {
            while(1)
            {
                QString line_date = in.readLine();
                if(line_date.mid(0,2) == "};"||in.atEnd())
                    break;
                /* 解析文件 */
                prase_pin_item_from_string(line_date);
            }
        }
    }
    file.close();
}
