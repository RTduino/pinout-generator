#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileInfo>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , funcPage(nullptr)
    , otherPage(nullptr)
    , pinsPage(nullptr)
    , projPage(nullptr)
{
    ui->setupUi(this);
    initStackedWidget();
    // 加载xmlfile文件
    //    setXmlfileName("123.rdpg");
    //    otherPage->loadUi();
    //    pinsPage->loadUi();
    //    funcPage->loadUi();
    //    projPage->loadUi();

    creat = new CreatFile;
    connect(projPage, SIGNAL(projSave()), this, SLOT(projWidgetSave()));
    connect(projPage, SIGNAL(projOpen(QString &)), this, SLOT(projWidgetOpen(QString &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateProUi()
{
    ui->label_mcu->setText(McuInfo::getInstance()->nowMcuSeries->mcuSeries);
    ui->label_board->setText(McuInfo::getInstance()->nowMcuSeries->mcuBoard);
    if(!projPage->getFileName().isEmpty())
        ui->label_proname->setText(projPage->getFileName() + ".rdpg");
    else
        ui->label_proname->setText("");

    if(!McuInfo::getInstance()->xmlFile->creatXmlFile())
    {
        qDebug() << "生成Xmlfile不成功";
        McuInfo::getInstance()->xmlFile->fileInfo = nullptr;
        return;
    }

    qDebug() << "生成Xmlfile成功";
}

void MainWindow::loadXmlfileToUi()
{
    McuInfo::getInstance()->nowMcuSeries->pinInfoClear();

    McuInfo::getInstance()->xmlFile->fileInfo = xmlfileName;

    if(!McuInfo::getInstance()->xmlFile->praseXmlFile())
    {
        qDebug() << "解析Xmlfile不成功";
        McuInfo::getInstance()->xmlFile->fileInfo = nullptr;
        return;
    }

    qDebug() << "解析Xmlfile成功";

    otherPage->loadUi();
    pinsPage->loadUi();
    funcPage->loadUi();
    projPage->loadUi();

    ui->label_mcu->setText(McuInfo::getInstance()->nowMcuSeries->mcuSeries);
    ui->label_board->setText(McuInfo::getInstance()->nowMcuSeries->mcuBoard);
    if(!projPage->getFileName().isEmpty())
        ui->label_proname->setText(projPage->getFileName() + ".rdpg");
    else
        ui->label_proname->setText("");

    // 解析成功直接跳到pin界面
    ui->pushButtonPins->setChecked(true);
    ui->stackedWidget->setCurrentWidget(pinsPage);
    pinsPage->loadUi();
}

void MainWindow::projWidgetSave()
{
    updateProUi();
}

void MainWindow::projWidgetOpen(QString &file)
{
    qDebug() << file;

    setXmlfileName(file);
    loadXmlfileToUi();
}

void MainWindow::on_pushButton_procreat_clicked()
{
    pinsPage->quitUi();
    funcPage->quitUi();
    projPage->quitUi();
    otherPage->quitUi();

    creat->setProjectPath(projPage->getFilePath());
    creat->creatPinoutDir();

    // 生成图片文件
    if(!McuInfo::getInstance()->nowMcuSeries->srcimagefile.isEmpty())
    {
        QFileInfo fileInfo(McuInfo::getInstance()->nowMcuSeries->srcimagefile);
        McuInfo::getInstance()->nowMcuSeries->aimimagefile = projPage->getFilePath() + "/arduino_pinout/" + fileInfo.fileName();
        QFile::rename(McuInfo::getInstance()->nowMcuSeries->srcimagefile, McuInfo::getInstance()->nowMcuSeries->aimimagefile);
    }
    else
    {
        McuInfo::getInstance()->nowMcuSeries->aimimagefile.clear();
    }

    updateProUi();

    creat->creatAllFiles();
    QMessageBox::StandardButton result = QMessageBox::question( this,"成功","代码生成成功，是否打开资源所在目录？");

    if(result == QMessageBox::No)
        return;

    QDesktopServices::openUrl(QUrl(projPage->getFilePath(),QUrl::TolerantMode));
}

// 初始化StackedWidget
void MainWindow::initStackedWidget(void)
{
    funcPage  = new FuncWidget;
    otherPage = new OtherWidget;
    pinsPage  = new PinsWidget;
    projPage  = new ProjWidget;

    ui->stackedWidget->addWidget(funcPage);
    ui->stackedWidget->addWidget(otherPage);
    ui->stackedWidget->addWidget(pinsPage);
    ui->stackedWidget->addWidget(projPage);
    // 默认显示工程页面
    ui->stackedWidget->setCurrentWidget(projPage);

    auto l = ui->toolwidget->children();
    foreach (auto it, l)
    {
        if(it->objectName().contains("pushButton"))
        {
            connect(static_cast<QPushButton *>(it),&QPushButton::clicked,this,&MainWindow::dealWidget);
        }
    }
}

// 处理页面切换
void MainWindow::dealWidget(void)
{
    auto btn_str = sender()->objectName();

    QWidget * oldPage = ui->stackedWidget->currentWidget();

    if("pushButtonFunc" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(funcPage);
    }
    else if("pushButtonProj" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(projPage);
    }
    else if("pushButtonOther" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(otherPage);
    }
    else if("pushButtonPins" == btn_str)
    {
        ui->stackedWidget->setCurrentWidget(pinsPage);
    }
    else
    {
        return;
    }

    QWidget * newPage = ui->stackedWidget->currentWidget();

    if(oldPage != newPage)
    {
        // 执行界面退出函数
        if(oldPage->objectName() == funcPage->objectName())
        {
            funcPage->quitUi();
        }
        if(oldPage->objectName() == projPage->objectName())
        {
            projPage->quitUi();
        }
        if(oldPage->objectName() == otherPage->objectName())
        {
            otherPage->quitUi();
        }
        if(oldPage->objectName() == pinsPage->objectName())
        {
            pinsPage->quitUi();
        }
        // 执行界面加载函数
        if(newPage->objectName() == funcPage->objectName())
        {
            funcPage->loadUi();
        }
        if(newPage->objectName() == projPage->objectName())
        {
            projPage->loadUi();
        }
        if(newPage->objectName() == otherPage->objectName())
        {
            otherPage->loadUi();
        }
        if(newPage->objectName() == pinsPage->objectName())
        {
            pinsPage->loadUi();
        }
    }
    else
    {
        // 执行界面刷新函数
        if(newPage->objectName() == funcPage->objectName())
        {
            funcPage->refreshUi();
        }
        if(newPage->objectName() == projPage->objectName())
        {
            projPage->refreshUi();
        }
        if(newPage->objectName() == otherPage->objectName())
        {
            otherPage->refreshUi();
        }
        if(newPage->objectName() == pinsPage->objectName())
        {
            pinsPage->refreshUi();
        }
    }
}

void MainWindow::setXmlfileName(QString file)
{
    xmlfileName = file;
}

