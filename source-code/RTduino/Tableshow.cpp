#include "widget.h"
#include "ui_widget.h"

void Widget::add_one_item(Pinmap &pin)
{
    QTreeWidgetItem* item=new QTreeWidgetItem();
    item->setText(0,pin.arduino_pin);
    item->setText(1,pin.rtthread_pin);
    item->setText(2,pin.io_function);
    item->setText(3,pin.io_name);
    item->setText(4,pin.io_channel);

    ui->treeWidget->addTopLevelItem(item);
}

void Widget::refresh_to_widget()
{
    ui->treeWidget->clear();
    for (int i = 0; i < pinmaplist.size() ; i++ ) {
        add_one_item(*pinmaplist.at(i));
    }
    serials_cbox_data();
    ledpin_cbox_data();
}
