#include "widget.h"
#include "ui_widget.h"
#include <QDesktopServices>
#include <QModelIndex>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->fcpuedit->setStyleSheet("font-size:30px; color:rgb(255,0,0);");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->autor->setText(tr("<a href = 'https://github.com/liYony'>https://github.com/liYony</a>"));
    ui->project->setText(tr("<a href = 'https://github.com/RTduino/pinout-generator'>https://github.com/RTduino/pinout-generator</a>"));
    addui = new add;
    addui->setWindowModality(Qt::ApplicationModal);//设置界面不可点击
    connect(addui,SIGNAL(send_add_data(QStringList &)),this,SLOT(recive_add_data(QStringList &)));
}

Widget::~Widget()
{
    delete ui;
    delete addui;
}

void Widget::update_pinmaps()
{
    /* 判断是否是ADC，如果是，Arduino引脚编号是Ax */
    int anum=0,dnum=0;
    for (int i = 0; i < pinmaplist.size() ; i++ ) {
        if(!QString::compare("ADC",pinmaplist.at(i)->io_function))
            pinmaplist.at(i)->arduino_pin = "A"+QString::number(anum++);
        else
            pinmaplist.at(i)->arduino_pin = "D"+QString::number(dnum++);
    }
   refresh_to_widget();
}

void Widget::remove_item_table(QStringList &pinlist)
{
    if(pinlist.at(0).isEmpty()||pinlist.at(0) == "rem")
    {
        return;
    }
    QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 "+pinlist.at(0)+" 这个引脚吗？");

    if(result == QMessageBox::No)
        return;

    for (int i = 0; i < pinmaplist.size() ; i++ ) {

        if(!QString::compare(pinlist.at(0),pinmaplist.at(i)->arduino_pin))
        {
            delete pinmaplist.at(i); /* 释放内存 */
            pinmaplist.removeAt(i); /* 从QList里面删除元素 */
            break;
        }
    }

    update_pinmaps();
}

void Widget::change_item_table(QStringList &pinlist)
{
    if(pinlist.at(0).isEmpty()||pinlist.at(0) == "chg")
    {
        return;
    }
    changeardpin = pinlist.at(0);
    addui->set_widget_info(pinlist);
    addui->show();
}

void Widget::insert_item_table(QStringList &pinlist)
{
    if(pinlist.at(0).isEmpty()||pinlist.at(0) == "ins")
    {
        add_item_table();
        return;
    }
    insertardpin = pinlist.at(0);
    addui->set_insert_info();
    addui->show();
}

void Widget::add_item_table()
{
    changeardpin = "";
    insertardpin = "";
    addui->show();
}

void Widget::serials_cbox_data()
{
    QString curs2(ui->s2box->currentText());
    QString curs3(ui->s3box->currentText());
    QStringList uartlist("NULL");
    ui->s2box->clear();
    ui->s3box->clear();
    foreach(auto i,pinmaplist)
    {
        if(i->io_name.mid(0,4) == "uart")
        {
            uartlist.removeOne(i->io_name);
            uartlist.append(i->io_name);
        }
    }
    ui->s2box->addItems(uartlist);
    ui->s3box->addItems(uartlist);
    ui->s2box->setCurrentText(curs2);
    ui->s3box->setCurrentText(curs3);
}

void Widget::ledpin_cbox_data()
{
    QString led(ui->ledbox->currentText());
    QString spi(ui->spissbox->currentText());
    QStringList ledlist("NULL");
    ui->ledbox->clear();
    ui->spissbox->clear();
    foreach(auto i,pinmaplist)
    {
        if(i->io_function != "ADC")
        {
            ledlist.removeOne(i->arduino_pin);
            ledlist.append(i->arduino_pin);
        }
    }
    ui->ledbox->addItems(ledlist);
    ui->spissbox->addItems(ledlist);
    ui->ledbox->setCurrentText(led);
    ui->spissbox->setCurrentText(spi);
}

void Widget::update_table_data()
{
    if(pinmaplist.isEmpty())
    {
        return;
    }
    foreach(auto i,pinmaplist)
    {
        if(!QString::compare("ADC",i->io_function))
        {
            pinmaplist.move(pinmaplist.indexOf(i),pinmaplist.size()-1);
        }
    }

    update_pinmaps();
}

