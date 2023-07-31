#ifndef PININFOWIDGET_H
#define PININFOWIDGET_H

#include <QWidget>
#include <QString>
#include "pinsmap.h"

namespace Ui {
class PinInfoWidget;
}

class PinInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PinInfoWidget(QWidget *parent = nullptr);
    ~PinInfoWidget();

    void initInfoUi(QString type);
    void uiRefresh(QString devFunc);
    void uiChangeRefresh();

signals:

    void refreshTreeWidget(void);

private slots:
    void on_comboBox_devfunc_currentIndexChanged(const QString &arg1);

    void on_pushButton_save_clicked();

    void on_comboBox_usefunc_currentIndexChanged(const QString &arg1);

private:
    Ui::PinInfoWidget *ui;
    QString typeInfo;
};

#endif // PININFOWIDGET_H
