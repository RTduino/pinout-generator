#include "widget.h"
#include "ui_widget.h"
#include "filecontent.h"
#include <QDateTime>

/*************************************************处理文件到表格*************************************************/

//([AD]\d+),[ ]?.*?(GET_PIN.*?\)|\d+)?[,} ]
void Widget::prase_pin_item_instring(QString &pinstr)
{
    if( pinstr.indexOf(",") < 0)
    {
        return;
    }
    auto pmap = new Pinmap;

    /* 正则表达式 */
    QRegularExpression arduinoreg("([AD]\\d+)");
    QRegularExpression rttpinreg("([AD]\\d+),[ ]?.*?(GET_PIN.*?\\)|\\d+)?[,} ]");
    QRegularExpression namereg("\"(.*)\"");
    QRegularExpression channelreg("[ ,](-?\\d+)[ }]");

    pmap->arduino_pin =  arduinoreg.match(pinstr).captured(1);
    pmap->io_channel = channelreg.match(pinstr).captured(1);
    pmap->io_name = namereg.match(pinstr).captured(0).mid(1,namereg.match(pinstr).captured(0).length()-2);
    if(rttpinreg.match(pinstr).captured(2) == "")
        pmap->rtthread_pin = "RT_NULL";
    else
        pmap->rtthread_pin = rttpinreg.match(pinstr).captured(2);

    set_io_function(pmap);

    pinmaplist.push_back(pmap);
}

void Widget::set_io_function(Pinmap *pinmap)
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
    qDeleteAll(pinmaplist.begin(),pinmaplist.end());
    pinmaplist.clear();

    if(!arduinodir.exists(ARDUINO_PINOUT_DIR))
    {
        return;
    }

    QFile file(ARDUINO_PINOUT_CFILE);
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
                prase_pin_item_instring(line_date);
            }
        }
    }
    file.close();
}

/*************************************************导出表格到文件*************************************************/

void Widget::load_data_to_dir()
{
    if(ui->fcpuedit->text().isEmpty())
    {
        QMessageBox::critical(this,"Error","Please write the clock frequency of the BSP!");
        return;
    }
    QDir arduinodir;
    if(!arduinodir.exists(ARDUINO_PINOUT_DIR))
    {
        if(!arduinodir.mkdir(ARDUINO_PINOUT_DIR))
        {
            QString errinfo ="Failed to create folder : "+ARDUINO_PINOUT_DIR+". Please check if it is the BSP path!";
            QMessageBox::critical(this,"Error",errinfo);
        }
        return;
    }
    get_some_info();
    write_data_to_cfile();
    write_data_to_hfile();
    write_data_to_scons();
    write_data_to_kconfig();
}

void Widget::get_some_info()
{
    QDateTime local(QDateTime::currentDateTime());
    QString localTime = local.toString("yyyy-MM-dd");
    sinfo.year = localTime.mid(0,4);
    sinfo.month = localTime.mid(5,2);
    sinfo.day = localTime.mid(8,2);
    if(ui->autoredit->text().isEmpty())
        sinfo.autor = "     default      first version";
    else
        sinfo.autor = "     "+ui->autoredit->text()+QString("%1").arg("first version", 13+13-ui->autoredit->text().size(), QLatin1Char(' '));
    sinfo.fcpu = ui->fcpuedit->text();
}

void Widget::write_data_to_cfile()
{
    QFile cfile(ARDUINO_PINOUT_CFILE);
    if(!cfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&cfile);

    out << FILE_RTT_LICENSE_PART;
    out << FILEC_RTT_INCLUDE;
    out << FILEC_RTDUINO_NOTES;
    out << "const pin_map_t pin_map_table[]=\n";
    out << "{\n";
    foreach(auto pmap , pinmaplist)
    {
        QString linestr = "    {";
        if(!pmap->arduino_pin.isEmpty())
            linestr += pmap->arduino_pin;
        if(!pmap->rtthread_pin.isEmpty())
            linestr += ", "+pmap->rtthread_pin;
        if(!pmap->io_name.isEmpty())
            linestr += ", \""+pmap->io_name+"\"";
        if(!pmap->io_channel.isEmpty())
            linestr += ", "+pmap->io_channel;
        linestr += "},\n";
        out << linestr;
    }
    out << "};\n";
}