void Widget::clear_table_data()
{
    if(pinmaplist.isEmpty())
    {
        QMessageBox::critical(this,"错误","数据已经被清空了！");
        return;
    }
    QMessageBox::StandardButton result = QMessageBox::question( this,"清空数据","你确定要清空所有数据吗？");

    if(result == QMessageBox::No)
        return;
    /* clear pinmaplist */
    qDeleteAll(pinmaplist.begin(),pinmaplist.end());
    pinmaplist.clear();

    refresh_to_widget();
}

void Widget::on_dirbtn_clicked()
{
    QDir rttBspdir;
    QSettings setting("./Setting.ini", QSettings::IniFormat);  //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    QString lastPath = setting.value("LastFilePath").toString();  //获取上次的打开路径

    tmprttBspdirpath = QFileDialog::getExistingDirectory( this, "Choose RT-Thread BSP Directory",lastPath);
    ui->diredit->setText(tmprttBspdirpath);

    /* 判断路径格式是否正确 */
    tmprttBspdirpath = ui->diredit->text();
    if(tmprttBspdirpath.isEmpty())
    {
        QMessageBox::critical(this,"错误","请填写BSP路径！");
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
}

void Widget::on_importbtn_clicked()
{
    /* import files */
    load_data_from_dir();
    refresh_to_widget();
}

void Widget::on_exportbtn_clicked()
{
    QDir rttBspdir;
    QSettings setting("./Setting.ini", QSettings::IniFormat);  //QSettings能记录一些程序中的信息，下次再打开时可以读取出来
    /* 判断路径格式是否正确 */
    tmprttBspdirpath = ui->diredit->text();
    if(tmprttBspdirpath.isEmpty())
    {
        QMessageBox::critical(this,"错误","请填写BSP路径！");
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

    update_table_data();
    load_data_to_dir();
}

void Widget::recive_add_data(QStringList &strlist)
{
    int highlight = 0;
    if(!changeardpin.isEmpty())
    {
        foreach(auto i,pinmaplist)
        {
            if(i->arduino_pin == changeardpin)
            {
                i->io_channel = strlist.at(3);
                i->io_function = strlist.at(0);
                i->io_name = strlist.at(2);
                i->rtthread_pin = strlist.at(1);
                highlight = pinmaplist.indexOf(i);
            }
        }
        update_pinmaps();
    }
    else if(!insertardpin.isEmpty())
    {
        foreach(auto i,pinmaplist)
        {
            if(i->arduino_pin == insertardpin)
            {
                auto pmap = new Pinmap;
                pmap->io_channel = strlist.at(3);
                pmap->io_function = strlist.at(0);
                pmap->io_name = strlist.at(2);
                pmap->rtthread_pin = strlist.at(1);
                pinmaplist.insert(pinmaplist.indexOf(i)+1, pmap);
                highlight = pinmaplist.indexOf(i)+1;
            }
        }
        update_pinmaps();
    }
    else
    {
        auto pmap = new Pinmap;
        pmap->io_channel = strlist.at(3);
        pmap->io_function = strlist.at(0);
        pmap->io_name = strlist.at(2);
        pmap->rtthread_pin = strlist.at(1);
        pinmaplist.push_back(pmap);
        update_pinmaps();
        highlight = pinmaplist.indexOf(pmap);
    }

    QModelIndex modelindex = ui->treeWidget->model()->index(highlight, 0);
    ui->treeWidget->setCurrentIndex(modelindex);
//    update_table_data();
}

void Widget::on_addbtn_clicked()
{
    add_item_table();
}


void Widget::on_clearbtn_clicked()
{
    clear_table_data();
}

void Widget::on_project_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link,QUrl::TolerantMode));
}


void Widget::on_autor_linkActivated(const QString &link)
{
    QDesktopServices::openUrl(QUrl(link,QUrl::TolerantMode));
}


void Widget::on_refreashbtn_clicked()
{
    update_table_data();
}


void Widget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QStringList pininfo;
    pininfo << item->text(0) << item->text(1) << item->text(2) << item->text(3) << item->text(4) << "chg";
    change_item_table(pininfo);
}

