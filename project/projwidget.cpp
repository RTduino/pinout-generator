#include "projwidget.h"
#include "ui_projwidget.h"
#include "mcuinfo.h"
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>

ProjWidget::ProjWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjWidget)
{
    ui->setupUi(this);
    ui->comboBox_mcuserias->addItems(McuInfo::getInstance()->boardMcuList);
}

ProjWidget::~ProjWidget()
{
    delete ui;
}

void ProjWidget::loadUi() // Refresh
{
    ui->comboBox_mcuserias->setCurrentText(McuInfo::getInstance()->nowMcuSeries->mcuSeries);
    ui->lineEdit_boardname->setText(McuInfo::getInstance()->nowMcuSeries->mcuBoard);
    if(!McuInfo::getInstance()->xmlFile->fileInfo.isEmpty())
    {
        xmlfile = McuInfo::getInstance()->xmlFile->fileInfo;
        QFileInfo fileInfo(xmlfile);
        ui->lineEdit_propath->setText(fileInfo.path());
        ui->lineEdit_proname->setText(fileInfo.baseName());
        xmlfilePath = fileInfo.path();
        xmlfileName = fileInfo.baseName();
    }
    else
    {
        ui->lineEdit_propath->clear();
        ui->lineEdit_proname->clear();
        xmlfilePath = "";
        xmlfileName = "";
    }

    ui->lineEdit_image->setText(McuInfo::getInstance()->nowMcuSeries->srcimagefile);
}

void ProjWidget::refreshUi()
{
    qDebug() << this->objectName() << "refresh";
}

void ProjWidget::quitUi()
{
    McuInfo::getInstance()->nowMcuSeries->mcuSeries = ui->comboBox_mcuserias->currentText();
    McuInfo::getInstance()->nowMcuSeries->mcuBoard = ui->lineEdit_boardname->text();
}

QString ProjWidget::getFileName()
{
    return xmlfileName;
}

QString ProjWidget::getFilePath()
{
    return xmlfilePath;
}

void ProjWidget::on_comboBox_mcuserias_currentIndexChanged(const QString &arg1)
{
    McuInfo::getInstance()->mcuSeriesNowSet(arg1);
}


void ProjWidget::on_pushButton_save_clicked()
{
    QString tmpXmlfile;

    xmlfilePath = ui->lineEdit_propath->text();
    xmlfileName = ui->lineEdit_proname->text();

    if(ui->lineEdit_boardname->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请填写你的板子名称!\n例如:\n <STM32F411 Nucleo>\n <STM32L475潘多拉>");
        return;
    }
    if(xmlfilePath.isEmpty())
    {
        QMessageBox::critical(this,"错误","请指定项目保存路径！");
        return;
    }
    if(xmlfileName.isEmpty())
    {
        QMessageBox::critical(this,"错误","请指定项目名称！");
        return;
    }

    tmpXmlfile = xmlfilePath + "/" + xmlfileName +".rdpg";

    qDebug() << xmlfile << tmpXmlfile;

    if(xmlfile != tmpXmlfile) // 更新文件路劲
    {
        if(xmlfile != nullptr)
        {
            QFile::rename(xmlfile, tmpXmlfile);
        }

        McuInfo::getInstance()->xmlFile->fileInfo = tmpXmlfile;
        xmlfile = tmpXmlfile;
    }

    McuInfo::getInstance()->nowMcuSeries->mcuSeries = ui->comboBox_mcuserias->currentText();
    McuInfo::getInstance()->nowMcuSeries->mcuBoard = ui->lineEdit_boardname->text();

    emit projSave();
}


void ProjWidget::on_pushButton_path_clicked()
{
    QString path = QFileDialog::getExistingDirectory( this, "选择项目保存路径","./");

    if(!path.isEmpty())
    {
        ui->lineEdit_propath->setText(path);
    }
}


void ProjWidget::on_pushButton_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开工程文件"), "./", tr("RTduino(*.rdpg)"));

    if(fileName.isEmpty())
        return;

    emit projOpen(fileName);
}


void ProjWidget::on_pushButton_image_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), "./", tr("Image(*.*)"));

    if(fileName.isEmpty())
    {
        ui->lineEdit_image->clear();
        McuInfo::getInstance()->nowMcuSeries->srcimagefile.clear();
        return;
    }

    QFileInfo fileInfo(fileName);

    QString fileChange = fileInfo.path() + "/" + McuInfo::getInstance()->nowMcuSeries->mcuBoard.toLower() + "-pinout." + fileInfo.suffix();
    QFile::copy(fileName, fileChange);
    fileName = fileChange;

    ui->lineEdit_image->setText(fileName);

    McuInfo::getInstance()->nowMcuSeries->srcimagefile = fileName;
}