void Widget::write_data_to_hfile()
{
    QFile hfile(ARDUINO_PINOUT_HFILE);
    if(!hfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&hfile);

    out << FILE_RTT_LICENSE_PART;
    out << FILEH_HEADER;

    for(int i = 0;i<pinmaplist.size();i++)
    {
        QString linestr("#define "+ pinmaplist.at(i)->arduino_pin+\
                        QString("%1").arg(' ',10-pinmaplist.at(i)->arduino_pin.size(), QLatin1Char(' '))\
                        +"("+QString::number(i)+")\n");
        out << linestr;
    }
    out << "\n";
    out << "#define F_CPU          "+ui->fcpuedit->text()+"000000L\n\n";
    if(!(ui->ledbox->currentText() == "NULL"))
    {
        out << "#define LED_BUITIN     "+ui->ledbox->currentText()+"\n\n";
    }

    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "i2c")
        {
            out << "#define RTDUINO_DEFAULT_IIC_BUS_NAME      \""+i->io_name+"\"\n\n";
            break;
        }
    }
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "spi")
        {
            out << "#define RTDUINO_DEFAULT_SPI_BUS_NAME      \""+i->io_name+"\"\n\n";
            break;
        }
    }
    if(!(ui->s2box->currentText() == "NULL"))
    {
        out << "#define RTDUINO_SERIAL2_DEVICE_NAME      \""+ui->s2box->currentText()+"\"\n\n";
    }
    if(!(ui->s3box->currentText() == "NULL"))
    {
        out << "#define RTDUINO_SERIAL3_DEVICE_NAME      \""+ui->s3box->currentText()+"\"\n\n";
    }
    if(!ui->timedit->text().isEmpty())
    {
        out << "#define RTDUINO_DEFAULT_HWTIMER_DEVICE_NAME      \""+ui->timedit->text()+"\"\n\n";
    }

    out << FILEH_END;

}

void Widget::write_data_to_kconfig()
{
    QFile kconfigfile(ARDUINO_PINOUT_KCONFIG);
    if(!kconfigfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&kconfigfile);

    out << "config BSP_USING_ARDUINO\n";
    out << "    bool \"Support Arduino\"\n";
    out << "    select PKG_USING_RTDUINO\n";
    out << "    select BSP_USING_STLINK_TO_USART\n";
    if(!(ui->s2box->currentText() == "NULL"))
        out << "    select BSP_USING_UART"+ui->s2box->currentText().mid(4,ui->s2box->currentText().size()-4)+"\n";
    if(!(ui->s3box->currentText() == "NULL"))
        out << "    select BSP_USING_UART"+ui->s3box->currentText().mid(4,ui->s3box->currentText().size()-4)+"\n";

    out << "    select BSP_USING_GPIO\n";

    if(!ui->timedit->text().isEmpty())
    {
        out << "    select BSP_USING_TIM\n";
        out << "    select BSP_USING_TIM"+ui->timedit->text().mid(5,ui->timedit->text().size()-5)+"\n";
    }
    /* add adc */
    QStringList adclist;
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "adc")
        {
            adclist.removeOne("    select BSP_USING_ADC\n");
            adclist.append("    select BSP_USING_ADC\n");
            adclist.removeOne("    select BSP_USING_ADC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            adclist.append("    select BSP_USING_ADC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
        }
    };
    foreach(auto i,adclist)
    {
        out << i;
    }

    /* add pwm */
    QStringList pwmlist,pwmchlist;
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "pwm")
        {
            int abschannel =qAbs(i->io_channel.toInt());
            pwmchlist.removeOne("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"_CH"+QString::number(abschannel)+"\n");
            pwmchlist.append("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"_CH"+QString::number(abschannel)+"\n");
            pwmlist.removeOne("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            pwmlist.append("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            pwmlist.removeOne("    select BSP_USING_PWM\n");
            pwmlist.append("    select BSP_USING_PWM\n");
        }
    };
    foreach(auto i,pwmchlist)
    {
        out << i;
    }
    foreach(auto i,pwmlist)
    {
        out << i;
    }

    /* add i2c */
    QStringList i2clist;
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "i2c")
        {
            i2clist.removeOne("    select BSP_USING_I2C\n");
            i2clist.append("    select BSP_USING_I2C\n");
            i2clist.removeOne("    select BSP_USING_I2C"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            i2clist.append("    select BSP_USING_I2C"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
        }
    };
    foreach(auto i,i2clist)
    {
        out << i;
    }
    /* add spi */
    QStringList spilist;
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,3) == "spi")
        {
            spilist.removeOne("    select BSP_USING_SPI\n");
            spilist.append("    select BSP_USING_SPI\n");
            spilist.removeOne("    select BSP_USING_SPI"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            spilist.append("    select BSP_USING_SPI"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
        }
    };
    foreach(auto i,spilist)
    {
        out << i;
    }
    out << "    imply RTDUINO_USING_SERVO\n";
    out << "    imply RTDUINO_USING_WIRE\n";
    out << "    imply RTDUINO_USING_SPI\n";
    out << "    default n\n";
}

void Widget::write_data_to_scons()
{
    QFile sconsfile(ARDUINO_PINOUT_SCONS);
    if(!sconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&sconsfile);

    out << FILESCONS_CODE;
}

