#include "projwidget.h"
#include "ui_projwidget.h"

ProjWidget::ProjWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjWidget)
{
    ui->setupUi(this);
    rtduino = RTduinoConfig::getInstance();
    rtduino->getAllMcuSeries(all_mcu_series);
    loadUi();
}

ProjWidget::~ProjWidget()
{
    delete ui;
}

void ProjWidget::loadUi()
{
    ui->comboBox_mcuserias->clear();
    ui->comboBox_mcuserias->addItems(all_mcu_series);
    if (all_mcu_series.contains(rtduino->getProjectInfo()->pinfo_series))
    {
        ui->comboBox_mcuserias->setCurrentText(rtduino->getProjectInfo()->pinfo_series);
        rtduino->setNowMcuSeries(rtduino->getProjectInfo()->pinfo_series);
    }
    else
    {
        rtduino->getProjectInfo()->pinfo_series = ui->comboBox_mcuserias->currentText();
        ui->comboBox_mcuserias->setCurrentText(rtduino->getProjectInfo()->pinfo_series);
        rtduino->setNowMcuSeries(rtduino->getProjectInfo()->pinfo_series);
    }

    ui->comboBox_childserias->clear();
    ui->comboBox_childserias->addItems(rtduino->getNowMcuSeries().childseries);
    if (rtduino->getNowMcuSeries().childseries.contains(rtduino->getProjectInfo()->child_series))
    {
        ui->comboBox_childserias->setCurrentText(rtduino->getProjectInfo()->child_series);
    }
    else
    {
        rtduino->getProjectInfo()->child_series = ui->comboBox_childserias->currentText();
        ui->comboBox_childserias->setCurrentText(rtduino->getProjectInfo()->child_series);
    }

    if (!rtduino->getProjectInfo()->board_name.isEmpty())
    {
        ui->lineEdit_boardname->setText(rtduino->getProjectInfo()->board_name);
    }
    else
    {
        rtduino->getProjectInfo()->board_name = "board_default";
        ui->lineEdit_boardname->setText(rtduino->getProjectInfo()->board_name);
    }

    if (!rtduino->getProjectInfo()->project_name.isEmpty())
    {
        ui->lineEdit_proname->setText(rtduino->getProjectInfo()->project_name);
    }
    else
    {
        rtduino->getProjectInfo()->project_name = "project_default";
        ui->lineEdit_proname->setText(rtduino->getProjectInfo()->project_name);
    }

    if (!rtduino->getProjectInfo()->project_path.isEmpty())
    {
        ui->lineEdit_propath->setText(rtduino->getProjectInfo()->project_path);
    }
    else
    {
        QDir tmpdir("./projects");
        QString default_path = tmpdir.absolutePath();
        rtduino->getProjectInfo()->project_path = default_path;
        ui->lineEdit_propath->setText(rtduino->getProjectInfo()->project_path);
    }
    if (!rtduino->getProjectInfo()->pinmap_image.isEmpty())
    {
        ui->lineEdit_image->setText(rtduino->getProjectInfo()->pinmap_image);
    }
}

void ProjWidget::refreshUi()
{

}

void ProjWidget::quitUi()
{
    //    rtduino->getProjectInfo()->pinfo_series = ui->comboBox_mcuserias->currentText();
    //    rtduino->getProjectInfo()->board_name = ui->lineEdit_boardname->text();
    //    rtduino->getProjectInfo()->project_name = ui->lineEdit_proname->text();
    //    rtduino->getProjectInfo()->project_path = ui->lineEdit_propath->text();
    //    rtduino->getProjectInfo()->pinmap_image = ui->lineEdit_image->text();
}

bool ProjWidget::checkConfigInfo()
{
    if(ui->lineEdit_boardname->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请填写你的板子名称!\n例如:\n <STM32F411 Nucleo>\n <STM32L475 Pandora>");
        return false;
    }
    if(ui->lineEdit_propath->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请指定项目保存路径！");
        return false;
    }
    QDir dir(ui->lineEdit_propath->text());
    if(!dir.exists())
    {
        QMessageBox::StandardButton result = QMessageBox::question( this,"文件路径不存在！","是否需要创建<"
                                                                    +ui->lineEdit_propath->text()+ ">路径？");

        if(result == QMessageBox::No)
            return false;

        dir.mkpath(ui->lineEdit_propath->text());//创建多级目录
    }
    if(ui->lineEdit_proname->text().isEmpty())
    {
        QMessageBox::critical(this,"错误","请指定工程名称！");
        return false;
    }

    return true;
}

void ProjWidget::on_pushButton_save_clicked()
{
    if (!checkConfigInfo())
        return;
    if (rtduino->getProjectInfo()->pinfo_series != ui->comboBox_mcuserias->currentText())
    {
        rtduino->clearPinInfoList();
        rtduino->clearFunctionInfo();
        rtduino->clearProjectInfo();
    }
    rtduino->getProjectInfo()->child_series = ui->comboBox_childserias->currentText();
    rtduino->getProjectInfo()->pinfo_series = ui->comboBox_mcuserias->currentText();
    rtduino->getProjectInfo()->board_name = ui->lineEdit_boardname->text();
    rtduino->getProjectInfo()->project_name = ui->lineEdit_proname->text();
    rtduino->getProjectInfo()->project_path = ui->lineEdit_propath->text();
    rtduino->getProjectInfo()->pinmap_image = ui->lineEdit_image->text();

    rtduino->setNowMcuSeries(rtduino->getProjectInfo()->pinfo_series);

    emit updateProject();
}


void ProjWidget::on_pushButton_image_clicked()
{
    if (!checkConfigInfo())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("打开图片"), "./", tr("Image(*.*)"));

    if(fileName.isEmpty())
    {
        ui->lineEdit_image->clear();
        rtduino->getProjectInfo()->pinmap_image.clear();
        return;
    }

    //    QFileInfo fileInfo(fileName);
    //    QString fileChange = ui->lineEdit_propath->text() + "/" + ui->lineEdit_boardname->text().toLower()
    //            + "-pinout." + fileInfo.suffix();
    //    QFile::copy(fileName, fileChange);
    //    fileName = fileChange;

    ui->lineEdit_image->setText(fileName);
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

    rtduino->clearPinInfoList();
    rtduino->clearFunctionInfo();
    rtduino->clearProjectInfo();

    rtduino->parsePinInfoJson(fileName);    // 解析文件
    QFileInfo fileInfo(fileName);           // 修改一下文件所在路径
    RTduinoConfig::getInstance()->getProjectInfo()->project_path = fileInfo.path();
    RTduinoConfig::getInstance()->getProjectInfo()->project_name = fileInfo.baseName();

    loadUi();                               // 将解析出来的文件加载到UI上

    emit updateProject();
}


void ProjWidget::on_comboBox_mcuserias_currentIndexChanged(const QString &arg1)
{
    ui->comboBox_childserias->clear();
    if (!arg1.isEmpty())
    {
        ui->comboBox_childserias->addItems(rtduino->getMcuSeries(arg1).childseries);
    }
}
