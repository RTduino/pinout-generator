#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->fcpuedit->setStyleSheet("font-size:30px; color:rgb(255,0,0);");
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    addui = new add;
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
    QMessageBox::StandardButton result = QMessageBox::question( this,"Delete Arduino Pin","Are you sure you want to delete "+pinlist.at(0)+"?");

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

void Widget::add_item_table()
{
    changeardpin = "";
    addui->show();
}

void Widget::serials_cbox_data()
{
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
}

void Widget::ledpin_cbox_data()
{
    QStringList ledlist("NULL");
    ui->ledbox->clear();
    foreach(auto i,pinmaplist)
    {
        if(i->io_function != "ADC")
        {
            ledlist.removeOne(i->arduino_pin);
            ledlist.append(i->arduino_pin);
        }
    }
    ui->ledbox->addItems(ledlist);
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
        QMessageBox::critical(this,"Error","It's already empty!");
        return;
    }
    QMessageBox::StandardButton result = QMessageBox::question( this,"Clear Table","Are you sure you want to clear table?");

    if(result == QMessageBox::No)
        return;
    /* clear pinmaplist */
    qDeleteAll(pinmaplist.begin(),pinmaplist.end());
    pinmaplist.clear();

    refresh_to_widget();
}

void Widget::on_dirbtn_clicked()
{
    tmprttBspdirpath = QFileDialog::getExistingDirectory( this, "Choose RT-Thread BSP Directory",  "/");
    ui->diredit->setText(tmprttBspdirpath);
}

void Widget::on_importbtn_clicked()
{
    QDir rttBspdir;

    /* 判断路径格式是否正确 */
    tmprttBspdirpath = ui->diredit->text();
    if(tmprttBspdirpath.isEmpty())
    {
        QMessageBox::critical(this,"Error","This is a empty path!");
        return;
    }
    if(!rttBspdir.exists(tmprttBspdirpath))
    {
        QString errinfo = tmprttBspdirpath + " does not exist!";
        QMessageBox::critical(this,"Error",errinfo);
        return;
    }

    rttBspdirpath = QDir::fromNativeSeparators(ui->diredit->text());
    /* import files */
    load_data_from_dir();
    refresh_to_widget();
}

void Widget::on_exportbtn_clicked()
{
    load_data_to_dir();
}

void Widget::recive_add_data(QStringList &strlist)
{
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

        ui->treeWidget->scrollToBottom();
    }

    update_table_data();

}

void Widget::on_addbtn_clicked()
{
    add_item_table();
}


void Widget::on_clearbtn_clicked()
{
    clear_table_data();
}

