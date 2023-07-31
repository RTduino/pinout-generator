#include "mcuinfo.h"

McuInfo *McuInfo::instance = nullptr;
McuInfo::McuInfo()
{
    nowMcuSeries = nullptr;
    xmlFile = new XmlFile;
}

bool McuInfo::mcuSeriesShow()
{
    if(mcuSerieslist.isEmpty())
        return false;

    for(int i = 0; i < mcuSerieslist.size(); i++) {
        boardMcuList << mcuSerieslist[i]->mcuSeries;
        qDebug() << mcuSerieslist[i]->mcuSeries;
    }

    return true;
}

bool McuInfo::mcuSeriesRegister(PinsMap *pinInfo, QString mcuSeries)
{
    pinInfo->mcuSeries = mcuSeries;

    mcuSerieslist.push_back(pinInfo);

    pinInfo->pinInfoConfigSet();
    return true;
}

bool McuInfo::mcuSeriesUnregister(QString mcuSeries)
{
    for(int i = 0; i < mcuSerieslist.size(); i++) {
        if(mcuSerieslist[i]->mcuSeries == mcuSeries)
        {
            mcuSerieslist.removeAt(i);
            return true;
        }
    }

    return false;
}

bool McuInfo::mcuSeriesNowSet(QString mcuSeries)
{
    for(int i = 0; i < mcuSerieslist.size(); i++) {
        if(mcuSerieslist[i]->mcuSeries == mcuSeries)
        {
            nowMcuSeries = mcuSerieslist[i];
            return true;
        }
    }

    nowMcuSeries = nullptr;
    return false;
}


