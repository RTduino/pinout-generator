#include "widget.h"
#include "ui_widget.h"
#include "fileinfo.h"

void Widget::get_date_info()
{
    QDateTime local(QDateTime::currentDateTime());
    QString localTime = local.toString("yyyy-MM-dd");
    dateinfo->year = localTime.mid(0,4);
    dateinfo->month = localTime.mid(5,2);
    dateinfo->day = localTime.mid(8,2);
    if(ui->autoredit->text().isEmpty())
        dateinfo->autor = "     default      first version";
    else
        dateinfo->autor = "     "+ui->autoredit->text()+QString("%1").arg("first version", 13+13-ui->autoredit->text().size(), QLatin1Char(' '));
}

void Widget::load_data_to_dir()
{
    if(ui->fcpuedit->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请填写时钟频率！");
        return;
    }
    QDir arduinodir;
    if(!arduinodir.exists(rttBspdirpath+"/applications"))
    {
        if(!arduinodir.mkdir(rttBspdirpath+"/applications"))
        {
            QString errinfo ="创建 "+rttBspdirpath+"/applications 文件夹失败！";
            QMessageBox::critical(this,"错误",errinfo);
        }
    }

    if(!arduinodir.exists(rttBspdirpath+"/applications/arduino_pinout"))
    {
        if(!arduinodir.mkdir(rttBspdirpath+"/applications/arduino_pinout"))
        {
            QString errinfo ="创建 "+rttBspdirpath+"/applications/arduino_pinout 文件夹失败！";
            QMessageBox::critical(this,"错误",errinfo);
        }
    }
    get_date_info();
    write_data_to_cfile();
    write_data_to_hfile();
    write_data_to_scons();
    write_data_to_kconfig();
    QMessageBox::StandardButton result = QMessageBox::question( this,"成功","代码生成成功，是否打开资源所在目录？");

    if(result == QMessageBox::No)
        return;

    QDesktopServices::openUrl(QUrl(rttBspdirpath+"/applications/arduino_pinout",QUrl::TolerantMode));
}

void Widget::write_data_to_cfile()
{
    QFile cfile(rttBspdirpath+"/applications/arduino_pinout/pins_arduino.c");
    if(!cfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&cfile);
    /* License */
    out << FILE_RTT_LICENSE_PART;
    /* Include */
    out << "#include <Arduino.h>\n";
    out << "#include <board.h>\n";
    out << "#include \"pins_arduino.h\"\n\n";

    out << "/*\n";
    out << " * {Arduino Pin, RT-Thread Pin [, Device Name, Channel]}\n";
    out << " * [] means optional\n";
    out << " * Digital pins must NOT give the device name and channel.\n";
    out << " * Analog pins MUST give the device name and channel(ADC, PWM or DAC).\n";
    out << " * Arduino Pin must keep in sequence.\n";
    out << " */\n";

    out << "const pin_map_t pin_map_table[]=\n";
    out << "{\n";
    foreach(auto pmap , pinmaplist.Allpinlist)
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
        linestr += "},";
        if(!pmap->io_notes.isEmpty())
        {
            linestr +=QString("%1").arg(' ', 40-linestr.size(), QLatin1Char(' '));
            linestr +=pmap->io_notes;
        }
        linestr += "\n";
        out << linestr;
    }
    out << "};\n";
}

void Widget::write_data_to_hfile()
{
    QFile cfile(rttBspdirpath+"/applications/arduino_pinout/pins_arduino.h");
    if(!cfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&cfile);
    /* License */
    out << FILE_RTT_LICENSE_PART;

    out << "#ifndef Pins_Arduino_h\n";
    out << "#define Pins_Arduino_h\n\n";

    out << "/* pins alias. Must keep in sequence */\n";
    for(int i = 0;i<pinmaplist.Allpinlist.size();i++)
    {
        QString linestr("#define "+ pinmaplist.Allpinlist.at(i)->arduino_pin+\
                        QString("%1").arg(' ',10-pinmaplist.Allpinlist.at(i)->arduino_pin.size(), QLatin1Char(' '))\
                        +"("+QString::number(i)+")\n");
        out << linestr;
    }
    out << "\n";

    out << "#define F_CPU          "+ui->fcpuedit->text()+"000000L\n\n";
    if(!(ui->ledbox->currentText() == "NULL"))
    {
        out << "#define LED_BUITIN     "+ui->ledbox->currentText()+"\n\n";
    }

    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "i2c")
        {
            out << "#define RTDUINO_DEFAULT_IIC_BUS_NAME      \""+i->io_name+"\"\n\n";
            break;
        }
    }
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "spi")
        {
            if(!(ui->spissbox->currentText() == "NULL"))
            {
                out << "#define SS      "+ui->spissbox->currentText()+"\n";
            }
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
    out << "#endif /* Pins_Arduino_h */\n";
}

void Widget::write_data_to_kconfig()
{
    QFile kconfigfile(rttBspdirpath+"/applications/arduino_pinout/Kconfig");
    if(!kconfigfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }
    QTextStream out(&kconfigfile);
    QStringList varlist;
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
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "adc")
        {
            varlist.removeOne("    select BSP_USING_ADC\n");
            varlist.append("    select BSP_USING_ADC\n");
            varlist.removeOne("    select BSP_USING_ADC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.append("    select BSP_USING_ADC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
        }
    };
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "dac")
        {
            varlist.removeOne("    select BSP_USING_DAC\n");
            varlist.append("    select BSP_USING_DAC\n");
            varlist.removeOne("    select BSP_USING_DAC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.append("    select BSP_USING_DAC"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
        }
    };
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "pwm")
        {
            int abschannel =qAbs(i->io_channel.toInt());
            varlist.removeOne("    select BSP_USING_PWM\n");
            varlist.append("    select BSP_USING_PWM\n");
            varlist.removeOne("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.append("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.removeOne("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"_CH"+QString::number(abschannel)+"\n");
            varlist.append("    select BSP_USING_PWM"+i->io_name.mid(3,i->io_name.size()-3)+"_CH"+QString::number(abschannel)+"\n");
        }
    };
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "i2c")
        {
            varlist.removeOne("    select BSP_USING_I2C\n");
            varlist.append("    select BSP_USING_I2C\n");
            varlist.removeOne("    select BSP_USING_I2C"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.append("    select BSP_USING_I2C"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            break;
        }
    };
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name.mid(0,3) == "spi")
        {
            varlist.removeOne("    select BSP_USING_SPI\n");
            varlist.append("    select BSP_USING_SPI\n");
            varlist.removeOne("    select BSP_USING_SPI"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            varlist.append("    select BSP_USING_SPI"+i->io_name.mid(3,i->io_name.size()-3)+"\n");
            break;
        }
    };
    foreach(auto i,varlist)
    {
        out << i;
    }
    out << "    imply RTDUINO_USING_SERVO\n";
    out << "    imply RTDUINO_USING_WIRE\n";
    out << "    imply RTDUINO_USING_SPI\n";
    out << "    default n";
}

void Widget::write_data_to_scons()
{
    QFile sconsfile(rttBspdirpath+"/applications/arduino_pinout/Sconscript");
    if(!sconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&sconsfile);

    out << FILESCONS_CODE;
}
