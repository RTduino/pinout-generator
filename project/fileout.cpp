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
    write_data_to_maincpp();
    write_data_to_mainscons();
    QMessageBox::StandardButton result = QMessageBox::question( this,"成功","代码生成成功，是否打开资源所在目录？");

    if(result == QMessageBox::No)
        return;

    QDesktopServices::openUrl(QUrl(rttBspdirpath+"/applications",QUrl::TolerantMode));
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
        if(pmap->arduino_pin == ui->ledbox->currentText())
            pmap->io_notes = "/* LED_BUILTIN */";

        if(pmap->arduino_pin == ui->spissbox->currentText())
            pmap->io_notes = "/* SPI-SS */";

        if(pmap->io_name == ui->s2box->currentText())
            pmap->io_notes = "/* Serial2-"+ pmap->pin_func +" */";

        if(pmap->io_name == ui->s3box->currentText())
            pmap->io_notes = "/* Serial3-"+ pmap->pin_func +" */";

        if(pmap->pin_func == "INTVOL")
            pmap->io_notes = "/* ADC, On-Chip: internal reference voltage, ADC_CHANNEL_VREFINT */";

        if(pmap->pin_func == "INTTEP")
            pmap->io_notes = "/* ADC, On-Chip: internal temperature sensor, ADC_CHANNEL_TEMPSENSOR */";

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

    out << "#define F_CPU          "+ui->fcpuedit->text()+"000000L  /* CPU:"+ui->fcpuedit->text()+"MHz */\n\n";
    if(!(ui->ledbox->currentText() == "NULL"))
    {
        out << "#define LED_BUILTIN     "+ui->ledbox->currentText()+"  /* Default Built-in LED */\n\n";
    }
    if(!(ui->i2cdevbox->currentText() == "NULL"))
    {
        QString pinscl = "xx",pinsda = "xx";
        foreach(auto i,pinmaplist.Allpinlist)
        {
            if(i->io_name == ui->i2cdevbox->currentText() && i->pin_func == "SCL")
                pinscl = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            if(i->io_name == ui->i2cdevbox->currentText() && i->pin_func == "SDA")
                pinsda = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
        }
        out << "/* "+ui->i2cdevbox->currentText()+" : P"+pinsda+"-SDA P"+pinscl+"-SCL */\n";
        out << "#define RTDUINO_DEFAULT_IIC_BUS_NAME    \""+ui->i2cdevbox->currentText()+"\"\n\n";
    }
    if(!(ui->spidevbox->currentText() == "NULL"))
    {
        QString pinsck = "xx",pinmiso = "xx",pinmosi = "xx";
        foreach(auto i,pinmaplist.Allpinlist)
        {
            if(i->io_name == ui->spidevbox->currentText() && i->pin_func == "SCK")
                pinsck = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            if(i->io_name == ui->spidevbox->currentText() && i->pin_func == "MISO")
                pinmiso = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            if(i->io_name == ui->spidevbox->currentText() && i->pin_func == "MOSI")
                pinmosi = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
        }
        out << "/* "+ui->spidevbox->currentText()+" : P"+pinsck+"-SCK  P"+pinmiso+"-MISO  P"+pinmosi+"-MOSI */\n";
        if(!(ui->spissbox->currentText() == "NULL"))
        {
            out << "#define SS      "+ui->spissbox->currentText()+"  /* Chip select pin of default spi */\n";
        }
        out << "#define RTDUINO_DEFAULT_SPI_BUS_NAME      \""+ui->spidevbox->currentText()+"\"\n\n";
    }
    if(!(ui->s2box->currentText() == "NULL"))
    {
        QString pintx = "xx",pinrx = "xx";
        foreach(auto i,pinmaplist.Allpinlist)
        {
            if(i->io_name == ui->s2box->currentText() && i->pin_func == "TX")
                pintx = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            if(i->io_name == ui->s2box->currentText() && i->pin_func == "RX")
                pinrx = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
        }
        out << "/* Serial2 : P"+pintx+"-TX  P"+pinrx+"-RX */\n";
        out << "#define RTDUINO_SERIAL2_DEVICE_NAME      \""+ui->s2box->currentText()+"\"\n\n";
    }
    if(!(ui->s3box->currentText() == "NULL"))
    {
        QString pintx = "xx",pinrx = "xx";
        foreach(auto i,pinmaplist.Allpinlist)
        {
            if(i->io_name == ui->s3box->currentText() && i->pin_func == "TX")
                pintx = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            if(i->io_name == ui->s3box->currentText() && i->pin_func == "RX")
                pinrx = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
        }
        out << "/* Serial3 : P"+pintx+"-TX  P"+pinrx+"-RX */\n";
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
    QFile kconfigfile(rttBspdirpath+"/applications/Kconfig(Please copy)");
    if(!kconfigfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }
    QTextStream out(&kconfigfile);
    QStringList varlist;

    out << "menu \"Onboard Peripheral Drivers\"\n\n";
    out << "    config BSP_USING_ARDUINO\n";
    out << "        bool \"Compatible with Arduino Ecosystem (RTduino)\"\n";
    out << "        select PKG_USING_RTDUINO\n";
    out << "        select BSP_USING_STLINK_TO_USART\n";
    if(!(ui->s2box->currentText() == "NULL"))
        out << "        select BSP_USING_UART"+ui->s2box->currentText().mid(4,ui->s2box->currentText().size()-4)+"\n";
    if(!(ui->s3box->currentText() == "NULL"))
        out << "        select BSP_USING_UART"+ui->s3box->currentText().mid(4,ui->s3box->currentText().size()-4)+"\n";

    out << "        select BSP_USING_GPIO\n";

    if(!ui->timedit->text().isEmpty())
    {
        out << "        select BSP_USING_TIM\n";
        out << "        select BSP_USING_TIM"+ui->timedit->text().mid(5,ui->timedit->text().size()-5)+"\n";
    }

    if(!pinmaplist.adcpinlist.isEmpty())
        out << "        select BSP_USING_ADC\n";
    foreach(auto i,pinmaplist.adcpinlist)
    {
        out << "        select BSP_USING_ADC"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
    };

    if(!pinmaplist.dacpinlist.isEmpty())
        out << "        select BSP_USING_DAC\n";
    foreach(auto i,pinmaplist.dacpinlist)
    {
        out << "        select BSP_USING_DAC"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
    };

    if(!pinmaplist.pwmpinlist.isEmpty())
        out << "        select BSP_USING_PWM\n";
    foreach(auto i,pinmaplist.pwmpinlist)
    {
        out << "        select BSP_USING_PWM"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
        foreach(auto c,i->device_channels)
        {
            out << "        select BSP_USING_PWM"+i->device_name.mid(3,i->device_name.size()-3)+"_CH"+c+"\n";
        }
    };

    if(!pinmaplist.i2cpinlist.isEmpty())
        out << "        select BSP_USING_I2C\n";
    foreach(auto i,pinmaplist.i2cpinlist)
    {
        out << "        select BSP_USING_I2C"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
    };

    if(!pinmaplist.spipinlist.isEmpty())
        out << "        select BSP_USING_SPI\n";
    foreach(auto i,pinmaplist.spipinlist)
    {
        out << "        select BSP_USING_SPI"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
    };

    if(!pinmaplist.pwmpinlist.isEmpty())
        out << "        imply RTDUINO_USING_SERVO\n";
    if(ui->i2cdevbox->currentText() != "NULL")
        out << "        imply RTDUINO_USING_WIRE\n";
    if(ui->spidevbox->currentText() != "NULL")
        out << "        imply RTDUINO_USING_SPI\n";
    out << "        default n\n\n";
    out << "endmenu\n\n";
//---------------------------------------------------------
    if(!(ui->s2box->currentText() == "NULL") || !(ui->s3box->currentText() == "NULL"))
    {
        out << "menu \"On-chip Peripheral Drivers\"\n\n";
        out << "    menuconfig BSP_USING_UART\n";
        out << "        bool \"Enable UART\"\n";
        out << "        default y\n";
        out << "        select RT_USING_SERIAL\n";
        out << "        if BSP_USING_UART\n";
    }

    if(!(ui->s2box->currentText() == "NULL"))
    {
        out << "            config BSP_USING_UART"+ui->s2box->currentText().mid(4,ui->s2box->currentText().size()-4)+"\n";
        out << "                bool \"Enable UART"+ui->s2box->currentText().mid(4,ui->s2box->currentText().size()-4)+"\"\n";
        out << "                default n\n";
    }
    if(!(ui->s3box->currentText() == "NULL"))
    {
        out << "            config BSP_USING_UART"+ui->s3box->currentText().mid(4,ui->s3box->currentText().size()-4)+"\n";
        out << "                bool \"Enable UART"+ui->s3box->currentText().mid(4,ui->s3box->currentText().size()-4)+"\"\n";
        out << "                default n\n";
    }
    if(!(ui->s2box->currentText() == "NULL") || !(ui->s3box->currentText() == "NULL"))
        out << "        endif\n\n";

    if(!ui->timedit->text().isEmpty())
    {
        out << "    menuconfig BSP_USING_TIM\n";
        out << "        bool \"Enable timer\"\n";
        out << "        default n\n";
        out << "        select RT_USING_HWTIMER\n";
        out << "        if BSP_USING_TIM\n";
        out << "            config BSP_USING_TIM"+ui->timedit->text().mid(5,ui->timedit->text().size()-5)+"\n";
        out << "            bool \"Enable TIM"+ui->timedit->text().mid(5,ui->timedit->text().size()-5)+"\"\n";
        out << "                default n\n";
        out << "        endif\n\n";
    }
    if(!pinmaplist.adcpinlist.isEmpty())
    {
        out << "    menuconfig BSP_USING_ADC\n";
        out << "        bool \"Enable ADC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_ADC\n";
        out << "        if BSP_USING_ADC\n";
        foreach(auto i,pinmaplist.adcpinlist)
        {
            out << "            config BSP_USING_ADC"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
            out << "                bool \"Enable ADC"+i->device_name.mid(3,i->device_name.size()-3)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }


    if(!pinmaplist.dacpinlist.isEmpty())
    {
        out << "    menuconfig BSP_USING_DAC\n";
        out << "        bool \"Enable DAC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_DAC\n";
        out << "        if BSP_USING_DAC\n";
        foreach(auto i,pinmaplist.dacpinlist)
        {
            out << "            config BSP_USING_DAC"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
            out << "                bool \"Enable DAC"+i->device_name.mid(3,i->device_name.size()-3)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }


    if(!pinmaplist.pwmpinlist.isEmpty())
    {
        out << "    menuconfig BSP_USING_PWM\n";
        out << "        bool \"Enable PWM\"\n";
        out << "        default n\n";
        out << "        select RT_USING_PWM\n";
        out << "        if BSP_USING_PWM\n";
        foreach(auto i,pinmaplist.pwmpinlist)
        {
            out << "        menuconfig BSP_USING_PWM"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
            out << "            bool \"Enable timer"+i->device_name.mid(3,i->device_name.size()-3)+" output PWM\"\n";
            out << "            default n\n";
            out << "            if BSP_USING_PWM"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
            foreach(auto c,i->device_channels)
            {
                out << "                config BSP_USING_PWM"+i->device_name.mid(3,i->device_name.size()-3)+"_CH"+c+"\n";
                out << "                    bool \"Enable PWM"+i->device_name.mid(3,i->device_name.size()-3)+" channel"+c+"\"\n";
                out << "                    default n\n";

            }
            out << "            endif\n\n";
        };
        out << "        endif\n\n";
    }


    if(!pinmaplist.spipinlist.isEmpty())
    {
        out << "    menuconfig BSP_USING_SPI\n";
        out << "        bool \"Enable SPI BUS\"\n";
        out << "        default n\n";
        out << "        select RT_USING_SPI\n";
        out << "        if BSP_USING_SPI\n";
        foreach(auto i,pinmaplist.spipinlist)
        {
            out << "            config BSP_USING_SPI"+i->device_name.mid(3,i->device_name.size()-3)+"\n";
            out << "                bool \"Enable SPI"+i->device_name.mid(3,i->device_name.size()-3)+" BUS\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }
//    qDebug()<<pinmaplist.count_num_by_rttpin("B1");
    int pinscl = -1,pinsda = -1;
    QString pinsclstr = "xx",pinsdastr = "xx";
    foreach(auto i,pinmaplist.Allpinlist)
    {
        if(i->io_name == ui->i2cdevbox->currentText() && i->pin_func == "SCL")
        {
            pinsclstr = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            pinscl = pinmaplist.count_num_by_rttpin(pinsclstr);
        }

        if(i->io_name == ui->i2cdevbox->currentText() && i->pin_func == "SDA")
        {
            pinsdastr = i->rtthread_pin.mid(8,1)+i->rtthread_pin.mid(10,i->rtthread_pin.size()-11);
            pinsda = pinmaplist.count_num_by_rttpin(pinsdastr);
        }
    }

    if(!pinmaplist.i2cpinlist.isEmpty())
    {
        out << "    menuconfig BSP_USING_I2C1\n";
        out << "        bool \"Enable I2C1 BUS (software simulation)\"\n";
        out << "        default n\n";
        out << "        select RT_USING_I2C\n";
        out << "        select RT_USING_I2C_BITOPS\n";
        out << "        select RT_USING_PIN\n";
        out << "        if BSP_USING_I2C1\n";
        foreach(auto i,pinmaplist.i2cpinlist)
        {
            out << "            config BSP_I2C"+i->device_name.mid(3,i->device_name.size()-3)+"_SCL_PIN\n";
            out << "                int \"i2c"+i->device_name.mid(3,i->device_name.size()-3)+" scl pin number (P"+pinsclstr+")\"\n";
            out << "                default "+QString::number(pinscl)+"\n";
            out << "            config BSP_I2C"+i->device_name.mid(3,i->device_name.size()-3)+"_SDA_PIN\n";
            out << "                int \"i2c"+i->device_name.mid(3,i->device_name.size()-3)+" sda pin number (P"+pinsdastr+")\"\n";
            out << "                default "+QString::number(pinsda)+"\n";
        };
        out << "        endif\n\n";
    }
    out << "endmenu\n";
}

void Widget::write_data_to_scons()
{
    QFile sconsfile(rttBspdirpath+"/applications/arduino_pinout/SConscript");
    if(!sconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&sconsfile);

    out << FILESCONS_CODE;
}

void Widget::write_data_to_maincpp()
{
    QFile cppfile(rttBspdirpath+"/applications/arduino_main.cpp");
    if(!cppfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&cppfile);
    /* License */
    out << FILE_RTT_LICENSE_PART;

    out << "#include <Arduino.h>\n\n";
    out << "void setup(void)\n";
    out << "{\n";
    out << "    /* put your setup code here, to run once: */\n";
    if(ui->mainbox->currentIndex() == 1)
    {
        out << "    pinMode(LED_BUILTIN, OUTPUT);\n";
    }
    else
    {
        out << "    Serial.begin();\n";
    }

    out << "}\n\n";
    out << "void loop(void)\n";
    out << "{\n";
    out << "    /* put your main code here, to run repeatedly: */\n";
    if(ui->mainbox->currentIndex() == 1)
    {
        out << "    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));\n";
        out << "    delay(100);\n";
    }
    else
    {
        out << "    Serial.println(\"Hello Arduino!\");\n";
        out << "    delay(800);\n";
    }
    out << "}\n";
}

void Widget::write_data_to_mainscons()
{
    QFile mainsconsfile(rttBspdirpath+"/applications/SConscript");
    if(!mainsconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return;
    }

    QTextStream out(&mainsconsfile);

    out << "from building import *\n";
    out << "import os\n\n";
    out << "cwd     = GetCurrentDir()\n";
    out << "CPPPATH = [cwd]\n";
    out << "src     = Glob('*.c')\n\n";
    out << "if GetDepend(['PKG_USING_RTDUINO']) and not GetDepend(['RTDUINO_NO_SETUP_LOOP']):\n";
    out << "    src += ['arduino_main.cpp']\n\n";
    out << "group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH)\n\n";
    out << "list = os.listdir(cwd)\n";
    out << "for item in list:\n";
    out << "    if os.path.isfile(os.path.join(cwd, item, 'SConscript')):\n";
    out << "        group = group + SConscript(os.path.join(item, 'SConscript'))\n\n";
    out << "Return('group')\n";
}

