#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    pinUI = new PininfoUI;
    dateinfo = new DateInfo;
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(pinUI,SIGNAL(send_pininfo_data(QStringList &)),this,SLOT(on_recive_pininfo_data(QStringList &)));
    ui->autor->setText(tr("<a href = 'https://github.com/liYony'>https://github.com/liYony</a>"));
    ui->project->setText(tr("<a href = 'https://github.com/RTduino/pinout-generator'>https://github.com/RTduino/pinout-generator</a>"));
    pinUI->setWindowModality(Qt::ApplicationModal);//设置界面不可点击
    ui->addbtn->setShortcut(tr("a"));
    ui->delbtn->setShortcut(tr("d"));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_dirbtn_clicked()
{
    QDir rttBspdir;
    QString tmprttBspdirpath;
    QSettings setting("./Setting.ini", QSettings::IniFormat);  //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    QString lastPath = setting.value("LastFilePath").toString();  //获取上次的打开路径

    tmprttBspdirpath = QFileDialog::getExistingDirectory( this, "Choose RT-Thread BSP Directory",lastPath);
    ui->diredit->setText(tmprttBspdirpath);

    /* 判断路径格式是否正确 */
    tmprttBspdirpath = ui->diredit->text();
    if(tmprttBspdirpath.isEmpty())
    {
        QMessageBox::critical(this,"错误","你并没有添加一个路径！");
        return;
    }
    if(!rttBspdir.exists(tmprttBspdirpath))
    {
        QString errinfo = tmprttBspdirpath + " 路径不存在！";
        QMessageBox::critical(this,"错误",errinfo);
        return;
    }

    rttBspdirpath = QDir::fromNativeSeparators(ui->diredit->text());

    if(!rttBspdirpath.isEmpty())
    {
        setting.setValue("LastFilePath",rttBspdirpath);  //记录路径到QSetting中保存
    }

    all_ui_reset();
    all_ui_component_refresh();
    if(!rttBspdir.exists(rttBspdirpath+"/applications/arduino_pinout"))
        return;
    QMessageBox::StandardButton result = QMessageBox::question( this,"成功","该路径已经存在arduino_pinout文件夹，是否导入？");

    if(result == QMessageBox::No)
        return;
    on_importbtn_clicked();
}

void Widget::on_importbtn_clicked()
{
    load_data_from_dir();
    all_ui_component_refresh();
}

void Widget::on_exportbtn_clicked()
{
    QDir rttBspdir;
    QString tmprttBspdirpath;
    QSettings setting("./Setting.ini", QSettings::IniFormat);  //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    /* 判断路径格式是否正确 */
    tmprttBspdirpath = ui->diredit->text();
    if(tmprttBspdirpath.isEmpty())
    {
        QMessageBox::critical(this,"错误","你并没有添加一个路径！");
        return;
    }
    if(!rttBspdir.exists(tmprttBspdirpath))
    {
        QString errinfo = tmprttBspdirpath + " 路径不存在！";
        QMessageBox::critical(this,"错误",errinfo);
        return;
    }

    rttBspdirpath = QDir::fromNativeSeparators(ui->diredit->text());

    if(!rttBspdirpath.isEmpty())
    {
        setting.setValue("LastFilePath",rttBspdirpath);  //记录路径到QSetting中保存
    }
    all_ui_component_refresh();
    pinmaplist.update_pinmap_notes();
    pinmaplist.update_uartpinlist();
    pinmaplist.update_pwmpinlist();
    pinmaplist.update_adcpinlist();
    pinmaplist.update_dacpinlist();
    pinmaplist.update_spipinlist();
    pinmaplist.update_i2cpinlist();
    load_data_to_dir();
}

void Widget::on_refreashbtn_clicked()
{
    menu_refresh_event();
}

void Widget::on_addbtn_clicked()
{
    menu_add_event();
    pinUI->show();
}

void Widget::on_clearbtn_clicked()
{
    menu_clear_event();
}

void Widget::on_project_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link,QUrl::TolerantMode));
}

void Widget::on_autor_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link,QUrl::TolerantMode));
}

void Widget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QStringList pininfo;
    QRegularExpression tablereg("(.*) - (.*)");
    QString devname = tablereg.match(item->text(3)).captured(1);
    QString pinfunc = tablereg.match(item->text(3)).captured(2);
    if(devname.isEmpty() && pinfunc.isEmpty())
        devname = item->text(3);
    pininfo << item->text(0) << item->text(1) << item->text(2) << devname << item->text(4) << pinfunc << "chg";
    qDebug() << pininfo;
    menu_change_event(pininfo);
    pinUI->show();
}

void Widget::on_ledbox_activated(const QString &arg1)
{
    code_add_items();
}
