#include "creatfile.h"
#include <QMessageBox>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include "mcuinfo.h"
#include <QDebug>

CreatFile::CreatFile()
{

}

bool CreatFile::creatAllFiles()
{
    nowMcu = McuInfo::getInstance()->nowMcuSeries;

    getNeedUiInfo();
    if(!creatPinoutCFile())
        return false;
    if(!creatPinoutHFile())
        return false;
    if(!creatSconsFile())
        return false;
    if(!creatMainCppFile())
        return false;
    if(!creatKconfigFile())
        return false;
    if(!creatReadmeFile())
        return false;

    return true;
}

void CreatFile::setProjectPath(QString path)
{
    projPath = path;
}

void CreatFile::getNeedUiInfo()
{
    QDateTime local(QDateTime::currentDateTime());
    localTime = local.toString("yyyy-MM-dd");

    year = localTime.mid(0,4);
    month = localTime.mid(5,2);
    day = localTime.mid(8,2);

    author = McuInfo::getInstance()->nowMcuSeries->authorInfo;
}

bool CreatFile::creatPinoutDir()
{
    QDir arduinodir;

    if(!arduinodir.exists(projPath+"/arduino_pinout"))
    {
        if(!arduinodir.mkdir(projPath+"/arduino_pinout"))
        {
            return false;
        }
    }

    return true;
}

bool CreatFile::creatPinoutCFile()
{
    QFile file(projPath+"/arduino_pinout/pins_arduino.c");
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
    foreach(auto pmap , nowMcu->pinInfoList)
    {
        // {D3, GET_PIN(B,4), "pwm3", 1},      /* PWM */
        QString linestr = "{";

        if(!pmap.pinArduino.isEmpty())
            linestr += (pmap.pinArduino);

        if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
        {
            linestr += (", " + pmap.pinRtthread);
        }
        else
        {
            if(!pmap.pinRttPort.isEmpty() && !pmap.pinRttPin.isEmpty())
                linestr += (", GET_PIN(" + pmap.pinRttPort + "," + pmap.pinRttPin + ")");
            else
                linestr += (", RT_NULL");
        }
        //        if(!pmap.pinRttPort.isEmpty() && !pmap.pinRttPin.isEmpty())
        //            linestr += (", GET_PIN(" + pmap.pinRttPort + "," + pmap.pinRttPin + ")");
        //        else
        //            linestr += (", RT_NULL");
        if(!pmap.pinDevName.isEmpty())
            linestr += (", \"" + pmap.pinDevName + "\"");

        if(pmap.pinUseFunc.mid(0,6) == "INTVOL" && pmap.pinDevChnl.isEmpty())
        {
            linestr += (", RT_ADC_INTERN_CH_VREF");
        }
        if(pmap.pinUseFunc.mid(0,6) == "INTTEP" && pmap.pinDevChnl.isEmpty())
        {
            linestr += (", RT_ADC_INTERN_CH_TEMPER");
        }
        if(!pmap.pinDevChnl.isEmpty())
            linestr += (", " + pmap.pinDevChnl);
        linestr += "};";

        if(pmap.pinMapNote.isEmpty())
            formatstr = QString("    %1%2\n").arg(linestr).arg(pmap.pinMapNote);
        else
            formatstr = QString("    %1%2\n").arg(linestr, -40, ' ').arg(pmap.pinMapNote);
        out << formatstr;
    }
    out << "};\n";
    out << McuInfo::getInstance()->nowMcuSeries->creatPwmToSpiCode(McuInfo::getInstance()->nowMcuSeries->spiPins);
    file.close();

    return true;
}

