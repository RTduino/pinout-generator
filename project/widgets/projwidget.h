#ifndef PROJWIDGET_H
#define PROJWIDGET_H

#include <QWidget>
#include <rtduinoconfig.h>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class ProjWidget;
}

class ProjWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjWidget(QWidget *parent = nullptr);
    ~ProjWidget();

    void loadUi();
    void refreshUi();
    void quitUi();

signals:
    void updateProject(void);

private:
    bool checkConfigInfo(void);

private slots:
    void on_pushButton_save_clicked();

    void on_pushButton_image_clicked();

    void on_pushButton_path_clicked();

    void on_pushButton_open_clicked();

    void on_comboBox_mcuserias_currentIndexChanged(const QString &arg1);

private:
    Ui::ProjWidget *ui;
    RTduinoConfig *rtduino;

    QStringList all_mcu_series;
};

#endif // PROJWIDGET_H
