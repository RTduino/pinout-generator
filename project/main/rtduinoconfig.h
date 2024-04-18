#ifndef RTDUINOCONFIG_H
#define RTDUINOCONFIG_H

#include <QRegExp>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonParseError>
#include <QApplication>

#define MCU_STORAGE_PATH "/mcus"

//========== MCU SERIES START ==========//
typedef struct io_function
{
    QString function;
    QStringList ioremaps;
    QStringList names;
    QStringList channels;
    QString help;
} io_function_t;

typedef struct io_pin_info
{
    QString format;
    QStringList ports;
    QStringList pins;
} io_pin_info_t;

typedef struct mcu_series
{
    QString series;
    QStringList childseries;
    QString date;
    QString author;
    io_pin_info_t iopininfo;
    QStringList timers;
    QList<io_function_t> iofunctions;
} mcu_series_t;
//=========== MCU SERIES END ===========//

//========== PIN INFO START ==========//
typedef struct pin_info
{
    QString pin_arduino;    // Arduino编号
    QString pin_rtthread;   // RT-Thread编号
    QString pin_rttport;    // 宏定义形式 STM32 为 A-K
    QString pin_rttpin;     // 宏定义形式 STM32 为 0-15
    QString pin_function;   // 引脚设备功能：PWM UART等
    QString pin_name;       // 引脚设备名字 pwm1 uart1等
    QString pin_channel;    // 引脚设备通道 2 3 4等
    QString pin_iomap;      // 引脚映射功能 SCL MOSI等
    QString pin_notes;      // 注释信息
    bool pin_5vallow;       // 5V容忍
} pin_info_t;

typedef struct pwm_spi
{
    QString spi_name;
    QString sck_port;
    QString sck_pin;
    QString miso_port;
    QString miso_pin;
    QString mosi_port;
    QString mosi_pin;
} pwm_spi_t;

typedef struct func_info
{
    QString serial1;
    QString serial2;
    QString serial3;
    QString spi;
    QString spiss;
    QString ledpin;
    QString i2c;
    QString timer;
    QString frequency;
    QString codedemo;
    pwm_spi_t pwmtospi;
} func_info_t;

typedef struct proj_info
{
    QString pinfo_series;
    QString child_series;
    QString pinfo_author;
    QString board_name;
    QString project_name;
    QString project_path;
    QString pinmap_image;
} proj_info_t;

//=========== PIN INFO END ===========//

class RTduinoConfig
{
public:
    RTduinoConfig();

    /* 单例模式 START */
    static RTduinoConfig *instance;
    static RTduinoConfig *getInstance() {
        if(nullptr == instance)
        {
            instance = new RTduinoConfig();
        }
        return instance;
    }
    /* 单例模式 END */

    // 解析文件
    bool parseMcuSeriesJson(QString file);
    bool parsePinInfoJson(QString file);
    bool createPinInfoJson(QString file);

    // 操作芯片系列
    bool getAllMcuSeries(QStringList &mlist);
    bool setNowMcuSeries(QString series_name);
    io_function_t getIoFunctionConfig(QString function);
    mcu_series_t getMcuSeries(QString series_name);
    mcu_series_t getNowMcuSeries(void)
    {
        return now_series;
    }
    // 操作PININFO
    void addPinInfo(pin_info_t info);
    bool insertPinInfo(QString arduino, pin_info_t info);
    bool removePinInfo(QString arduino);
    bool changePinInfo(QString arduino, pin_info_t info);
    QStringList getArduinoDigital(void);
    QString getArduinoNumber(QString port, QString pin);
    pin_info_t getPinInfo(QString arduino);
    void clearPinInfoList(void)
    {
        all_pin_infos.clear();
    }
    QList<pin_info_t> getAllPinInfoList(void)
    {
        return all_pin_infos;
    }

    QStringList getAllDeviceName(QString device);
    QStringList getDeviceChannel(QString name);
    QString getDeviceNumber(QString name);
    QString getDeviceIoPort(QString name, QString iomap);
    QString getDeviceIoPin(QString name, QString iomap);
    int getIOrtthreadNumber(QString port, QString pin);
    proj_info_t *getProjectInfo()
    {
        return &project_info;
    }
    void clearProjectInfo()
    {
        proj_info_t _info;
        project_info = _info;
    }
    func_info_t *getFunctionInfo()
    {
        return &function_info;
    }
    void clearFunctionInfo()
    {
        func_info_t _info;
        function_info = _info;
    }

    // 自动检测更新
    void setAutoCheckUpdate(bool auto_check)
    {
        auto_check_update = auto_check;
    }
    bool getAutoCheckUpdate(void)
    {
        return auto_check_update;
    }
private:
    bool getJsonObject(QString file, QJsonObject &json_object);
    mcu_series_t *findMcuSeries(QString series_name);
    int findIndexPinInfo(QString arduino);
    void praseAllMcuSeries(void);

    void sortPinInfo(void);
    void updatePinInfoArduino(void);
    void createPinInfoNote(void);
    void fullPinInfo(void);
    proj_info_t project_info;
    func_info_t function_info;
    QList<pin_info_t> all_pin_infos;

    // 打开软件的时候读取配置文件获取，全程不可修改
    mcu_series_t now_series;
    QList<mcu_series_t> all_series;

    // 软件启动自动检测更新
    bool auto_check_update;
};

bool sortRules(QString &p1, QString &p2);

#endif // RTDUINOCONFIG_H
