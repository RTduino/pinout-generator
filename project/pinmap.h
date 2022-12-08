#ifndef PINMAP_H
#define PINMAP_H
#include <QString>
#include <QList>
#include <QStringList>

/* Contents of each pin */
class Pinmap
{
public:
    QString rtthread_pin;
    QString io_name;
    QString io_channel;
    QString io_function;

    QString arduino_pin;
    QString io_notes;
    QString pin_func;
};

class Pindevice
{
public:
    QString device_name;
    QStringList device_channels;
};

class Pinmaplist
{
public:
    QList<Pinmap *> Allpinlist;

    QList<Pindevice *> uartpinlist;
    QList<Pindevice *> pwmpinlist;
    QList<Pindevice *> adcpinlist;
    QList<Pindevice *> dacpinlist;
    QList<Pindevice *> spipinlist;
    QList<Pindevice *> i2cpinlist;

    void fill_pinmap_arduino_and_name();
    void fill_pinmap_notes(Pinmap *pinmap);
    void update_pinmap_notes();
    void adjust_order_to_keep_arduino();
    void remove_item_by_arduinopin(QString &ardpin);
    QString change_item_by_arduinopin(QString &ardpin,QStringList &pinmap);
    QString add_pinmap_info_to_list(QStringList &pinmap);
    QString insert_item_back_arduinopin(QString &ardpin,QStringList &pinmap);
    int find_item_index_by_arduinopin(QString &ardpin);
    bool check_device_isexist(QString name,QList<Pindevice *> devlist);
    bool check_device_ch_isexist(QString channel,QStringList channellist);
    void update_uartpinlist();
    void update_pwmpinlist();
    void update_adcpinlist();
    void update_dacpinlist();
    void update_spipinlist();
    void update_i2cpinlist();
    int count_num_by_rttpin(QString rtt);

};


#endif // PINMAP_H
