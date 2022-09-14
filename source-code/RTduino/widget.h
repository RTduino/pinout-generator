#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QList>
#include "Pinmap.h"
#include <QTextStream>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QStringList>
#include <QRegularExpression>
#include "add.h"
#include <QSettings>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

#define DEFALUT_BSP_PATH                        "C:/Users/94184/Desktop/RTtst"
#define ARDUINO_PINOUT_DIR                  rttBspdirpath+"/applications/arduino_pinout"
#define ARDUINO_PINOUT_CFILE              rttBspdirpath+"/applications/arduino_pinout/pins_arduino.c"
#define ARDUINO_PINOUT_HFILE              rttBspdirpath+"/applications/arduino_pinout/pins_arduino.h"
#define ARDUINO_PINOUT_SCONS            rttBspdirpath+"/applications/arduino_pinout/Sconscript"
#define ARDUINO_PINOUT_KCONFIG        rttBspdirpath+"/applications/arduino_pinout/Kconfig"

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void insert_item_table(QStringList &pinlist);

    void load_data_from_dir();

    void load_data_to_dir();

    void get_some_info();

    void write_data_to_cfile();

    void write_data_to_hfile();

    void write_data_to_kconfig();

    void write_data_to_scons();

    void update_pinmaps();

    void add_one_item(Pinmap &pin);

    void refresh_to_widget();

    void display_add_edit(bool name,bool channel);

    void prase_pin_item_instring(QString &pinstr);

    void set_io_function(Pinmap *pinmap);

    void clear_table_data();

    void update_table_data();

    void remove_item_table(QStringList &pinlist);

    void change_item_table(QStringList &pinlist);

    void add_item_table();

    void serials_cbox_data();

    void ledpin_cbox_data();

private slots:

    void on_dirbtn_clicked();

    void on_importbtn_clicked();

    void on_exportbtn_clicked();

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void recive_add_data(QStringList &);

    void on_menu_envent();
    void on_addbtn_clicked();

    void on_clearbtn_clicked();

    void on_project_linkActivated(const QString &link);

    void on_autor_linkActivated(const QString &link);

private:
    Ui::Widget *ui;

    add *addui;

    QString rttBspdirpath;

    QString tmprttBspdirpath;

    QString arduino_file_path;

    SomeInfo sinfo;

    QString changeardpin;

    QString insertardpin;

    QList<Pinmap *> pinmaplist;
};
#endif // WIDGET_H
