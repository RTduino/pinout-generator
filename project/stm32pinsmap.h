#ifndef STM32PINSMAP_H
#define STM32PINSMAP_H

#include "pinsmap.h"

class Stm32PinsMap : public PinsMap
{
public:
    virtual void pinInfoConfigSet();
    virtual QString getPinName(QStringList pin)  {
            if(pin.size() < 2)
                return "NONE";
            else
                return "P" + pin.at(0) + pin.at(1);
        };
    virtual QString creatPwmToSpiCode(struct spiPinInfo spipin);
    virtual QString countNumByRttPin(QString rtt);
};

#endif // STM32PINSMAP_H
