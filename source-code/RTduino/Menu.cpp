#include "widget.h"
#include "ui_widget.h"
#include <QMenu>
#include <QAction>


void Widget::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* curItem=ui->treeWidget->itemAt(pos);  //获取当前被点击的节点
    QStringList pininfo;
    if(curItem != NULL)
    {

        pininfo << curItem->text(0) << curItem->text(1) << curItem->text(2) << curItem->text(3) << curItem->text(4);
    }
    //创建菜单对象
    QMenu *Menu = new QMenu(this);

    QAction *addpin = new QAction(tr("添加引脚"), this);
    QAction *changepin = new QAction(tr("修改引脚"), this);
    QAction *removepin = new QAction(tr("删除引脚"), this);
    QAction *refreshtb = new QAction(tr("刷新页面"), this);
    QAction *cleartb = new QAction(tr("清空页面"), this);

    addpin->setData( pininfo << "add");
    pininfo.removeLast();
    changepin->setData(pininfo << "chg");
    pininfo.removeLast();
    removepin ->setData(pininfo << "rem");
    pininfo.removeLast();
    refreshtb->setData(pininfo << "ref");
    pininfo.removeLast();
    cleartb->setData(pininfo << "clr");
    pininfo.removeLast();

    //把QAction对象添加到菜单上
    Menu->addAction(addpin);
    Menu->addAction(changepin);
    Menu->addAction(removepin);
    Menu->addAction(refreshtb);
    Menu->addAction(cleartb);
    QFont font;
    font.setPixelSize(25);
    font.setFamily("新宋体");
    font.setBold(true);
    Menu->setFont(font);

    //连接鼠标右键点击信号
    connect(addpin, SIGNAL(triggered()), this, SLOT(on_menu_envent()));
    connect(changepin, SIGNAL(triggered()), this, SLOT(on_menu_envent()));
    connect(removepin, SIGNAL(triggered()), SLOT(on_menu_envent()));
    connect(refreshtb, SIGNAL(triggered()), this, SLOT(on_menu_envent()));
    connect(cleartb, SIGNAL(triggered()), this, SLOT(on_menu_envent()));

    //在鼠标右键点击的地方显示菜单
    Menu->exec(cursor().pos());

    //释放内存
    QList<QAction*> list = Menu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete Menu;
}

void Widget::on_menu_envent()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    QStringList pitemlist( pEven->data().toStringList());
    if(pitemlist.last() == "chg")
    {
        change_item_table(pitemlist);
    }
    else if(pitemlist.last() == "rem")
    {
        remove_item_table(pitemlist);
    }
    else if(pitemlist.last() == "ref")
    {
        update_table_data();
    }
    else if(pitemlist.last() == "clr")
    {
        clear_table_data();
    }
    else
    {
        add_item_table();
    }
}

