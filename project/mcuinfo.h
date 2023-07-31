#ifndef MCUINFO_H
#define MCUINFO_H

#include "xmlfile.h"
#include "pinsmap.h"
#include "stm32pinsmap.h"

class McuInfo
{
public:

    McuInfo();

    static McuInfo *instance;
    static McuInfo *getInstance() {
        if(nullptr == instance)
        {
            instance = new McuInfo();
        }
        return instance;
    }

    bool mcuSeriesShow();
    bool mcuSeriesRegister(PinsMap *pinInfo, QString mcuSeries);
    bool mcuSeriesUnregister(QString mcuSeries);
    bool mcuSeriesNowSet(QString mcuSeries);

    QStringList boardMcuList;

    QVector<PinsMap *> mcuSerieslist; // 所有支持的
    PinsMap *nowMcuSeries;
    class XmlFile *xmlFile;
};

#endif // MCUINFO_H