bool CreatFile::creatPinoutHFile()
{
    QFile file(projPath+"/arduino_pinout/pins_arduino.h");
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
    for(int i = 0; i < nowMcu->pinInfoList.size(); i++)
    {
        formatstr = QString("#define %1(%2)\n").arg(nowMcu->pinInfoList[i].pinArduino, -10, ' ').arg(i);
        out << formatstr;
        pinMaxLimit = nowMcu->pinInfoList[i].pinArduino;
    }
    out << "\n";

    out << QString("#define RTDUINO_PIN_MAX_LIMIT %1 /* pin number max limit check */\n\n").arg(pinMaxLimit);

    out << QString("#define F_CPU %1 /* CPU:%2MHz */\n\n").arg(nowMcu->frequency + "000000L").arg(nowMcu->frequency);

    if(!nowMcu->ledPin.isEmpty())
    {
        out << QString("#define LED_BUILTIN %1 /* Default Built-in LED */\n\n").arg(nowMcu->ledPin);
    }
    if(!nowMcu->i2cDev.isEmpty())
    {
        QStringList sda, scl;
        foreach (auto pinItem, nowMcu->pinInfoList) {
            if(pinItem.pinDevName == nowMcu->i2cDev && pinItem.pinUseFunc == "SDA")
            {
                sda << pinItem.pinRttPort << pinItem.pinRttPin;
            }
            if(pinItem.pinDevName == nowMcu->i2cDev && pinItem.pinUseFunc == "SCL")
            {
                scl << pinItem.pinRttPort << pinItem.pinRttPin;
            }
        }

        out << QString("/* %1 : %2-SDA %3-SCL */\n").arg(nowMcu->i2cDev).arg(nowMcu->getPinName(sda)).arg(nowMcu->getPinName(scl));
        out << QString("#define RTDUINO_DEFAULT_IIC_BUS_NAME \"%1\"\n\n").arg(nowMcu->i2cDev);
    }
    if(!nowMcu->spiDev.isEmpty())
    {
        QStringList miso, mosi, sck;
        foreach (auto pinItem, nowMcu->pinInfoList) {
            if(pinItem.pinDevName == nowMcu->spiDev && pinItem.pinUseFunc == "MISO")
            {
                miso << pinItem.pinRttPort << pinItem.pinRttPin;
            }
            if(pinItem.pinDevName == nowMcu->spiDev && pinItem.pinUseFunc == "MOSI")
            {
                mosi << pinItem.pinRttPort << pinItem.pinRttPin;
            }
            if(pinItem.pinDevName == nowMcu->spiDev && pinItem.pinUseFunc == "SCK")
            {
                sck << pinItem.pinRttPort << pinItem.pinRttPin;
            }
        }
        out << QString("/* %1 : %2-SCK  %3-MISO  %4-MOSI */\n")
               .arg(nowMcu->spiDev).arg(nowMcu->getPinName(sck))
               .arg(nowMcu->getPinName(miso)).arg(nowMcu->getPinName(mosi));
        if(!nowMcu->spissPin.isEmpty())
        {
            out << QString("#define SS %1 /* Chip select pin of default spi */\n").arg(nowMcu->spissPin);

        }
        out << QString("#define RTDUINO_DEFAULT_SPI_BUS_NAME \"%1\"\n\n").arg(nowMcu->spiDev);
    }
    if(!nowMcu->serial2Dev.isEmpty())
    {
        QStringList tx, rx;
        foreach (auto pinItem, nowMcu->pinInfoList) {
            if(pinItem.pinDevName == nowMcu->serial2Dev && pinItem.pinUseFunc == "TX")
            {
                tx << pinItem.pinRttPort << pinItem.pinRttPin;
            }
            if(pinItem.pinDevName == nowMcu->serial2Dev && pinItem.pinUseFunc == "RX")
            {
                rx << pinItem.pinRttPort << pinItem.pinRttPin;
            }
        }
        out << QString("/* Serial2(%1) : %2-TX  %3-RX */\n").arg(nowMcu->serial2Dev).arg(nowMcu->getPinName(tx)).arg(nowMcu->getPinName(rx));
        out << QString("#define RTDUINO_SERIAL2_DEVICE_NAME \"%1\"\n\n").arg(nowMcu->serial2Dev);
    }

    if(!nowMcu->serial3Dev.isEmpty())
    {
        QStringList tx, rx;
        foreach (auto pinItem, nowMcu->pinInfoList) {
            if(pinItem.pinDevName == nowMcu->serial3Dev && pinItem.pinUseFunc == "TX")
            {
                tx << pinItem.pinRttPort << pinItem.pinRttPin;
            }
            if(pinItem.pinDevName == nowMcu->serial3Dev && pinItem.pinUseFunc == "RX")
            {
                rx << pinItem.pinRttPort << pinItem.pinRttPin;
            }
        }
        out << QString("/* Serial3(%1) : %2-TX  %3-RX */\n").arg(nowMcu->serial3Dev).arg(nowMcu->getPinName(tx)).arg(nowMcu->getPinName(rx));
        out << QString("#define RTDUINO_SERIAL3_DEVICE_NAME \"%1\"\n\n").arg(nowMcu->serial3Dev);
    }
    if(!nowMcu->timerDev.isEmpty())
    {
        out << QString("#define RTDUINO_DEFAULT_HWTIMER_DEVICE_NAME \"%1\"\n\n").arg(nowMcu->timerDev);
    }

    out << "#endif /* Pins_Arduino_h */\n";

    file.close();

    return true;
}

