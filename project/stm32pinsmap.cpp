#include "stm32pinsmap.h"

void Stm32PinsMap::pinInfoConfigSet()
{
    qDebug() << "stm32 pinInfoConfigSet!";

    struct pinFunc func;

    func.devFunc = "GPIO";
    func.devName.clear();
    func.devChnl.clear();
    func.useFunc.clear();
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "UART";
    func.devName.clear();
    for(int i = 1; i <= 8; i++)
    {
        func.devName << "uart" +QString::number(i);
    }
    func.devName<< "lpuart1";
    func.devChnl.clear();
    func.useFunc.clear();
    func.useFunc << "TX" << "RX";
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "PWM";
    func.devName.clear();
    for(int i = 1; i <= 18; i++)
    {
        func.devName << "pwm" +QString::number(i);
    }
    func.devChnl.clear();
    for(int i = 1; i <= 4; i++)
    {
        func.devChnl << QString::number(i);
    }
    for(int i = -1; i >= -4; i--)
    {
        func.devChnl << QString::number(i);
    }
    func.useFunc.clear();
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "ADC";
    func.devName.clear();
    for(int i = 1; i <= 3; i++)
    {
        func.devName << "adc" +QString::number(i);
    }
    func.devChnl.clear();
    for(int i = 0; i <= 19; i++)
    {
        func.devChnl << QString::number(i);
    }
    func.useFunc.clear();
    func.useFunc << "EXTVOL(外部电压)" << "INTVOL(内部电压)" << "INTTEP(内部温度)";
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "DAC";
    func.devName.clear();
    for(int i = 1; i <= 2; i++)
    {
        func.devName << "dac" +QString::number(i);
    }
    func.devChnl.clear();
    for(int i = 1; i <= 2; i++)
    {
        func.devChnl << QString::number(i);
    }
    func.useFunc.clear();
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "SPI";
    func.devName.clear();
    for(int i = 1; i <= 6; i++)
    {
        func.devName << "spi" +QString::number(i);
    }
//    for(int i = 1; i <= 2; i++)
//    {
//        func.devName << "sspi" +QString::number(i);
//    }
//    func.devName << "qspi1";
    func.devChnl.clear();
    func.useFunc.clear();
    func.useFunc << "MISO" << "MOSI" << "SCK";
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "I2C";
    func.devName.clear();
    for(int i = 1; i <= 4; i++)
    {
        func.devName << "i2c" +QString::number(i);
    }

    func.devChnl.clear();
    func.useFunc.clear();
    func.useFunc << "SCL" << "SDA";
    pinInfoCfg.pinFuncList.push_back(func);

    func.devFunc = "USB";
    func.devName.clear();
    func.devName << "usb" << "usbd" << "usbh";
    func.devChnl.clear();
    func.useFunc.clear();
    func.useFunc << "DP" << "DM";
    pinInfoCfg.pinFuncList.push_back(func);

    pinInfoCfg.useNumber = false;

    pinInfoCfg.rttPort << "A" << "B" << "C" << "D" << "E" << "F" << "G" << "H" << "I" << "J" << "K";
    for(int i = 0; i <= 15; i++)
    {
        pinInfoCfg.rttPin << QString::number(i);
    }

    int allpin = pinInfoCfg.rttPort.size() * pinInfoCfg.rttPin.size();
    for(int i = 0; i < allpin; i++)
    {
        pinInfoCfg.rttNumber << QString::number(i);
    }

    for(int i = 1; i <= 17; i++)
    {
        pinInfoCfg.timer << "timer" + QString::number(i);
    }

    pinInfoConfigShow();

    pwmTospi = true;
}

QString Stm32PinsMap::creatPwmToSpiCode(spiPinInfo spipin)
{
    QString code;
    code += "\n";
    code += "#ifdef RTDUINO_USING_SPI\n";
    code += "void switchToSPI(const char *bus_name)\n";
    code += "{\n";
    code += "    GPIO_InitTypeDef GPIO_InitStruct = {0};\n\n";
    code += "    if(!rt_strcmp(bus_name, \"" + spipin.spi_name + "\"))\n";
    code += "    {\n";
    code += "        __HAL_RCC_" + spipin.spi_name.toUpper() + "_CLK_ENABLE();\n";
    code += "        __HAL_RCC_GPIO" + spipin.sck.Port + "_CLK_ENABLE();\n";
    code += "        __HAL_RCC_GPIO" + spipin.miso.Port + "_CLK_ENABLE();\n";
    code += "        __HAL_RCC_GPIO" + spipin.mosi.Port + "_CLK_ENABLE();\n\n";
    code += "        HAL_GPIO_DeInit(GPIO" + spipin.sck.Port + ", GPIO_PIN_" + spipin.sck.Pin + ");\n";
    code += "        HAL_GPIO_DeInit(GPIO" + spipin.miso.Port + ", GPIO_PIN_" + spipin.miso.Pin + ");\n";
    code += "        HAL_GPIO_DeInit(GPIO" + spipin.mosi.Port + ", GPIO_PIN_" + spipin.mosi.Pin + ");\n\n";
    code += "        /**" + spipin.spi_name.toUpper() + " GPIO Configuration\n";
    code += "        P" + spipin.sck.Port + spipin.sck.Pin + "     ------> " + spipin.spi_name.toUpper() + "_SCK\n";
    code += "        P" + spipin.miso.Port + spipin.miso.Pin + "     ------> " + spipin.spi_name.toUpper() + "_MISO\n";
    code += "        P" + spipin.mosi.Port + spipin.mosi.Pin + "     ------> " + spipin.spi_name.toUpper() + "_MOSI\n";
    code += "        */\n";
    code += "        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;\n";
    code += "        GPIO_InitStruct.Pull = GPIO_NOPULL;\n";
    code += "        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;\n";
    code += "        GPIO_InitStruct.Alternate = GPIO_AF5_" + spipin.spi_name.toUpper() + ";\n\n";
    code += "        GPIO_InitStruct.Pin = GPIO_PIN_" + spipin.sck.Pin + ";\n";
    code += "        HAL_GPIO_Init(GPIO" + spipin.sck.Port + ", &GPIO_InitStruct);\n";
    code += "        GPIO_InitStruct.Pin = GPIO_PIN_" + spipin.miso.Pin + ";\n";
    code += "        HAL_GPIO_Init(GPIO" + spipin.miso.Port + ", &GPIO_InitStruct);\n";
    code += "        GPIO_InitStruct.Pin = GPIO_PIN_" + spipin.mosi.Pin + ";\n";
    code += "        HAL_GPIO_Init(GPIO" + spipin.mosi.Port + ", &GPIO_InitStruct);\n\n";
    code += "        LOG_W(\"D11, D12 and D13 will switch from PWM to SPI\");\n";
    code += "    }\n";
    code += "}\n";
    code += "#endif /* RTDUINO_USING_SPI */\n";

    return code;
}

#define PIN_NUM(port, no) (((((port) & 0xFu) << 4) | ((no) & 0xFu)))

QString Stm32PinsMap::countNumByRttPin(QString rtt)
{
    int hw_port_num, hw_pin_num = 0;
    QByteArray rttarr = rtt.toUtf8();
    char * rttStr = rttarr.data();
    hw_port_num =  rttStr[0] - 'A';
    hw_pin_num = rtt.mid(1,rtt.size()-1).toInt();
    return QString::number(PIN_NUM(hw_port_num, hw_pin_num));
}
