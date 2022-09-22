#include "widget.h"
#include "ui_widget.h"

void Widget::on_recive_pininfo_data(QStringList &pininfo)
{
    QString curardpin;
    int highlight;
    if(pininfo.last() == "add")
    {
        curardpin = pinmaplist.add_pinmap_info_to_list(pininfo);
        pinmaplist.adjust_order_to_keep_arduino();

    }
    if(pininfo.last() == "insert")
    {
        curardpin = pinmaplist.insert_item_back_arduinopin(insertpin,pininfo);
        pinmaplist.adjust_order_to_keep_arduino();
    }
    if(pininfo.last() == "change")
    {
        curardpin = pinmaplist.change_item_by_arduinopin(changepin,pininfo);
        pinmaplist.adjust_order_to_keep_arduino();
    }
    all_ui_component_refresh();
    highlight = pinmaplist.find_item_index_by_arduinopin(curardpin);

    QModelIndex modelindex = ui->treeWidget->model()->index(highlight, 0);
    ui->treeWidget->setCurrentIndex(modelindex);
}

void Widget::menu_add_event()
{
    pinUI->show_add_ui_info();
}

void Widget::menu_change_event(QStringList &list)
{
    pinUI->show_change_ui_info(list);
    if(list.at(0).isEmpty())
        return;
    changepin = list.at(0);
}

void Widget::menu_insert_event(QStringList &list)
{
    pinUI->show_insert_ui_info();
    if(list.at(0).isEmpty())
    {
        menu_add_event();
        return;
    }
    insertpin = list.at(0);
}

void Widget::on_delbtn_clicked()
{
    QTreeWidgetItem  *curItem = ui->treeWidget->currentItem();
    QStringList pininfo;
    if(curItem == NULL)
        return;

    QString pin = curItem->text(0);
    QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 "+pin+" 这个引脚吗？");
    if(result == QMessageBox::No)
        return;
    pinmaplist.remove_item_by_arduinopin(pin);
    all_ui_component_refresh();
}

void Widget::menu_remove_event(QStringList &list)
{
    if(list.at(0).isEmpty())
        return;

    removepin = list.at(0);
    QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 "+removepin+" 这个引脚吗？");
    if(result == QMessageBox::No)
        return;

    pinmaplist.remove_item_by_arduinopin(removepin);
    all_ui_component_refresh();
}

void Widget::menu_refresh_event()
{
    if(pinmaplist.Allpinlist.isEmpty())
    {
        return;
    }
    pinmaplist.adjust_order_to_keep_arduino();
    all_ui_component_refresh();
}

void Widget::menu_clear_event()
{
    if(pinmaplist.Allpinlist.isEmpty())
    {
        QMessageBox::critical(this,"错误","数据已经被清空了！");
        return;
    }
    QMessageBox::StandardButton result = QMessageBox::question( this,"清空数据","你确定要清空所有数据吗？");

    if(result == QMessageBox::No)
        return;
    qDeleteAll(pinmaplist.Allpinlist.begin(),pinmaplist.Allpinlist.end());
    pinmaplist.Allpinlist.clear();
    all_ui_component_refresh();
}

void Widget::on_menu_item_change_event()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    QStringList pitemlist( pEven->data().toStringList());
    if(pitemlist.last() == "chg")
    {
        qDebug()<<pitemlist;
        menu_change_event(pitemlist);
        pinUI->show();
    }
    else if(pitemlist.last() == "ins")
    {
       menu_insert_event(pitemlist);
       pinUI->show();
    }
    else if(pitemlist.last() == "rem")
    {
        menu_remove_event(pitemlist);
    }
    else if(pitemlist.last() == "ref")
    {
        menu_refresh_event();
    }
    else if(pitemlist.last() == "clr")
    {
        menu_clear_event();
    }
    else
    {
        menu_add_event();
        pinUI->show();
    }
}

void Widget::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    /* 获取当前被点击的节点 */
    QTreeWidgetItem* curItem=ui->treeWidget->itemAt(pos);
    QStringList pininfo;
    if(curItem != NULL)
        pininfo << curItem->text(0) << curItem->text(1) << curItem->text(2) << curItem->text(3) << curItem->text(4);
    else
        pininfo << "" << "" << "" << "" << "";

    /* 创建菜单 */
    QMenu *Menu = new QMenu(this);

    QAction *addpin = new QAction(tr("添加引脚"), this);
    QAction *insertpin = new QAction(tr("插入引脚"), this);
    QAction *changepin = new QAction(tr("修改引脚"), this);
    QAction *removepin = new QAction(tr("删除引脚"), this);
    QAction *refreshtb = new QAction(tr("刷新页面"), this);
    QAction *cleartb = new QAction(tr("清空页面"), this);

    addpin->setData( pininfo << "add");
    pininfo.removeLast();
    insertpin->setData( pininfo << "ins");
    pininfo.removeLast();
    changepin->setData(pininfo << "chg");
    pininfo.removeLast();
    removepin ->setData(pininfo << "rem");
    pininfo.removeLast();
    refreshtb->setData(pininfo << "ref");
    pininfo.removeLast();
    cleartb->setData(pininfo << "clr");
    pininfo.removeLast();

    /* 把QAction对象添加到菜单上 */
    Menu->addAction(addpin);
    Menu->addAction(insertpin);
    Menu->addAction(changepin);
    Menu->addAction(removepin);
    Menu->addAction(refreshtb);
    Menu->addAction(cleartb);

    /* 修改菜单字体 */
    QFont font;
    font.setPixelSize(25);
    font.setFamily("新宋体");
    font.setBold(true);
    Menu->setFont(font);

    /* 连接鼠标右键点击信号 */
    connect(addpin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(insertpin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(changepin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(removepin, SIGNAL(triggered()), SLOT(on_menu_item_change_event()));
    connect(refreshtb, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(cleartb, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));

    /* 在鼠标右键点击的地方显示菜单 */
    Menu->exec(cursor().pos());

    /* 释放内存 */
    QList<QAction*> list = Menu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete Menu;
}