bool CreatFile::creatMainCppFile()
{
    QFile file(projPath+"/arduino_main.cpp");
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
    if(nowMcu->codeDemo == "LED Blinking")
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
    if(nowMcu->codeDemo == "LED Blinking")
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

bool CreatFile::creatSconsFile()
{
    QFile mainsconsfile(projPath+"/SConscript");
    QFile pinoutsconsfile(projPath+"/arduino_pinout/SConscript");
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

bool sortRules(QString &p1, QString &p2)
{
    if(p1.mid(0,3) == "spi" || p2.mid(0,3) == "spi")
    {
        return p1.mid(3).toInt() < p2.mid(3).toInt();
    }
    else
    {
        return p1.toInt() < p2.toInt();
    }
}

bool CreatFile::creatKconfigFile()
{
    QFile file(projPath+"/Kconfig.demo");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);
    QStringList tempList;
    QStringList spi_list;

    // 内容
    out << "menu \"Onboard Peripheral Drivers\"\n\n";
    out << "    config BSP_USING_ARDUINO\n";
    out << "        bool \"Compatible with Arduino Ecosystem (RTduino)\"\n";
    out << "        select PKG_USING_RTDUINO\n";
    out << "        select BSP_USING_STLINK_TO_USART\n";
    if(!nowMcu->serial2Dev.isEmpty())
        out << "        select BSP_USING_UART" + nowMcu->serial2Dev.mid(4,nowMcu->serial2Dev.size()-4)+"\n";
    if(!nowMcu->serial3Dev.isEmpty())
        out << "        select BSP_USING_UART" + nowMcu->serial3Dev.mid(4,nowMcu->serial3Dev.size()-4)+"\n";

    out << "        select BSP_USING_GPIO\n";

    if(!nowMcu->timerDev.isEmpty())
    {
        out << "        select BSP_USING_TIM\n";
        out << "        select BSP_USING_TIM"+nowMcu->timerDev.mid(5,nowMcu->timerDev.size()-5)+"\n";
    }

    if(!nowMcu->adcAllDev.isEmpty())
        out << "        select BSP_USING_ADC\n";
    foreach(auto i,nowMcu->adcAllDev)
    {
        out << "        select BSP_USING_ADC"+i.mid(3,i.size()-3)+"\n";
    };

    if(!nowMcu->dacAllDev.isEmpty())
        out << "        select BSP_USING_DAC\n";
    foreach(auto i,nowMcu->dacAllDev)
    {
        out << "        select BSP_USING_DAC"+i.mid(3,i.size()-3)+"\n";
    };

    if(!nowMcu->pwmAllDev.isEmpty())
        out << "        select BSP_USING_PWM\n";
    foreach(auto i,nowMcu->pwmAllDev)
    {
        out << "        select BSP_USING_PWM"+i.mid(3,i.size()-3)+"\n";
        tempList.clear();
        foreach(auto pinItem, nowMcu->pinInfoList)
        {
            if(pinItem.pinDevName == i)
            {
                if(pinItem.pinDevChnl.toInt() < 0)
                    pinItem.pinDevChnl = QString::number(-pinItem.pinDevChnl.toInt());
                if(!tempList.contains(pinItem.pinDevChnl))
                {
                    tempList.push_back(pinItem.pinDevChnl);

                }
            }
        }
        qSort(tempList.begin(), tempList.end(), sortRules);
        qDebug() << tempList;
        foreach(auto c,tempList)
        {
            out << "        select BSP_USING_PWM"+i.mid(3,i.size()-3)+"_CH"+c+"\n";
        }
    };
    tempList.clear();

    if(!nowMcu->i2cAllDev.isEmpty())
        out << "        select BSP_USING_I2C\n";
    foreach(auto i,nowMcu->i2cAllDev)
    {
        out << "        select BSP_USING_I2C"+i.mid(3,i.size()-3)+"\n";
    };

    tempList += nowMcu->spiAllDev;
    if(!tempList.contains(nowMcu->spiPins.spi_name) && !nowMcu->spiPins.spi_name.isEmpty() && nowMcu->spiPins.spi_name.mid(0, 3) == "spi")
        tempList += nowMcu->spiPins.spi_name;
    qDebug() << tempList;

    if(!tempList.isEmpty())
        out << "        select BSP_USING_SPI\n";
    qSort(tempList.begin(), tempList.end(), sortRules);
    spi_list = tempList;
    qDebug() << "spi_list" << spi_list;
    foreach(auto i,spi_list)
    {
        out << "        select BSP_USING_"+i.toUpper()+"\n";
    };

    if(!nowMcu->pwmAllDev.isEmpty())
        out << "        imply RTDUINO_USING_SERVO\n";
    if(!nowMcu->i2cDev.isEmpty())
        out << "        imply RTDUINO_USING_WIRE\n";
    if(!nowMcu->spiDev.isEmpty())
        out << "        imply RTDUINO_USING_SPI\n";
    out << "        default n\n\n";
    out << "endmenu\n\n";
    //---------------------------------------------------------

    if(!nowMcu->serial2Dev.isEmpty() || !nowMcu->serial3Dev.isEmpty())
    {
        out << "menu \"On-chip Peripheral Drivers\"\n\n";
        out << "    menuconfig BSP_USING_UART\n";
        out << "        bool \"Enable UART\"\n";
        out << "        default y\n";
        out << "        select RT_USING_SERIAL\n";
        out << "        if BSP_USING_UART\n";
    }

    if(!nowMcu->serial2Dev.isEmpty())
    {
        out << "            config BSP_USING_UART"+nowMcu->serial2Dev.mid(4,nowMcu->serial2Dev.size()-4)+"\n";
        out << "                bool \"Enable UART"+nowMcu->serial2Dev.mid(4,nowMcu->serial2Dev.size()-4)+"\"\n";
        out << "                default n\n";
    }
    if(!nowMcu->serial3Dev.isEmpty())
    {
        out << "            config BSP_USING_UART"+nowMcu->serial3Dev.mid(4,nowMcu->serial3Dev.size()-4)+"\n";
        out << "                bool \"Enable UART"+nowMcu->serial3Dev.mid(4,nowMcu->serial3Dev.size()-4)+"\"\n";
        out << "                default n\n";
    }
    if(!nowMcu->serial2Dev.isEmpty() || !nowMcu->serial3Dev.isEmpty())
        out << "        endif\n\n";

    if(!nowMcu->timerDev.isEmpty())
    {
        out << "    menuconfig BSP_USING_TIM\n";
        out << "        bool \"Enable timer\"\n";
        out << "        default n\n";
        out << "        select RT_USING_HWTIMER\n";
        out << "        if BSP_USING_TIM\n";
        out << "            config BSP_USING_TIM"+nowMcu->timerDev.mid(5,nowMcu->timerDev.size()-5)+"\n";
        out << "            bool \"Enable TIM"+nowMcu->timerDev.mid(5,nowMcu->timerDev.size()-5)+"\"\n";
        out << "                default n\n";
        out << "        endif\n\n";
    }

    if(!nowMcu->adcAllDev.isEmpty())
    {
        out << "    menuconfig BSP_USING_ADC\n";
        out << "        bool \"Enable ADC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_ADC\n";
        out << "        if BSP_USING_ADC\n";
        foreach(auto i,nowMcu->adcAllDev)
        {
            out << "            config BSP_USING_ADC"+i.mid(3,i.size()-3)+"\n";
            out << "                bool \"Enable ADC"+i.mid(3,i.size()-3)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    if(!nowMcu->dacAllDev.isEmpty())
    {
        out << "    menuconfig BSP_USING_DAC\n";
        out << "        bool \"Enable DAC\"\n";
        out << "        default n\n";
        out << "        select RT_USING_DAC\n";
        out << "        if BSP_USING_DAC\n";
        foreach(auto i,nowMcu->dacAllDev)
        {
            out << "            config BSP_USING_DAC"+i.mid(3,i.size()-3)+"\n";
            out << "                bool \"Enable DAC"+i.mid(3,i.size()-3)+"\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    if(!nowMcu->pwmAllDev.isEmpty())
    {
        out << "    menuconfig BSP_USING_PWM\n";
        out << "        bool \"Enable PWM\"\n";
        out << "        default n\n";
        out << "        select RT_USING_PWM\n";
        out << "        if BSP_USING_PWM\n";
        foreach(auto i,nowMcu->pwmAllDev)
        {
            out << "        menuconfig BSP_USING_PWM"+i.mid(3,i.size()-3)+"\n";
            out << "            bool \"Enable timer"+i.mid(3,i.size()-3)+" output PWM\"\n";
            out << "            default n\n";
            out << "            if BSP_USING_PWM"+i.mid(3,i.size()-3)+"\n";
            tempList.clear();
            foreach(auto pinItem, nowMcu->pinInfoList)
            {
                if(pinItem.pinDevName == i)
                {
                    if(pinItem.pinDevChnl.toInt() < 0)
                        pinItem.pinDevChnl = QString::number(-pinItem.pinDevChnl.toInt());
                    if(!tempList.contains(pinItem.pinDevChnl))
                    {
                        tempList.push_back(pinItem.pinDevChnl);

                    }
                }
            }
            qSort(tempList.begin(), tempList.end(), sortRules);
            qDebug() << tempList;
            foreach(auto c,tempList)
            {
                out << "                config BSP_USING_PWM"+i.mid(3,i.size()-3)+"_CH"+c+"\n";
                out << "                    bool \"Enable PWM"+i.mid(3,i.size()-3)+" channel"+c+"\"\n";
                out << "                    default n\n";

            }
            out << "            endif\n\n";
        };
        out << "        endif\n\n";
    }

    if(!spi_list.isEmpty())
    {
        out << "    menuconfig BSP_USING_SPI\n";
        out << "        bool \"Enable SPI BUS\"\n";
        out << "        default n\n";
        out << "        select RT_USING_SPI\n";
        out << "        if BSP_USING_SPI\n";
        foreach(auto i,spi_list)
        {
            out << "            config BSP_USING_SPI"+i.mid(3,i.size()-3)+"\n";
            out << "                bool \"Enable SPI"+i.mid(3,i.size()-3)+" BUS\"\n";
            out << "                default n\n";
        };
        out << "        endif\n\n";
    }

    // i2c
    QString pinscl ,pinsda;
    QString pinsclstr = "NONE",pinsdastr = "NONE";
    foreach(auto i,nowMcu->pinInfoList)
    {
        if(i.pinDevName == nowMcu->i2cDev && i.pinUseFunc == "SCL")
        {
            pinsclstr = i.pinRttPort + i.pinRttPin;
            pinscl = nowMcu->countNumByRttPin(i.pinRttPort + i.pinRttPin);
        }

        if(i.pinDevName == nowMcu->i2cDev && i.pinUseFunc ==  "SDA")
        {
            pinsdastr = i.pinRttPort + i.pinRttPin;
            pinsda = nowMcu->countNumByRttPin(i.pinRttPort + i.pinRttPin);
        }
    }

    if(!nowMcu->i2cAllDev.isEmpty())
    {
        out << "    menuconfig BSP_USING_I2C1\n";
        out << "        bool \"Enable I2C1 BUS (software simulation)\"\n";
        out << "        default n\n";
        out << "        select RT_USING_I2C\n";
        out << "        select RT_USING_I2C_BITOPS\n";
        out << "        select RT_USING_PIN\n";
        out << "        if BSP_USING_I2C1\n";
        foreach(auto i,nowMcu->i2cAllDev)
        {
            out << "            config BSP_I2C"+i.mid(3,i.size()-3)+"_SCL_PIN\n";
            out << "                int \"i2c"+i.mid(3,i.size()-3)+" scl pin number (P"+ pinsclstr + ")\"\n";
            out << "                default "+pinscl+"\n";
            out << "            config BSP_I2C"+i.mid(3,i.size()-3)+"_SDA_PIN\n";
            out << "                int \"i2c"+i.mid(3,i.size()-3)+" sda pin number (P"+ pinsdastr + ")\"\n";
            out << "                default "+pinsda+"\n";
        };
        out << "        endif\n\n";
    }
    out << "endmenu\n";


    file.close();

    return true;
}

bool CreatFile::creatReadmeFile()
{
    QFile file(projPath+"/arduino_pinout/README.md");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text|QFile::Truncate))
    {
        return false;
    }

    QTextStream out(&file);

    out.setCodec("UTF-8");
    // 内容
    out << QString("# %1 ").arg(nowMcu->mcuBoard);
    out << QObject::tr("开发板的Arduino生态兼容说明\n\n");
    out << QObject::tr("## 1 RTduino - RT-Thread的Arduino生态兼容层\n\n");
    out << QString("%1 ").arg(nowMcu->mcuBoard);
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
    if(!McuInfo::getInstance()->nowMcuSeries->aimimagefile.isEmpty())
    {
        QFileInfo fileinfo(McuInfo::getInstance()->nowMcuSeries->aimimagefile);
        out << QString("![%1](%2)\n").arg(fileinfo.baseName()).arg(fileinfo.fileName());//QObject::tr("![xxx-pinout](xxx-pinout.jpg)\n");//
    }
    out << QObject::tr("| Arduino引脚编号  | STM32引脚编号 | 5V容忍 | 备注  |\n");
    out << QObject::tr("| ------------------- | --------- | ---- | ------------------------------------------------------------------------- |\n");
    for(int i = 0; i < nowMcu->pinInfoList.size(); i++)
    {
        QString index, ardpin, rttpin, vol53, note;
        index = QString::number(i);

        ardpin = nowMcu->pinInfoList[i].pinArduino;

        QStringList rttpininfo = {nowMcu->pinInfoList[i].pinRttPort, nowMcu->pinInfoList[i].pinRttPin};

        if(!nowMcu->pinInfoList[i].pinRttPort.isEmpty() && !nowMcu->pinInfoList[i].pinRttPin.isEmpty())
            rttpin = nowMcu->getPinName(rttpininfo);
        else
            rttpin = "--";

        if(nowMcu->pinInfoList[i].pinUseFunc.mid(0,6) == "INTVOL")
            vol53 = "";
        else if(nowMcu->pinInfoList[i].pinUseFunc.mid(0,6) == "INTTEP")
            vol53 = "";
        else
        {
            if(nowMcu->pinInfoList[i].vol5vAllow == "Yes")
                vol53 = "是";
            else
                vol53 = "否";
        }

        QString noteDev;

        if(nowMcu->pinInfoList[i].pinDevFunc == "ADC")
        {
            if(nowMcu->pinInfoList[i].pinUseFunc.mid(0, 6) == "INTVOL")
                noteDev = "芯片内部参考电压 ADC";
            else if(nowMcu->pinInfoList[i].pinUseFunc.mid(0, 6) == "INTTEP")
                noteDev = "芯片内部温度 ADC";
            else
                noteDev = nowMcu->pinInfoList[i].pinDevName.toUpper();
        }
        else if(nowMcu->pinInfoList[i].pinDevFunc == "UART")
        {
            if(nowMcu->pinInfoList[i].pinDevName == nowMcu->serial2Dev)
            {
                noteDev = "Serial2";
            }
            else if(nowMcu->pinInfoList[i].pinDevName == nowMcu->serial3Dev)
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
            noteDev = nowMcu->pinInfoList[i].pinDevName.toUpper();
        }

        note = QString("%1%2%3,默认被RT-Thread的%4设备框架%5接管").arg(noteDev)
                .arg(nowMcu->pinInfoList[i].pinDevChnl.isEmpty() ? "" : "-CH" + nowMcu->pinInfoList[i].pinDevChnl)
                .arg(nowMcu->pinInfoList[i].pinUseFunc.isEmpty() ? "" : "-" + nowMcu->pinInfoList[i].pinUseFunc)
                .arg(nowMcu->pinInfoList[i].pinDevFunc)
                .arg(nowMcu->pinInfoList[i].pinDevName);
        if(nowMcu->pinInfoList[i].pinDevFunc == "USB")
        {
            note = QString("%1%2%3,默认被 [TinyUSB软件包](https://github.com/RT-Thread-packages/tinyusb) 接管").arg(noteDev)
                    .arg(nowMcu->pinInfoList[i].pinDevChnl.isEmpty() ? "" : "-CH" + nowMcu->pinInfoList[i].pinDevChnl)
                    .arg(nowMcu->pinInfoList[i].pinUseFunc.isEmpty() ? "" : "-" + nowMcu->pinInfoList[i].pinUseFunc);
        }
        if(nowMcu->pinInfoList[i].pinDevFunc == "GPIO")
        {
            note = "";
            if(nowMcu->pinInfoList[i].pinArduino == nowMcu->spissPin)
            {
                note = "SPI片选默认引脚";
            }
            if(nowMcu->pinInfoList[i].pinArduino == nowMcu->ledPin)
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
