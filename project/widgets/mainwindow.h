#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <funcwidget.h>
#include <otherwidget.h>
#include <pinswidget.h>
#include <projwidget.h>
#include <rtduinoconfig.h>
#include <createfile.h>
#include <QSimpleUpdater.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initStackedWidget();
    QString getCountry();
private slots:
    void dealWidget();
    void updateProjectInfo();
    void on_pushButton_procreat_clicked();

private:
    Ui::MainWindow *ui;
    FuncWidget  *ui_func;
    OtherWidget *ui_other;
    PinsWidget  *ui_pins;
    ProjWidget  *ui_proj;
    RTduinoConfig *rtduino;
    CreateFile *cfile;
    QSimpleUpdater *updater;
};
#endif // MAINWINDOW_H
