#ifndef PINSWIDGET_H
#define PINSWIDGET_H

#include <QWidget>
#include <rtduinoconfig.h>
#include <QSettings>
#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <pininfowidget.h>
#include <QTreeWidgetItem>

namespace Ui {
class PinsWidget;
}

class PinsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PinsWidget(QWidget *parent = nullptr);
    ~PinsWidget();

    void treeWidgetAddItem(pin_info_t &info);
    void treeWidgetRefresh();
    void updateShortCut();

    void loadUi();
    void refreshUi();
    void quitUi();

public slots:
    void on_ShortCut();
    void menu_item_change_event();

private slots:
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void treeWidget_refresh();

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    void bindShortCutAction(void);
    Ui::PinsWidget *ui;
    RTduinoConfig *rtduino;

    QAction *action_add;
    QAction *action_insert;
    QAction *action_change;
    QAction *action_remove;

    QString select_pin;

    PinInfoWidget *ui_pininfo;
};

#endif // PINSWIDGET_H
