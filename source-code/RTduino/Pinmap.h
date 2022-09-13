#ifndef PINMAP_H
#define PINMAP_H
#include <QString>

class Pinmap
{
public:
    Pinmap();
    QString arduino_pin;
    QString rtthread_pin;
    QString io_function;
    QString io_name;
    QString io_channel;
};

class SomeInfo
{
public:
    QString year;
    QString month;
    QString day;
    QString fcpu;
    QString ledpin;
    QString autor;
};

#endif // PINMAP_H
