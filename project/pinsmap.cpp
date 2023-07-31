#include "pinsmap.h"

PinsMap::PinsMap()
{

}

PinsMap::~PinsMap()
{

}

void PinsMap::pinInfoRefresh()
{
    pinInfoArduinoUpdate(); // 更新Arduino引脚
    pinInfoSort();          // 进行排序
    devInfoUpdate();        // 生成设备信息
    pinInfoNoteCreat();     // 生成注释
}

int PinsMap::pinIndexOfArduino(QString pinArduino)
{
    if(pinInfoList.isEmpty())
        return -1;

    for(int i = 0; i < pinInfoList.size(); i++)
    {
        if(pinInfoList.at(i).pinArduino == pinArduino)
        {
            return i;
        }
    }

    return -1;
}

void PinsMap::pinInfoShow()
{
    if(pinInfoList.isEmpty())
        return;

    foreach (auto pinItem, pinInfoList) {
        qDebug() << pinItem.pinArduino << pinItem.pinRtthread << pinItem.pinDevFunc << pinItem.pinDevName << pinItem.pinDevChnl << pinItem.pinUseFunc << pinItem.pinMapNote;
    }

    qDebug() << serial1Dev;
    qDebug() << serial2Dev;
    qDebug() << serial3Dev;
    qDebug() << spiDev;
    qDebug() << spissPin;
    qDebug() << ledPin;
    qDebug() << i2cDev;
    qDebug() << timerDev;
    qDebug() << authorInfo;
    qDebug() << frequency;
    qDebug() << codeDemo;
}

void PinsMap::pinInfoConfigShow()
{
    foreach (auto pinCfgItem, pinInfoCfg.pinFuncList) {
        qDebug() << pinCfgItem.devFunc << pinCfgItem.devName << pinCfgItem.useFunc << pinCfgItem.devChnl;
    }
    qDebug() << pinInfoCfg.rttPort;
    qDebug() << pinInfoCfg.rttPin;
    qDebug() << pinInfoCfg.rttNumber;
}

pinFunc *PinsMap::findPinFunc(QString devfunc)
{
    for (int i = 0; i < pinInfoCfg.pinFuncList.size(); i++) {
        if(pinInfoCfg.pinFuncList[i].devFunc == devfunc)
        {
            return &pinInfoCfg.pinFuncList[i];
        }
    }

    return nullptr;
}

// 排序顺序为：Dx Ax DACx
void PinsMap::pinInfoSort()
{
    if(pinInfoList.isEmpty())
        return;

    // 把Ax移动到Dx后面
    foreach (auto pinItem, pinInfoList) {
        if(pinItem.pinDevFunc == "ADC")
        {
            pinInfoList.move(pinInfoList.indexOf(pinItem), pinInfoList.size() - 1);
        }
    }
    foreach (auto pinItem, pinInfoList) {
        if(pinItem.pinUseFunc.mid(0,6) == "INTVOL")
        {
            pinInfoList.move(pinInfoList.indexOf(pinItem), pinInfoList.size() - 1);
        }
    }
    foreach (auto pinItem, pinInfoList) {
        if(pinItem.pinUseFunc.mid(0,6) == "INTTEP")
        {
            pinInfoList.move(pinInfoList.indexOf(pinItem), pinInfoList.size() - 1);
        }
    }
    // 把DACx移动到Ax后面
    foreach (auto pinItem, pinInfoList) {
        if(pinItem.pinDevFunc == "DAC")
        {
            pinInfoList.move(pinInfoList.indexOf(pinItem), pinInfoList.size() - 1);
        }
    }
}

void PinsMap::pinInfoAdd(pinInfo &Info)
{
    pinInfoList.push_back(Info);
}

void PinsMap::pinInfoSelectedSet(QString pinArduino)
{
    if(pinInfoList.isEmpty())
        return;

    for(int i = 0; i < pinInfoList.size(); i++)
    {
        if(pinInfoList.at(i).pinArduino == pinArduino)
        {
            selectPin = &pinInfoList[i];
            return;
        }
    }

    selectPin = nullptr;
}

void PinsMap::pinInfoInsert(QString pinArduino, struct pinInfo &Info)
{
    int index = pinIndexOfArduino(pinArduino);

    if(index >= 0)
    {
        pinInfoList.insert(index + 1, Info);
    }
}

void PinsMap::pinInfoDelete(QString pinArduino)
{
    int index = pinIndexOfArduino(pinArduino);

    if(index >= 0)
    {
        pinInfoList.removeAt(index);
    }
}

void PinsMap::pinInfoChange(QString pinArduino, pinInfo &Info)
{
    int index = pinIndexOfArduino(pinArduino);

    if(index >= 0)
    {
        pinInfoList.replace(index, Info);
    }
}

void PinsMap::pinInfoArduinoUpdate()
{
    int Anum = 0,Dnum = 0,DACnum = 0;
    if(pinInfoList.isEmpty())
        return;

    for(int i = 0; i < pinInfoList.size(); i++) {
        if(pinInfoList[i].pinDevFunc == "ADC")
        {
            pinInfoList[i].pinArduino = "A" + QString::number(Anum++);
        }
        else
        {
            if(pinInfoList[i].pinDevFunc == "DAC")
            {
                pinInfoList[i].pinArduino = "DAC" + QString::number(DACnum++);
            }
            else
            {
                pinInfoList[i].pinArduino = "D" + QString::number(Dnum++);
            }
        }
    }
}

