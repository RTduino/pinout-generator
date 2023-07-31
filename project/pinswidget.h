#ifndef PINSWIDGET_H
#define PINSWIDGET_H

#include <QTreeWidget>
#include <QWidget>
#include <pininfowidget.h>

namespace Ui {
class PinsWidget;
}

class PinsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PinsWidget(QWidget *parent = nullptr);
    ~PinsWidget();

    void treeWidgetAddItem(struct pinInfo &info);
    void treeWidgetRefresh();

    void ShortCut();
    // page生命周期函数
    void loadUi();
    void refreshUi();
    void quitUi();
public slots:
    void on_ShortCut();
    void on_treeWidget_refresh();
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void on_menu_item_change_event();

private slots:
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

private:
    Ui::PinsWidget *ui;

    class PinInfoWidget *uiPinInfo;

    QAction *actionAdd;
    QAction *actionInsert;
    QAction *actionChange;
    QAction *actionRemove;
};

#endif // PINSWIDGET_H
