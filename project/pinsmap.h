#ifndef PINSMAP_H
#define PINSMAP_H

#include <QString>
#include <QVector>
#include <QDebug>

struct pinInfo
{
    QString pinArduino; // Arduino编号
    QString pinRtthread;// RT-Thread编号 0-x
    QString pinRttPort; // 宏定义形式 STM32 为 A-K
    QString pinRttPin;  // 宏定义形式 STM32 为 0-15
    QString pinDevFunc; // 引脚设备功能：PWM UART等
    QString pinDevName; // 引脚设备名字 pwm1 uart1等
    QString pinDevChnl; // 引脚设备通道 2 3 4等
    QString pinUseFunc; // 引脚使用功能 SCL MOSI等
    QString pinMapNote; // 注释信息
    QString vol5vAllow; // 5V容忍

    bool operator==(const struct pinInfo &p)
    {
        return (this->pinArduino == p.pinArduino) \
                && (this->pinRtthread == p.pinRtthread) \
                && (this->pinRttPort == p.pinRttPort) \
                && (this->pinRttPin == p.pinRttPin) \
                && (this->pinDevFunc == p.pinDevFunc) \
                && (this->pinDevName == p.pinDevName) \
                && (this->pinDevChnl == p.pinDevChnl) \
                && (this->pinUseFunc == p.pinUseFunc) \
                && (this->vol5vAllow == p.vol5vAllow) \
                && (this->pinMapNote == p.pinMapNote);
    }
};

struct pinFunc
{
    QString devFunc;
    QStringList useFunc;

    QStringList devName;
    QStringList devChnl;
};

struct pinInfoConfig
{
    bool useNumber;

    QVector<struct pinFunc> pinFuncList;

    QStringList rttPort;
    QStringList rttPin;

    QStringList rttNumber;

    QStringList timer;
};

struct _spipin
{
    QString Port;
    QString Pin;
};

struct spiPinInfo
{
    QString spi_name;
    struct _spipin sck;
    struct _spipin miso;
    struct _spipin mosi;
};

class PinsMap
{
public:
    PinsMap();
    ~PinsMap();

    // 类方法
    void pinInfoRefresh();
    void pinInfoShow(void); // 打印pinInfoList
    void pinInfoConfigShow(void); // 打印pinInfoList
    struct pinFunc *findPinFunc(QString devfunc);


    void pinInfoAdd(struct pinInfo &Info);
    void pinInfoSelectedSet(QString pinArduino);
    void pinInfoInsert(QString pinArduino, struct pinInfo &Info);
    void pinInfoDelete(QString pinArduino);
    void pinInfoChange(QString pinArduino, struct pinInfo &Info);
    void pinInfoClear();

    virtual QString getPinName(QStringList pin) = 0;

    // 虚函数接口
    virtual void pinInfoConfigSet() = 0;
    bool pwmTospi;
    virtual QString creatPwmToSpiCode(struct spiPinInfo spipin) = 0;
    virtual QString countNumByRttPin(QString rtt) = 0;

    QVector<struct pinInfo> pinInfoList;

    // 外设列表
    QStringList uartAllDev;
    QStringList pwmAllDev;
    QStringList adcAllDev;
    QStringList dacAllDev;
    QStringList spiAllDev;
    QStringList i2cAllDev;

    QString mcuSeries; // MCU系列
    QString mcuBoard; // MCU系列

    struct pinInfo *selectPin; // 当前选中引脚
    struct spiPinInfo spiPins;

    // funcwidget内容
    QString serial1Dev;
    QString serial2Dev;
    QString serial3Dev;

    QString spiDev;
    QString spissPin;

    QString ledPin;
    QString i2cDev;

    QString timerDev;
    QString authorInfo;
    QString frequency;
    QString codeDemo;

    // pininfowidget页面的数据
    struct pinInfoConfig pinInfoCfg;

    // projwidget数据
    QString proPath;
    QString proName;
    QString board;
    QString srcimagefile; // 图片源文件路劲
    QString aimimagefile; // 图片现文件路劲

protected:
    int pinIndexOfArduino(QString pinArduino); // 根据Arduino编号找到索引值
    void pinInfoSort(void); // 排序
    void pinInfoArduinoUpdate(void);
    void pinInfoNoteCreat();
    void devInfoUpdate(void);
};

#endif // PINSMAP_H
