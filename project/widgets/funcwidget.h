#ifndef FUNCWIDGET_H
#define FUNCWIDGET_H

#include <QWidget>
#include <rtduinoconfig.h>

namespace Ui {
class FuncWidget;
}

class FuncWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FuncWidget(QWidget *parent = nullptr);
    ~FuncWidget();

    void loadUi();
    void refreshUi();
    void quitUi();

private slots:
    void on_checkBox_serial2_stateChanged(int arg1);

    void on_checkBox_serial3_stateChanged(int arg1);

    void on_checkBox_timer_stateChanged(int arg1);

    void on_checkBox_spi_stateChanged(int arg1);

private:
    Ui::FuncWidget *ui;
    RTduinoConfig *rtduino;
};

#endif // FUNCWIDGET_H
