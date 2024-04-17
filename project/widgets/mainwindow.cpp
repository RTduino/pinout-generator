#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDir>
#include <QDebug>
#include <QSslSocket>
#include <QDesktopServices>
#include <version.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    rtduino = RTduinoConfig::getInstance();
    /* QSimpleUpdater is single-instance */
    qDebug()<<"QSslSocket:"<<QSslSocket::sslLibraryBuildVersionString();
    qDebug() << "OpenSSL support:" << QSslSocket::supportsSsl();
    if (QSslSocket::supportsSsl() == true)
    {
        updater = QSimpleUpdater::getInstance();

        /* Config for updates */
        updater->setModuleVersion(DEFS_URL, get_version_string());
        updater->setNotifyOnFinish(DEFS_URL, false);
        updater->setNotifyOnUpdate(DEFS_URL, true);
        updater->setUseCustomAppcast(DEFS_URL, false);
        updater->setDownloaderEnabled(DEFS_URL, true);
        updater->setMandatoryUpdate(DEFS_URL, false);

        /* Check for updates */
        updater->checkForUpdates(DEFS_URL);
    }
    cfile = new CreateFile;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initStackedWidget()
{
    ui_func  = new FuncWidget;
    ui_other = new OtherWidget;
    ui_pins  = new PinsWidget;
    ui_proj  = new ProjWidget;

    ui->stackedWidget->addWidget(ui_func);
    ui->stackedWidget->addWidget(ui_other);
    ui->stackedWidget->addWidget(ui_pins);
    ui->stackedWidget->addWidget(ui_proj);
    // 默认显示工程页面
    ui->stackedWidget->setCurrentWidget(ui_proj);
    connect(ui_proj, SIGNAL(updateProject()), this, SLOT(updateProjectInfo()));
    auto l = ui->toolwidget->children();
    foreach (auto it, l)
    {
        if(it->objectName().contains("pushButton"))
        {
            connect(static_cast<QPushButton *>(it),&QPushButton::clicked,this,&MainWindow::dealWidget);
        }
    }

    // 初始化一下默认工程页面
    ui->label_mcu->setText(rtduino->getProjectInfo()->pinfo_series);
    ui->label_childmcu->setText(rtduino->getProjectInfo()->child_series);
    ui->label_board->setText(rtduino->getProjectInfo()->board_name);
    if(!rtduino->getProjectInfo()->project_name.isEmpty())
        ui->label_proname->setText(rtduino->getProjectInfo()->project_name + ".rdpg");
    else
        ui->label_proname->setText("");
}

void MainWindow::dealWidget()
{
    auto btn_str = sender()->objectName();

    QWidget * oldPage = ui->stackedWidget->currentWidget();

    if("pushButtonFunc" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(ui_func);
    }
    else if("pushButtonProj" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(ui_proj);
    }
    else if("pushButtonOther" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(ui_other);
    }
    else if("pushButtonPins" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(ui_pins);
    }
    else
    {
        return;
    }

    QWidget * newPage = ui->stackedWidget->currentWidget();

    if (oldPage != newPage)
    {
        // 执行界面退出函数
        if(oldPage->objectName() == ui_func->objectName())
        {
            ui_func->quitUi();
        }
        if(oldPage->objectName() == ui_proj->objectName())
        {
            ui_proj->quitUi();
        }
        if(oldPage->objectName() == ui_other->objectName())
        {
            ui_other->quitUi();
        }
        if(oldPage->objectName() == ui_pins->objectName())
        {
            ui_pins->quitUi();
        }
        // 执行界面加载函数
        if(newPage->objectName() == ui_func->objectName())
        {
            ui_func->loadUi();
        }
        if(newPage->objectName() == ui_proj->objectName())
        {
            ui_proj->loadUi();
        }
        if(newPage->objectName() == ui_other->objectName())
        {
            ui_other->loadUi();
        }
        if(newPage->objectName() == ui_pins->objectName())
        {
            ui_pins->loadUi();
        }
    }
    else
    {
        // 执行界面刷新函数
        if(newPage->objectName() == ui_func->objectName())
        {
            ui_func->refreshUi();
        }
        if(newPage->objectName() == ui_proj->objectName())
        {
            ui_proj->refreshUi();
        }
        if(newPage->objectName() == ui_other->objectName())
        {
            ui_other->refreshUi();
        }
        if(newPage->objectName() == ui_pins->objectName())
        {
            ui_pins->refreshUi();
        }
    }
}

void MainWindow::updateProjectInfo()
{
    ui->label_mcu->setText(rtduino->getProjectInfo()->pinfo_series);
    ui->label_childmcu->setText(rtduino->getProjectInfo()->child_series);
    ui->label_board->setText(rtduino->getProjectInfo()->board_name);
    if(!rtduino->getProjectInfo()->project_name.isEmpty())
        ui->label_proname->setText(rtduino->getProjectInfo()->project_name + ".rdpg");
    else
        ui->label_proname->setText("");
}

void MainWindow::on_pushButton_procreat_clicked()
{
    cfile->creatPinoutDir();
    cfile->creatImage();
    cfile->createSConscript();
    cfile->creatPinoutCFile();
    cfile->creatPinoutHFile();
    cfile->creatMainCppFile();
    cfile->creatKconfigFile();
    cfile->creatReadmeFile();
    rtduino->createPinInfoJson(rtduino->getProjectInfo()->project_path + "/" +
                               rtduino->getProjectInfo()->project_name + ".rdpg");
    QMessageBox::StandardButton result = QMessageBox::question( this,"创建成功","代码生成成功，是否打开资源所在目录？");

    if(result == QMessageBox::No)
        return;

    QDesktopServices::openUrl(QUrl(rtduino->getProjectInfo()->project_path, QUrl::TolerantMode));
}

