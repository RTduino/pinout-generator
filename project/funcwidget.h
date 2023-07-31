#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H

#include <QWidget>
#include "mcuinfo.h"

namespace Ui {
class FuncWidget;
}

class FuncWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuncWidget(QWidget *parent = nullptr);
    ~FuncWidget();

//    void uiRefresh();
//    void saveUiData();

//    void onloadUi();

//    void initUi();
    // page生命周期函数
    void loadUi();
    void refreshUi();
    void quitUi();
private slots:
    void on_checkBox_serial2_stateChanged(int arg1);

    void on_checkBox_serial3_stateChanged(int arg1);

    void on_checkBox_timer_stateChanged(int arg1);

    void on_checkBox_useNumber_stateChanged(int arg1);

    void on_checkBox_spi_stateChanged(int arg1);

private:
    Ui::FuncWidget *ui;
};

#endif // FUNCWIDGET_H
