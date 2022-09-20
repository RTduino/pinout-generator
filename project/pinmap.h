#ifndef PINMAP_H
#define PINMAP_H
#include <QString>
#include <QList>

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
};

class Pinmaplist
{
public:
    QList<Pinmap *> Allpinlist;

    void fill_pinmap_arduino_and_name();
    void fill_pinmap_notes(Pinmap *pinmap);
    void update_pinmap_notes();
    void adjust_order_to_keep_arduino();
    void remove_item_by_arduinopin(QString &ardpin);
    QString change_item_by_arduinopin(QString &ardpin,QStringList &pinmap);
    QString add_pinmap_info_to_list(QStringList &pinmap);
    QString insert_item_back_arduinopin(QString &ardpin,QStringList &pinmap);
    int find_item_index_by_arduinopin(QString &ardpin);

};


#endif // PINMAP_H
