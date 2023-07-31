#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <funcwidget.h>
#include <otherwidget.h>
#include <pinswidget.h>
#include <projwidget.h>
#include "mcuinfo.h"
#include "xmlfile.h"
#include "creatfile.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setXmlfileName(QString file);
    void initStackedWidget();
    void updateProUi();
    void loadXmlfileToUi();
public slots:
    void dealWidget();
    void projWidgetSave();
    void projWidgetOpen(QString &file);
private slots:
    void on_pushButton_procreat_clicked();

private:
    Ui::MainWindow *ui;
    QString xmlfileName;
    FuncWidget  *funcPage;
    OtherWidget *otherPage;
    PinsWidget  *pinsPage;
    ProjWidget  *projPage;
    CreatFile   *creat;
};
#endif // MAINWINDOW_H
