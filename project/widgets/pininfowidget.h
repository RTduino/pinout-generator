#ifndef PININFOWIDGET_H
#define PININFOWIDGET_H

#include <QWidget>
#include <rtduinoconfig.h>

namespace Ui {
class PinInfoWidget;
}

class PinInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PinInfoWidget(QWidget *parent = nullptr);
    ~PinInfoWidget();

    void initaddWidget(void);
    void initinsertWidget(pin_info_t sel_pin);
    void initchangeWidget(pin_info_t sel_pin);
    void setVisibleUi(QString func);
private slots:
    void on_comboBox_devfunc_currentIndexChanged(const QString &arg1);

    void on_pushButton_save_clicked();

    void on_comboBox_devchnl_currentIndexChanged(const QString &arg1);

signals:
    void refreshTreeWidget(void);

private:
    Ui::PinInfoWidget *ui;
    RTduinoConfig *rtduino;

    QString ui_type;
    QString select_pin;
};

#endif // PININFOWIDGET_H
