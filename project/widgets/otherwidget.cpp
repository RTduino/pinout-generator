#include "otherwidget.h"
#include "ui_otherwidget.h"
#include <QKeyEvent>
#include <QSslSocket>
#include <QSettings>
#include <QDebug>

HotkeyLineEdit::HotkeyLineEdit(QWidget *parent)
    : QLineEdit(parent)
{
    this->setVisible(false);
}

HotkeyLineEdit::~HotkeyLineEdit()
{
}

void HotkeyLineEdit::keyPressEvent(QKeyEvent *event)
{
    //捕获输入的键， 除了A-Z以及Ctrl/Shift/Alt,其它都无视
    if (event->key() >= Qt::Key_A && event->key() <= Qt::Key_Z)
    {
        QChar strKey = (QChar)event->key();

        if (event->modifiers() == Qt::ControlModifier)
        {
            this->setText(QString("ctrl+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == Qt::ShiftModifier)
        {
            this->setText(QString("shift+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == Qt::AltModifier)
        {
            this->setText(QString("alt+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier))
        {
            this->setText(QString("ctrl+shift+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == (Qt::AltModifier | Qt::ControlModifier))
        {
            this->setText(QString("ctrl+alt+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == (Qt::ShiftModifier | Qt::AltModifier))
        {
            this->setText(QString("alt+shift+%0").arg(strKey).toLower());
        }
        else if (event->modifiers() == (Qt::ShiftModifier | Qt::ControlModifier | Qt::AltModifier))
        {
            this->setText(QString("ctrl+alt+shift+%0").arg(strKey).toLower());
        }
    }
}

OtherWidget::OtherWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OtherWidget)
{
    ui->setupUi(this);
    rtduino = RTduinoConfig::getInstance();

    ui->rtduino_git_url->setOpenExternalLinks(true);
    ui->rtduino_git_url->setText("<a style='color: green; text-decoration: none' "
                                 "href = https://github.com/RTduino/RTduino>"
                                 "https://github.com/RTduino/RTduino");

    ui->rtduino_gitee_url->setOpenExternalLinks(true);
    ui->rtduino_gitee_url->setText("<a style='color: green; text-decoration: none' "
                                   "href = https://gitee.com/rtduino/RTduino>"
                                   "https://gitee.com/rtduino/RTduino");

    ui->doc_git_url->setOpenExternalLinks(true);
    ui->doc_git_url->setText("<a style='color: green; text-decoration: none' "
                                 "href = https://rtduino.github.io/RTduino/#/zh/manual/adapt/bsp/stm32/stm32>"
                                 "https://rtduino.github.io/RTduino/#/zh/manual/adapt/bsp/stm32/stm32");

    ui->doc_gitee_url->setOpenExternalLinks(true);
    ui->doc_gitee_url->setText("<a style='color: green; text-decoration: none' "
                                   "href = https://rtduino.gitee.io/#/zh/manual/adapt/bsp/stm32/stm32>"
                                   "https://rtduino.gitee.io/#/zh/manual/adapt/bsp/stm32/stm32");

    ui->autor_liyony_url->setOpenExternalLinks(true);
    ui->autor_liyony_url->setText("<a style='color: green; text-decoration: none' "
                                  "href = https://github.com/liYony>"
                                  "https://github.com/liYony");

    ui->autor_meco_url->setOpenExternalLinks(true);
    ui->autor_meco_url->setText("<a style='color: green; text-decoration: none' "
                                "href = https://github.com/mysterywolf>"
                                "https://github.com/mysterywolf");
    // 是否支持OpenSSL
    support_openssl = QSslSocket::supportsSsl();
    if (support_openssl == false)
    {
        ui->checkBox_update->setChecked(false);
        ui->checkBox_update->setDisabled(true);
    }
}

OtherWidget::~OtherWidget()
{
    delete ui;
}

void OtherWidget::loadUi()
{
    qDebug() << qApp->applicationDirPath();
    QSettings setting(qApp->applicationDirPath() +"/Setting.ini", QSettings::IniFormat);               //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    QString addShortcut = setting.value("addShortcut").toString();          //获取addShortcut
    QString insertShortcut = setting.value("insertShortcut").toString();    //获取insertShortcut
    QString deleteShortcut = setting.value("deleteShortcut").toString();    //获取deleteShortcut
    QString changeShortcut = setting.value("changeShortcut").toString();    //获取changeShortcut
    QString autoUpdateCfg = setting.value("autoUpdate").toString();         //获取自动更新配置

    qDebug() << addShortcut << insertShortcut << deleteShortcut << changeShortcut << autoUpdateCfg;

    if(!addShortcut.isEmpty())
    {
        ui->checkBox_add->setChecked(true);
        ui->lineEdit_add->setVisible(true);
        ui->lineEdit_add->setText(addShortcut);
    }
    if(!insertShortcut.isEmpty())
    {
        ui->checkBox_insert->setChecked(true);
        ui->lineEdit_insert->setVisible(true);
        ui->lineEdit_insert->setText(insertShortcut);
    }
    if(!deleteShortcut.isEmpty())
    {
        ui->checkBox_delete->setChecked(true);
        ui->lineEdit_delete->setVisible(true);
        ui->lineEdit_delete->setText(deleteShortcut);
    }
    if(!changeShortcut.isEmpty())
    {
        ui->checkBox_change->setChecked(true);
        ui->lineEdit_change->setVisible(true);
        ui->lineEdit_change->setText(changeShortcut);
    }
    if(!autoUpdateCfg.isEmpty() && support_openssl == true)
    {
        ui->checkBox_update->setEnabled(true);
        ui->checkBox_update->setChecked(true);
    }
}

void OtherWidget::quitUi()
{
    QSettings setting(qApp->applicationDirPath() +"/Setting.ini", QSettings::IniFormat);               //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    if(ui->checkBox_add->isChecked())
    {
        setting.setValue("addShortcut", ui->lineEdit_add->text());
    }
    else
    {
        setting.setValue("addShortcut", "");
    }
    if(ui->checkBox_insert->isChecked())
    {
        setting.setValue("insertShortcut", ui->lineEdit_insert->text());
    }
    else
    {
        setting.setValue("insertShortcut", "");
    }
    if(ui->checkBox_delete->isChecked())
    {
        setting.setValue("deleteShortcut", ui->lineEdit_delete->text());
    }
    else
    {
        setting.setValue("deleteShortcut", "");
    }
    if(ui->checkBox_change->isChecked())
    {
        setting.setValue("changeShortcut", ui->lineEdit_change->text());
    }
    else
    {
        setting.setValue("changeShortcut", "");
    }
    if(ui->checkBox_update->isChecked() && support_openssl == true)
    {
        setting.setValue("autoUpdate", "Support");
    }
    else
    {
        setting.setValue("autoUpdate", "");
    }
}

void OtherWidget::refreshUi()
{

}

void OtherWidget::on_checkBox_delete_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->lineEdit_delete->setVisible(false);
    }
    else
    {
        ui->lineEdit_delete->setVisible(true);
    }
}


void OtherWidget::on_checkBox_change_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->lineEdit_change->setVisible(false);
    }
    else
    {
        ui->lineEdit_change->setVisible(true);
    }
}


void OtherWidget::on_checkBox_insert_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->lineEdit_insert->setVisible(false);
    }
    else
    {
        ui->lineEdit_insert->setVisible(true);
    }
}


void OtherWidget::on_checkBox_add_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->lineEdit_add->setVisible(false);
    }
    else
    {
        ui->lineEdit_add->setVisible(true);
    }
}