void PinsMap::pinInfoClear()
{
    if(pinInfoList.isEmpty())
        return;

    pinInfoList.clear();
}

void PinsMap::pinInfoNoteCreat() // 最好做成接口
{
    if(pinInfoList.isEmpty())
        return;

    for(int i = 0; i < pinInfoList.size(); i++) {
        if(pinInfoList[i].pinDevFunc == "GPIO")
        {
            if(spissPin == pinInfoList[i].pinArduino)
            {
                pinInfoList[i].pinMapNote = "/* SPI-SS */";
            }
            if(ledPin == pinInfoList[i].pinArduino)
            {
                pinInfoList[i].pinMapNote = "/* LED_BUILTIN */";
            }
        }
        if(pinInfoList[i].pinDevFunc == "UART")
        {
            if(serial2Dev == pinInfoList[i].pinDevName)
            {
                pinInfoList[i].pinMapNote = "/* Serial2-" + pinInfoList[i].pinUseFunc + " */";
            }
            else if(serial3Dev == pinInfoList[i].pinDevName)
            {
                pinInfoList[i].pinMapNote = "/* Serial3-" + pinInfoList[i].pinUseFunc + " */";
            }
            else
            {
                pinInfoList[i].pinMapNote = "/* Serial-" + pinInfoList[i].pinUseFunc + " */";
            }
        }
        if(pinInfoList[i].pinDevFunc == "PWM")
        {
            pinInfoList[i].pinMapNote = "/* PWM */";
        }
        if(pinInfoList[i].pinDevFunc == "ADC")
        {
            if(pinInfoList[i].pinUseFunc.mid(0, 6) == "INTVOL")
            {
                pinInfoList[i].pinMapNote = "/* ADC, On-Chip: internal reference voltage, ADC_CHANNEL_VREFINT */";
            }
            else if(pinInfoList[i].pinUseFunc.mid(0, 6) == "INTTEP")
            {
                pinInfoList[i].pinMapNote = "/* ADC, On-Chip: internal temperature sensor, ADC_CHANNEL_TEMPSENSOR */";
            }
            else
            {
                pinInfoList[i].pinMapNote = "/* ADC */";
            }
        }
        if(pinInfoList[i].pinDevFunc == "DAC")
        {
            pinInfoList[i].pinMapNote = "/* DAC */";
        }
        if(pinInfoList[i].pinDevFunc == "USB")
        {
            pinInfoList[i].pinMapNote = "/* SerialUSB-" + pinInfoList[i].pinUseFunc + " */";
        }
        if(pinInfoList[i].pinDevFunc == "I2C")
        {
            pinInfoList[i].pinMapNote = "/* I2C-" + pinInfoList[i].pinUseFunc + " (Wire) */";
        }
        if(pinInfoList[i].pinDevFunc == "SPI")
        {
            pinInfoList[i].pinMapNote = "/* SPI-" + pinInfoList[i].pinUseFunc + " */";
        }
    }
}


bool sortNameRules(QString &p1, QString &p2)
{
    if(p1.mid(0,4) == "uart" || p2.mid(0,4) == "uart")
    {
        return p1.mid(4).toInt() < p2.mid(4).toInt();
    }
    else
    {
        return p1.mid(3).toInt() < p2.mid(3).toInt();
    }
}

void PinsMap::devInfoUpdate()
{
    pwmAllDev.clear();
    uartAllDev.clear();
    spiAllDev.clear();
    i2cAllDev.clear();
    adcAllDev.clear();
    dacAllDev.clear();

    if(pinInfoList.isEmpty())
        return;

    foreach(auto pinItem, pinInfoList)
    {
        if(pinItem.pinDevFunc == "PWM")
        {
            if(!pwmAllDev.contains(pinItem.pinDevName))
            {
                pwmAllDev.push_back(pinItem.pinDevName);
            }
        }
        else if(pinItem.pinDevFunc == "UART")
        {
            if(!uartAllDev.contains(pinItem.pinDevName))
            {
                uartAllDev.push_back(pinItem.pinDevName);
            }
        }
        else if(pinItem.pinDevFunc == "SPI")
        {
            if(!spiAllDev.contains(pinItem.pinDevName))
            {
                spiAllDev.push_back(pinItem.pinDevName);
            }
        }
        else if(pinItem.pinDevFunc == "I2C")
        {
            if(!i2cAllDev.contains(pinItem.pinDevName))
            {
                i2cAllDev.push_back(pinItem.pinDevName);
            }
        }
        else if(pinItem.pinDevFunc == "ADC")
        {
            if(!adcAllDev.contains(pinItem.pinDevName))
            {
                adcAllDev.push_back(pinItem.pinDevName);
            }
        }
        else if(pinItem.pinDevFunc == "DAC")
        {
            if(!dacAllDev.contains(pinItem.pinDevName))
            {
                dacAllDev.push_back(pinItem.pinDevName);
            }
        }
        else
        {
        }
    }

    qSort(pwmAllDev.begin(), pwmAllDev.end(), sortNameRules);
    qSort(uartAllDev.begin(), uartAllDev.end(), sortNameRules);
    qSort(spiAllDev.begin(), spiAllDev.end(), sortNameRules);
    qSort(i2cAllDev.begin(), i2cAllDev.end(), sortNameRules);
    qSort(adcAllDev.begin(), adcAllDev.end(), sortNameRules);
    qSort(dacAllDev.begin(), dacAllDev.end(), sortNameRules);

    qDebug() << pwmAllDev << uartAllDev << spiAllDev << i2cAllDev << adcAllDev << dacAllDev;
}
