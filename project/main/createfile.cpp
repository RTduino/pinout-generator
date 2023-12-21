#include "createfile.h"

CreateFile::CreateFile()
{
    rtduino = RTduinoConfig::getInstance();
}

bool CreateFile::creatPinoutDir()
{
    QDir arduinodir;

    if(!arduinodir.exists(rtduino->getProjectInfo()->project_path + "/arduino_pinout"))
    {
        if(!arduinodir.mkdir(rtduino->getProjectInfo()->project_path + "/arduino_pinout"))
        {
            return false;
        }
    }

    updateRTThreadNoteInfo();

    return true;
}

bool CreateFile::creatImage()
{
    if (rtduino->getProjectInfo()->pinmap_image.isEmpty())
        return false;
    QFileInfo fileInfo(rtduino->getProjectInfo()->pinmap_image);
    QString fileChange = rtduino->getProjectInfo()->project_path + "/arduino_pinout/" +
            rtduino->getProjectInfo()->board_name.toLower()
            + "-pinout." + fileInfo.suffix();
    qDebug() << fileChange;
    QFile::copy(rtduino->getProjectInfo()->pinmap_image, fileChange);
    rtduino->getProjectInfo()->pinmap_image = fileChange;

    return true;
}

bool CreateFile::createSConscript()
{
    QFile mainsconsfile(rtduino->getProjectInfo()->project_path + "/SConscript");
    QFile pinoutsconsfile(rtduino->getProjectInfo()->project_path + "/arduino_pinout/SConscript");
    if(!mainsconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream mainout(&mainsconsfile);

    // 内容
    mainout << "from building import *\n";
    mainout << "import os\n\n";
    mainout << "cwd     = GetCurrentDir()\n";
    mainout << "src     = Glob('*.c')\n";
    mainout << "CPPPATH = [cwd]\n\n";
    mainout << "if GetDepend(['PKG_USING_RTDUINO']) and not GetDepend(['RTDUINO_NO_SETUP_LOOP']):\n";
    mainout << "    src += ['arduino_main.cpp']\n\n";
    mainout << "group = DefineGroup('Applications', src, depend = [''], CPPPATH = CPPPATH)\n\n";
    mainout << "list = os.listdir(cwd)\n";
    mainout << "for item in list:\n";
    mainout << "    if os.path.isfile(os.path.join(cwd, item, 'SConscript')):\n";
    mainout << "        group = group + SConscript(os.path.join(item, 'SConscript'))\n\n";
    mainout << "Return('group')\n";

    mainsconsfile.close();

    if(!pinoutsconsfile.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream pinout(&pinoutsconsfile);

    // 内容
    pinout << "from building import *\n\n";
    pinout << "cwd = GetCurrentDir()\n";
    pinout << "src = Glob('*.c') + Glob('*.cpp')\n";
    pinout << "inc = [cwd]\n\n";
    pinout << "group = DefineGroup('RTduino', src, depend = ['PKG_USING_RTDUINO'], CPPPATH = inc)\n\n";
    pinout << "Return('group')\n";

    pinoutsconsfile.close();
    return true;
}

bool CreateFile::creatPinoutCFile()
{
    QFile file(rtduino->getProjectInfo()->project_path+"/arduino_pinout/pins_arduino.c");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    QString formatstr;

    // 生成注释信息
    out << "/*\n";
    formatstr = QString(" * Copyright (c) 2006-%1, RT-Thread Development Team\n").arg(year);
    out << formatstr;
    out << " *\n";
    out << " * SPDX-License-Identifier: Apache-2.0\n";
    out << " *\n";
    out << " * Change Logs:\n";
    out << " * Date           Author         Notes\n";
    formatstr = QString(" * %1%2first version\n").arg(localTime, -15, ' ').arg(author, -15, ' ');
    out << formatstr;
    out << " */\n\n";

    // 引入头文件
    out << "#include <Arduino.h>\n";
    out << "#include \"pins_arduino.h\"\n\n";

    // 生成引脚对接表
    // 1.注释
    out << "/*\n";
    out << " * {Arduino Pin, RT-Thread Pin [, Device Name, Channel]}\n";
    out << " * [] means optional\n";
    out << " * Digital pins must NOT give the device name and channel.\n";
    out << " * Analog pins MUST give the device name and channel(ADC, PWM or DAC).\n";
    out << " * Arduino Pin must keep in sequence.\n";
    out << " */\n";
    // 2.主体
    out << "const pin_map_t pin_map_table[]=\n";
    out << "{\n";
    foreach(auto pmap , rtduino->getAllPinInfoList())
    {
        // {D3, GET_PIN(B,4), "pwm3", 1},      /* PWM */
        QString linestr = "{";

        if(!pmap.pin_arduino.isEmpty())
            linestr += (pmap.pin_arduino);


        if (!pmap.pin_rttport.isEmpty() && !pmap.pin_rttpin.isEmpty())
        {
            QString rtthread_format = rtduino->getNowMcuSeries().iopininfo.format;
            rtthread_format.replace("(port)", pmap.pin_rttport);
            rtthread_format.replace("(pin)", pmap.pin_rttpin);
            linestr += (", " + rtthread_format);
        }
        else
        {
            linestr += ", RT_NULL";
        }

        if(!pmap.pin_name.isEmpty())
            linestr += (", \"" + pmap.pin_name + "\"");

        if (!QString(pmap.pin_channel).contains(QRegExp("^\\d+$")) && !pmap.pin_channel.isEmpty())
        {
            if (pmap.pin_channel == "VREFINT")
            {
                linestr += (", RT_ADC_INTERN_CH_VREF");
            }
            else if (pmap.pin_channel == "TEMPSENSOR")
            {
                linestr += (", RT_ADC_INTERN_CH_TEMPER");
            }
        }
        else if (QString(pmap.pin_channel).contains(QRegExp("^\\d+$")) && !pmap.pin_channel.isEmpty())
        {
            linestr += (", " + pmap.pin_channel);
        }

        linestr += "};";

        // 注释TAB对齐
        uint8_t tab_align = 40;
        if (tab_align < linestr.size())
        {
            tab_align = 50;
        }

        if(pmap.pin_notes.isEmpty())
            formatstr = QString("    %1\n").arg(linestr);
        else
            formatstr = QString("    %1%2\n").arg(linestr, -tab_align, ' ').arg(pmap.pin_notes);
        out << formatstr;
    }
    out << "};\n";

    out << createPwm2Spi();

    file.close();

    return true;
}

QString CreateFile::createPwm2Spi()
{
    QString pwm2spi_code;

    if (!rtduino->getFunctionInfo()->pwmtospi.spi_name.isEmpty())
    {
        pwm2spi_code += "\n";
        pwm2spi_code += "#ifdef RTDUINO_USING_SPI\n";
        pwm2spi_code += "   #error \"Please modify switchToSPI().\"\n";
        pwm2spi_code += "void switchToSPI(const char *bus_name)\n";
        pwm2spi_code += "{\n";
        pwm2spi_code += "    if(!rt_strcmp(bus_name, \"" +
                rtduino->getFunctionInfo()->pwmtospi.spi_name + "\"))\n";
        pwm2spi_code += "    {\n";
        pwm2spi_code += "        /**" + rtduino->getFunctionInfo()->pwmtospi.spi_name.toUpper() +
                " GPIO Configuration\n";
        pwm2spi_code += "        P" + rtduino->getFunctionInfo()->pwmtospi.sck_port + "." +
                rtduino->getFunctionInfo()->pwmtospi.sck_pin + "     ------> " +
                rtduino->getFunctionInfo()->pwmtospi.spi_name.toUpper() + "_SCK\n";
        pwm2spi_code += "        P" + rtduino->getFunctionInfo()->pwmtospi.miso_port + "." +
                rtduino->getFunctionInfo()->pwmtospi.miso_pin + "     ------> " +
                rtduino->getFunctionInfo()->pwmtospi.spi_name.toUpper() + "_MISO\n";
        pwm2spi_code += "        P" + rtduino->getFunctionInfo()->pwmtospi.mosi_port + "." +
                rtduino->getFunctionInfo()->pwmtospi.mosi_pin + "     ------> " +
                rtduino->getFunctionInfo()->pwmtospi.spi_name.toUpper() + "_MOSI\n";
        pwm2spi_code += "        */\n";
        pwm2spi_code += "        /* Open the SPI peripheral clock */\n\n";
        pwm2spi_code += "        /* DeInit all GPIO */\n\n";
        pwm2spi_code += "        /* Init all GPIO */\n\n";
        pwm2spi_code += "        LOG_W(\""+
                rtduino->getArduinoNumber(rtduino->getFunctionInfo()->pwmtospi.sck_port,
                                          rtduino->getFunctionInfo()->pwmtospi.sck_pin) + ", "+
                rtduino->getArduinoNumber(rtduino->getFunctionInfo()->pwmtospi.miso_port,
                                          rtduino->getFunctionInfo()->pwmtospi.miso_pin) + " and "+
                rtduino->getArduinoNumber(rtduino->getFunctionInfo()->pwmtospi.mosi_port,
                                          rtduino->getFunctionInfo()->pwmtospi.mosi_pin) +
                " will switch from PWM to SPI\");\n";
        pwm2spi_code += "    }\n";
        pwm2spi_code += "}\n";
        pwm2spi_code += "#endif /* RTDUINO_USING_SPI */\n";
    }

    return pwm2spi_code;
}

bool CreateFile::creatPinoutHFile()
{
    QFile file(rtduino->getProjectInfo()->project_path+"/arduino_pinout/pins_arduino.h");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    QString formatstr;

    // 生成注释信息
    out << "/*\n";
    formatstr = QString(" * Copyright (c) 2006-%1, RT-Thread Development Team\n").arg(year);
    out << formatstr;
    out << " *\n";
    out << " * SPDX-License-Identifier: Apache-2.0\n";
    out << " *\n";
    out << " * Change Logs:\n";
    out << " * Date           Author         Notes\n";
    formatstr = QString(" * %1%2first version\n").arg(localTime, -15, ' ').arg(author, -15, ' ');
    out << formatstr;
    out << " */\n\n";

    out << "#ifndef Pins_Arduino_h\n";
    out << "#define Pins_Arduino_h\n\n";

    out << "/* pins alias. Must keep in sequence */\n";

    QString pinMaxLimit;
    for(int i = 0; i < rtduino->getAllPinInfoList().size(); i++)
    {
        formatstr = QString("#define %1(%2)\n").arg(rtduino->getAllPinInfoList().at(i).pin_arduino, -10, ' ').arg(i);
        out << formatstr;
        pinMaxLimit = rtduino->getAllPinInfoList().at(i).pin_arduino;
    }
    out << "\n";

    out << QString("#define RTDUINO_PIN_MAX_LIMIT %1 /* pin number max limit check */\n\n").arg(pinMaxLimit);

    out << QString("#define F_CPU %1 /* CPU:%2MHz */\n\n")
           .arg(rtduino->getFunctionInfo()->frequency + "000000L")
           .arg(rtduino->getFunctionInfo()->frequency);

    if(!rtduino->getFunctionInfo()->ledpin.isEmpty())
    {
        out << QString("#define LED_BUILTIN %1 /* Default Built-in LED */\n\n")
               .arg(rtduino->getFunctionInfo()->ledpin);
    }
    if(!rtduino->getFunctionInfo()->i2c.isEmpty())
    {
        QString sda = "NONE", scl = "NONE";
        foreach (auto pinItem, rtduino->getAllPinInfoList()) {
            if(pinItem.pin_name == rtduino->getFunctionInfo()->i2c && pinItem.pin_iomap == "SDA")
            {
                sda = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
            if(pinItem.pin_name == rtduino->getFunctionInfo()->i2c && pinItem.pin_iomap == "SCL")
            {
                scl = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
        }

        out << QString("/* %1 : %2-SDA %3-SCL */\n")
               .arg(rtduino->getFunctionInfo()->i2c).arg(sda).arg(scl);
        out << QString("#define RTDUINO_DEFAULT_IIC_BUS_NAME \"%1\"\n\n").arg(rtduino->getFunctionInfo()->i2c);
    }
    if(!rtduino->getFunctionInfo()->spi.isEmpty())
    {
        QString miso = "NONE", mosi = "NONE", sck = "NONE";
        foreach (auto pinItem, rtduino->getAllPinInfoList()) {
            if(pinItem.pin_name == rtduino->getFunctionInfo()->spi && pinItem.pin_iomap == "MISO")
            {
                miso = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
            if(pinItem.pin_name == rtduino->getFunctionInfo()->spi && pinItem.pin_iomap == "MOSI")
            {
                mosi = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
            if(pinItem.pin_name == rtduino->getFunctionInfo()->spi && pinItem.pin_iomap == "SCK")
            {
                sck = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
        }
        out << QString("/* %1 : %2-SCK  %3-MISO  %4-MOSI */\n")
               .arg(rtduino->getFunctionInfo()->spi).arg(sck).arg(miso).arg(mosi);
        if(!rtduino->getFunctionInfo()->spiss.isEmpty())
        {
            out << QString("#define SS %1 /* Chip select pin of default spi */\n").arg(rtduino->getFunctionInfo()->spiss);

        }
        out << QString("#define RTDUINO_DEFAULT_SPI_BUS_NAME \"%1\"\n\n").arg(rtduino->getFunctionInfo()->spi);
    }
    if(!rtduino->getFunctionInfo()->serial2.isEmpty())
    {
        QString tx = "NONE", rx = "NONE";
        foreach (auto pinItem, rtduino->getAllPinInfoList()) {
            if(pinItem.pin_name == rtduino->getFunctionInfo()->serial2 && pinItem.pin_iomap == "TX")
            {
                tx = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
            if(pinItem.pin_name == rtduino->getFunctionInfo()->serial2 && pinItem.pin_iomap == "RX")
            {
                rx = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
        }
        out << QString("/* Serial2(%1) : %2-TX  %3-RX */\n")
               .arg(rtduino->getFunctionInfo()->serial2).arg(tx).arg(rx);
        out << QString("#define RTDUINO_SERIAL2_DEVICE_NAME \"%1\"\n\n")
               .arg(rtduino->getFunctionInfo()->serial2);
    }

    if(!rtduino->getFunctionInfo()->serial3.isEmpty())
    {
        QString tx = "NONE", rx = "NONE";
        foreach (auto pinItem, rtduino->getAllPinInfoList()) {
            if(pinItem.pin_name == rtduino->getFunctionInfo()->serial3 && pinItem.pin_iomap == "TX")
            {
                tx = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
            if(pinItem.pin_name == rtduino->getFunctionInfo()->serial3 && pinItem.pin_iomap == "RX")
            {
                rx = "P" + pinItem.pin_rttport + "." + pinItem.pin_rttpin;
            }
        }
        out << QString("/* Serial3(%1) : %2-TX  %3-RX */\n")
               .arg(rtduino->getFunctionInfo()->serial3).arg(tx).arg(rx);
        out << QString("#define RTDUINO_SERIAL3_DEVICE_NAME \"%1\"\n\n").arg(rtduino->getFunctionInfo()->serial3);
    }
    if(!rtduino->getFunctionInfo()->timer.isEmpty())
    {
        out << QString("#define RTDUINO_DEFAULT_HWTIMER_DEVICE_NAME \"%1\"\n\n")
               .arg(rtduino->getFunctionInfo()->timer);
    }

    out << "#endif /* Pins_Arduino_h */\n";

    file.close();

    return true;
}

bool CreateFile::creatMainCppFile()
{
    QFile file(rtduino->getProjectInfo()->project_path+"/arduino_main.cpp");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    QString formatstr;

    // 生成注释信息
    out << "/*\n";
    formatstr = QString(" * Copyright (c) 2006-%1, RT-Thread Development Team\n").arg(year);
    out << formatstr;
    out << " *\n";
    out << " * SPDX-License-Identifier: Apache-2.0\n";
    out << " *\n";
    out << " * Change Logs:\n";
    out << " * Date           Author         Notes\n";
    formatstr = QString(" * %1%2first version\n").arg(localTime, -15, ' ').arg(author, -15, ' ');
    out << formatstr;
    out << " */\n\n";

    out << "#include <Arduino.h>\n\n";
    out << "void setup(void)\n";
    out << "{\n";
    out << "    /* put your setup code here, to run once: */\n";
    if(rtduino->getFunctionInfo()->codedemo == "LED Blinking")
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
    if(rtduino->getFunctionInfo()->codedemo == "LED Blinking")
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

    file.close();

    return true;
}

bool CreateFile::creatKconfigFile()
{
    QFile file(rtduino->getProjectInfo()->project_path+"/Kconfig.demo");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    // 内容
    out << "menu \"Onboard Peripheral Drivers\"\n\n";
    out << "    config BSP_USING_ARDUINO\n";
    out << "        bool \"Compatible with Arduino Ecosystem (RTduino)\"\n";
    out << "        select PKG_USING_RTDUINO\n";
    out << "        select BSP_USING_STLINK_TO_USART\n";
    if(!rtduino->getFunctionInfo()->serial1.isEmpty())
        out << "        select BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial1) +"\n";
    if(!rtduino->getFunctionInfo()->serial2.isEmpty())
        out << "        select BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial2) +"\n";
    if(!rtduino->getFunctionInfo()->serial3.isEmpty())
        out << "        select BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial3) +"\n";
    out << "        select BSP_USING_GPIO\n";

    if(!rtduino->getFunctionInfo()->timer.isEmpty())
    {
        out << "        select BSP_USING_TIM\n";
        out << "        select BSP_USING_TIM" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->timer)+"\n";
    }

    if(!rtduino->getAllDeviceName("adc").isEmpty())
        out << "        select BSP_USING_ADC\n";
    foreach(auto i, rtduino->getAllDeviceName("adc"))
    {
        out << "        select BSP_USING_ADC" +
               rtduino->getDeviceNumber(i) + "\n";
    };

    if(!rtduino->getAllDeviceName("dac").isEmpty())
        out << "        select BSP_USING_DAC\n";
    foreach(auto i, rtduino->getAllDeviceName("dac"))
    {
        out << "        select BSP_USING_DAC" +
               rtduino->getDeviceNumber(i) + "\n";
    };

    if(!rtduino->getAllDeviceName("pwm").isEmpty())
        out << "        select BSP_USING_PWM\n";
    foreach(auto i, rtduino->getAllDeviceName("pwm"))
    {
        out << "        select BSP_USING_PWM" +
               rtduino->getDeviceNumber(i) + "\n";
        QStringList ch_list = rtduino->getDeviceChannel(i);
        foreach(auto c, ch_list)
        {
            out << "        select BSP_USING_PWM" +
                   rtduino->getDeviceNumber(i) + "_CH" + c +"\n";
        }
    };

    if(!rtduino->getAllDeviceName("i2c").isEmpty())
        out << "        select BSP_USING_I2C\n";
    foreach(auto i, rtduino->getAllDeviceName("i2c"))
    {
        out << "        select BSP_USING_I2C" +
               rtduino->getDeviceNumber(i)+"\n";
    };

    if(!rtduino->getAllDeviceName("spi").isEmpty())
        out << "        select BSP_USING_SPI\n";
    foreach(auto i, rtduino->getAllDeviceName("spi"))
    {
        out << "        select BSP_USING_SPI" +
               rtduino->getDeviceNumber(i)+"\n";
    };

    if(!rtduino->getAllDeviceName("pwm").isEmpty())
        out << "        imply RTDUINO_USING_SERVO\n";
    if(!rtduino->getAllDeviceName("i2c").isEmpty())
        out << "        imply RTDUINO_USING_WIRE\n";
    if(!rtduino->getAllDeviceName("spi").isEmpty())
        out << "        imply RTDUINO_USING_SPI\n";
    out << "        default n\n\n";
    out << "endmenu\n\n";
    //---------------------------------------------------------

    if(!rtduino->getFunctionInfo()->serial1.isEmpty() ||
            !rtduino->getFunctionInfo()->serial2.isEmpty() ||
            !rtduino->getFunctionInfo()->serial3.isEmpty())
    {
        out << "menu \"On-chip Peripheral Drivers\"\n\n";
        out << "    menuconfig BSP_USING_UART\n";
        out << "        bool \"Enable UART\"\n";
        out << "        default y\n";
        out << "        select RT_USING_SERIAL\n";
        out << "        if BSP_USING_UART\n";
    }

    if(!rtduino->getFunctionInfo()->serial1.isEmpty())
    {
        out << "            config BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial1)+"\n";
        out << "                bool \"Enable UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial1)+"\"\n";
        out << "                default n\n";
    }

    if(!rtduino->getFunctionInfo()->serial2.isEmpty())
    {
        out << "            config BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial2)+"\n";
        out << "                bool \"Enable UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial2)+"\"\n";
        out << "                default n\n";
    }

    if(!rtduino->getFunctionInfo()->serial3.isEmpty())
    {
        out << "            config BSP_USING_UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial3)+"\n";
        out << "                bool \"Enable UART" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->serial3)+"\"\n";
        out << "                default n\n";
    }

    if(!rtduino->getFunctionInfo()->serial1.isEmpty() ||
            !rtduino->getFunctionInfo()->serial2.isEmpty() ||
            !rtduino->getFunctionInfo()->serial3.isEmpty())
        out << "        endif\n\n";

    if(!rtduino->getFunctionInfo()->timer.isEmpty())
    {
        out << "    menuconfig BSP_USING_TIM\n";
        out << "        bool \"Enable timer\"\n";
        out << "        default n\n";
        out << "        select RT_USING_HWTIMER\n";
        out << "        if BSP_USING_TIM\n";
        out << "            config BSP_USING_TIM" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->timer)+"\n";
        out << "            bool \"Enable TIM" +
               rtduino->getDeviceNumber(rtduino->getFunctionInfo()->timer)+"\"\n";
        out << "                default n\n";
        out << "        endif\n\n";
    }

    out << "    config BSP_USING_GPIO\n";
    out << "        bool \"Enable GPIO\"\n";
    out << "        select RT_USING_PIN\n";
    out << "        default y\n\n";

    if(!rtduino->getAllDeviceName("adc").isEmpty())
    {
        out << "    menuconfig BSP_USING_ADC\n";
        out << "        bool \"Enable ADC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_ADC\n";
        out << "        if BSP_USING_ADC\n";
        foreach(auto i, rtduino->getAllDeviceName("adc"))
        {
            out << "            config BSP_USING_ADC"+
                   rtduino->getDeviceNumber(i)+"\n";
            out << "                bool \"Enable ADC"+
                   rtduino->getDeviceNumber(i)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    if(!rtduino->getAllDeviceName("dac").isEmpty())
    {
        out << "    menuconfig BSP_USING_DAC\n";
        out << "        bool \"Enable DAC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_DAC\n";
        out << "        if BSP_USING_DAC\n";
        foreach(auto i, rtduino->getAllDeviceName("dac"))
        {
            out << "            config BSP_USING_DAC"+
                   rtduino->getDeviceNumber(i)+"\"\n";
            out << "                bool \"Enable DAC"+
                   rtduino->getDeviceNumber(i)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    if(!rtduino->getAllDeviceName("pwm").isEmpty())
    {
        out << "    menuconfig BSP_USING_PWM\n";
        out << "        bool \"Enable PWM\"\n";
        out << "        default n\n";
        out << "        select RT_USING_PWM\n";
        out << "        if BSP_USING_PWM\n";
        foreach(auto i, rtduino->getAllDeviceName("pwm"))
        {
            out << "        menuconfig BSP_USING_PWM"+
                   rtduino->getDeviceNumber(i)+"\n";
            out << "            bool \"Enable timer"+
                   rtduino->getDeviceNumber(i)+" output PWM\"\n";
            out << "            default n\n";
            out << "            if BSP_USING_PWM"+
                   rtduino->getDeviceNumber(i)+"\n";
            QStringList ch_list = rtduino->getDeviceChannel(i);
            foreach(auto c, ch_list)
            {
                out << "                config BSP_USING_PWM"+
                       rtduino->getDeviceNumber(i)+"_CH"+c+"\n";
                out << "                    bool \"Enable PWM"+
                       rtduino->getDeviceNumber(i)+" channel"+c+"\"\n";
                out << "                    default n\n";

            }
            out << "            endif\n\n";
        };
        out << "        endif\n\n";
    }

    if(!rtduino->getAllDeviceName("spi").isEmpty())
    {
        out << "    menuconfig BSP_USING_SPI\n";
        out << "        bool \"Enable SPI BUS\"\n";
        out << "        default n\n";
        out << "        select RT_USING_SPI\n";
        out << "        if BSP_USING_SPI\n";
        foreach(auto i, rtduino->getAllDeviceName("spi"))
        {
            out << "            config BSP_USING_SPI" +
                   rtduino->getDeviceNumber(i)+"\n";
            out << "                bool \"Enable SPI" +
                   rtduino->getDeviceNumber(i)+" BUS\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    if(!rtduino->getAllDeviceName("i2c").isEmpty())
    {
        out << "    menuconfig BSP_USING_I2C1\n";
        out << "        bool \"Enable I2C1 BUS (software simulation)\"\n";
        out << "        default n\n";
        out << "        select RT_USING_I2C\n";
        out << "        select RT_USING_I2C_BITOPS\n";
        out << "        select RT_USING_PIN\n";
        out << "        if BSP_USING_I2C1\n";
        foreach(auto i, rtduino->getAllDeviceName("i2c"))
        {
            out << "            config BSP_I2C"+
                   rtduino->getDeviceNumber(i)+"_SCL_PIN\n";
            out << "                int \"i2c"+
                   rtduino->getDeviceNumber(i)+" scl pin number (P" +
                   rtduino->getDeviceIoPort(i, "SCL") +"." +
                   rtduino->getDeviceIoPin(i, "SCL") + ")\"\n";
            out << "                default "+
                   QString::number(rtduino->getIOrtthreadNumber(
                                       rtduino->getDeviceIoPort(i, "SCL"),
                                       rtduino->getDeviceIoPin(i, "SCL")))+"\n";
            out << "            config BSP_I2C"+
                   rtduino->getDeviceNumber(i)+"_SDA_PIN\n";
            out << "                int \"i2c"+
                   rtduino->getDeviceNumber(i)+" sda pin number (P"+
                   rtduino->getDeviceIoPort(i, "SDA") +"." +
                   rtduino->getDeviceIoPin(i, "SDA") + ")\"\n";
            out << "                default "+
                   QString::number(rtduino->getIOrtthreadNumber(
                                       rtduino->getDeviceIoPort(i, "SDA"),
                                       rtduino->getDeviceIoPin(i, "SDA")))+"\n";
        };
        out << "        endif\n\n";
    }
    out << "endmenu\n";


    file.close();

    return true;
}

bool CreateFile::creatReadmeFile()
{
    QFile file(rtduino->getProjectInfo()->project_path+"/arduino_pinout/README.md");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    out.setCodec("UTF-8");
    // 内容
    out << QString("# %1 ").arg(rtduino->getProjectInfo()->board_name);
    out << QObject::tr("开发板的Arduino生态兼容说明\n\n");
    out << QObject::tr("## 1 RTduino - RT-Thread的Arduino生态兼容层\n\n");
    out << QString("%1 ").arg(rtduino->getProjectInfo()->board_name);
    out << QObject::tr("开发板已经完整适配了[RTduino软件包](https://github.com/RTduino/RTduino)，即RT-Thread的Arduino"\
                       "生态兼容层。用户可以按照Arduino的编程习惯来操作该BSP，并且可以使用大量Arduino社区丰富的库，是"\
                       "对RT-Thread生态的极大增强。更多信息，请参见[RTduino软件包说明文档](https://github.com/RTduino/RTduino)。\n\n");
    out << QObject::tr("### 1.1 如何开启针对本BSP的Arduino生态兼容层\n\n");
    out << QObject::tr("Env 工具下敲入 menuconfig 命令，或者 RT-Thread Studio IDE 下选择 RT-Thread Settings：\n\n");
    out << QObject::tr("```Kconfig\n");
    out << QObject::tr("Hardware Drivers Config --->\n");
    out << QObject::tr("    Onboard Peripheral Drivers --->\n");
    out << QObject::tr("        [*] Compatible with Arduino Ecosystem (RTduino)\n");
    out << QObject::tr("```\n\n");
    out << QObject::tr("## 2 Arduino引脚排布\n\n");
    out << QObject::tr("更多引脚布局相关信息参见 [pins_arduino.c](pins_arduino.c) 和 [pins_arduino.h](pins_arduino.h)。\n\n");
    if(!rtduino->getProjectInfo()->pinmap_image.isEmpty())
    {
        QFileInfo fileinfo(rtduino->getProjectInfo()->pinmap_image);
        out << QString("![%1](%2)\n").arg(fileinfo.baseName()).arg(fileinfo.fileName());//QObject::tr("![xxx-pinout](xxx-pinout.jpg)\n");//
    }
    out << QObject::tr("| Arduino引脚编号  | STM32引脚编号 | 5V容忍 | 备注  |\n");
    out << QObject::tr("| ------------------- | --------- | ---- | ------------------------------------------------------------------------- |\n");
    for(int i = 0; i < rtduino->getAllPinInfoList().size(); i++)
    {
        QString index, ardpin, rttpin, vol53, note;
        index = QString::number(i);

        ardpin = rtduino->getAllPinInfoList().at(i).pin_arduino;

        QStringList rttpininfo = {rtduino->getAllPinInfoList().at(i).pin_rttport, rtduino->getAllPinInfoList().at(i).pin_rttpin};

        if(!rtduino->getAllPinInfoList().at(i).pin_rttport.isEmpty() &&
                !rtduino->getAllPinInfoList().at(i).pin_rttpin.isEmpty())
        {
            rttpin = rtduino->getNowMcuSeries().iopininfo.format;
            rttpin.replace("(port)", rtduino->getAllPinInfoList().at(i).pin_rttport);
            rttpin.replace("(pin)", rtduino->getAllPinInfoList().at(i).pin_rttpin);
        }
        else
        {
            rttpin = "--";
        }

        if (rtduino->getAllPinInfoList().at(i).pin_channel == "VREFINT" ||
                rtduino->getAllPinInfoList().at(i).pin_channel == "TEMPSENSOR")
        {
            vol53 = "";
        }
        else
        {
            if(rtduino->getAllPinInfoList().at(i).pin_5vallow == true)
                vol53 = "是";
            else
                vol53 = "否";
        }

        QString noteDev;

        if(rtduino->getAllPinInfoList().at(i).pin_function == "ADC")
        {
            if(rtduino->getAllPinInfoList().at(i).pin_channel == "VREFINT")
                noteDev = "芯片内部参考电压 ADC";
            else if(rtduino->getAllPinInfoList().at(i).pin_channel == "TEMPSENSOR")
                noteDev = "芯片内部温度 ADC";
            else
                noteDev = rtduino->getAllPinInfoList().at(i).pin_name.toUpper();
        }
        else if(rtduino->getAllPinInfoList().at(i).pin_function == "UART")
        {
            if(rtduino->getAllPinInfoList().at(i).pin_name == rtduino->getFunctionInfo()->serial2)
            {
                noteDev = "Serial2";
            }
            else if(rtduino->getAllPinInfoList().at(i).pin_name == rtduino->getFunctionInfo()->serial3)
            {
                noteDev = "Serial3";
            }
            else
            {
                noteDev = "Serial";
            }
        }
        else
        {
            noteDev = rtduino->getAllPinInfoList().at(i).pin_name.toUpper();
        }

        note = QString("%1%2%3,默认被RT-Thread的%4设备框架%5接管").arg(noteDev)
                .arg(rtduino->getAllPinInfoList().at(i).pin_channel.isEmpty() ?
                         "" : "-CH" + rtduino->getAllPinInfoList().at(i).pin_channel)
                .arg(rtduino->getAllPinInfoList().at(i).pin_iomap.isEmpty() ?
                         "" : "-" + rtduino->getAllPinInfoList().at(i).pin_iomap)
                .arg(rtduino->getAllPinInfoList().at(i).pin_function)
                .arg(rtduino->getAllPinInfoList().at(i).pin_name);
        if(rtduino->getAllPinInfoList().at(i).pin_function == "USB")
        {
            note = QString("%1%2%3,默认被 [TinyUSB软件包](https://github.com/RT-Thread-packages/tinyusb) 接管").arg(noteDev)
                    .arg(rtduino->getAllPinInfoList().at(i).pin_channel.isEmpty() ? "" : "-CH" + rtduino->getAllPinInfoList().at(i).pin_channel)
                    .arg(rtduino->getAllPinInfoList().at(i).pin_iomap.isEmpty() ? "" : "-" + rtduino->getAllPinInfoList().at(i).pin_iomap);
        }
        if(rtduino->getAllPinInfoList().at(i).pin_function == "GPIO")
        {
            note = "";
            if(rtduino->getAllPinInfoList().at(i).pin_arduino == rtduino->getFunctionInfo()->spiss)
            {
                note = "SPI片选默认引脚";
            }
            if(rtduino->getAllPinInfoList().at(i).pin_arduino == rtduino->getFunctionInfo()->ledpin)
            {
                note = "板载用户LED";
            }
        }
        out << "| " +index+" ("+ardpin+ ") | " +rttpin+ " | " +vol53+ " | " +note+ " |\n" ;

    }
    out << QObject::tr("\n");
    out << QObject::tr("> 注意：\n");
    out << QObject::tr(">\n");
    out << QObject::tr("> 1. xxxxxxxxx\n");
    out << QObject::tr("> 2. xxxxxxxxx\n");
    file.close();

    return true;
}

void CreateFile::updateRTThreadNoteInfo()
{
    QDateTime local(QDateTime::currentDateTime());
    localTime = local.toString("yyyy-MM-dd");

    year = localTime.mid(0,4);
    month = localTime.mid(5,2);
    day = localTime.mid(8,2);

    author = rtduino->getProjectInfo()->pinfo_author;
}
