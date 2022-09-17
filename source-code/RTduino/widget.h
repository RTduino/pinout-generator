#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "pininfoui.h"
#include "pinmap.h"
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
#include <QSettings>
#include <QMenu>
#include <QAction>
#include <QDateTime>
#include <QDesktopServices>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class DateInfo
{
public:
    QString year;
    QString month;
    QString day;
    QString autor;
};

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    PininfoUI *pinUI;
    DateInfo *dateinfo;
    Pinmaplist pinmaplist;
    QString insertpin,changepin,removepin;
    QString rttBspdirpath;

    void menu_add_event();
    void menu_change_event(QStringList &list);
    void menu_insert_event(QStringList &list);
    void menu_remove_event(QStringList &list);
    void menu_refresh_event();
    void menu_clear_event();

    void all_ui_component_refresh();
    void table_add_item(Pinmap &pin);
    void table_all_items_refresh();
    void serials_add_items();
    void led_ss_pin_add_items();

    void load_data_from_dir();
    void prase_pin_item_from_string(QString &pinline);
    void pin_set_io_function(Pinmap *pinmap);

    void  load_data_to_dir();
    void write_data_to_cfile();
    void write_data_to_hfile();
    void write_data_to_kconfig();
    void write_data_to_scons();
    void get_date_info();

private slots:
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void on_menu_item_change_event();
    void on_addbtn_clicked();
    void on_recive_pininfo_data(QStringList &pininfo);

    void on_dirbtn_clicked();

    void on_importbtn_clicked();

    void on_exportbtn_clicked();

    void on_refreashbtn_clicked();

    void on_clearbtn_clicked();

    void on_project_linkActivated(const QString &link);

    void on_autor_linkActivated(const QString &link);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
