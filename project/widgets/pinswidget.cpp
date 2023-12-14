#include "pinswidget.h"
#include "ui_pinswidget.h"

PinsWidget::PinsWidget(QWidget *parent) : QWidget(parent),
    ui(new Ui::PinsWidget)
{
    ui->setupUi(this);
    rtduino = RTduinoConfig::getInstance();

    bindShortCutAction();

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

PinsWidget::~PinsWidget()
{
    delete ui;
}

void PinsWidget::treeWidgetAddItem(pin_info_t &info)
{
    QTreeWidgetItem *item = new QTreeWidgetItem();

    item->setText(0, info.pin_arduino);

    if (!info.pin_rttport.isEmpty() && !info.pin_rttpin.isEmpty())
    {
        QString rtthread_format = rtduino->getNowMcuSeries().iopininfo.format;
        rtthread_format.replace("(port)", info.pin_rttport);
        rtthread_format.replace("(pin)", info.pin_rttpin);
        item->setText(1, rtthread_format);
    }
    else
    {
        item->setText(1, "RT_NULL");
    }

    item->setText(2, info.pin_function);
    item->setText(3, info.pin_name);
    item->setText(4, info.pin_iomap);
    item->setText(5, info.pin_channel);
    item->setText(6, info.pin_5vallow ? "Yes" : "No");

    ui->treeWidget->addTopLevelItem(item);
}

void PinsWidget::treeWidgetRefresh()
{
    // 写入treeWidget
    ui->treeWidget->clear();

    foreach (auto pininfo, rtduino->getAllPinInfoList())
    {
        treeWidgetAddItem(pininfo);
    }
}

void PinsWidget::loadUi()
{
    updateShortCut();
    treeWidgetRefresh();
}

void PinsWidget::refreshUi()
{
}

void PinsWidget::quitUi()
{
}

void PinsWidget::updateShortCut()
{
    // 设置触发QAction对象的快捷操作.
    QSettings setting(qApp->applicationDirPath() + "/Setting.ini", QSettings::IniFormat);
    QString shortcut;

    shortcut = setting.value("addShortcut").toString();
    action_add->setShortcut(shortcut);
    shortcut = setting.value("insertShortcut").toString();
    action_insert->setShortcut(shortcut);
    shortcut = setting.value("changeShortcut").toString();
    action_change->setShortcut(shortcut);
    shortcut = setting.value("deleteShortcut").toString();
    action_remove->setShortcut(shortcut);
}

void PinsWidget::on_ShortCut()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); // this->sender()就是发信号者 QAction
    QString type = pEven->data().toString();
    // 当前选中的引脚
    if(ui->treeWidget->currentItem() != nullptr)
    {
        select_pin = ui->treeWidget->currentItem()->text(0);
    }
    else
    {
        select_pin = "";
    }
    if(type == "remove")
    {
        if(select_pin != "")
        {
            QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 " + \
                                                                        select_pin+" 这个引脚吗？");
            if(result == QMessageBox::No)
                return;

            rtduino->removePinInfo(select_pin);

            treeWidgetRefresh();
        }
    }
    else if(type == "change")
    {
        if (select_pin == "")
        {
            return;
        }
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initchangeWidget(rtduino->getPinInfo(select_pin));
        ui_pininfo->show();
    }
    else if(type == "insert")
    {
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initinsertWidget(rtduino->getPinInfo(select_pin));
        ui_pininfo->show();
    }
    else
    {
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initaddWidget();
        ui_pininfo->show();
    }
}

void PinsWidget::bindShortCutAction()
{
    action_add = new QAction(this);
    action_insert = new QAction(this);
    action_change = new QAction(this);
    action_remove = new QAction(this);

    action_add->setData("add");
    action_insert->setData("insert");
    action_change->setData("change");
    action_remove->setData("remove");

    // 把这个QAction的对象加入到当前窗口中去.
    this->addAction(action_add);
    this->addAction(action_insert);
    this->addAction(action_change);
    this->addAction(action_remove);

    connect(action_add, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(action_insert, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(action_change, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(action_remove, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
}

void PinsWidget::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *curItem = ui->treeWidget->itemAt(pos);

    if (curItem != nullptr)
    {
        select_pin = curItem->text(0);
    }
    else
    {
        select_pin = "";
    }

    QMenu *Menu = new QMenu(this);
    QAction *addpin = new QAction(tr("添加引脚"), this);
    QAction *insertpin = new QAction(tr("插入引脚"), this);
    QAction *changepin = new QAction(tr("修改引脚"), this);
    QAction *removepin = new QAction(tr("删除引脚"), this);
    QAction *refreshtb = new QAction(tr("刷新页面"), this);
    QAction *cleartb = new QAction(tr("清空页面"), this);

    addpin->setData("add");
    insertpin->setData("insert");
    changepin->setData("change");
    removepin->setData("remove");
    refreshtb->setData("refresh");
    cleartb->setData("clear");

    Menu->addAction(addpin);
    Menu->addAction(insertpin);
    Menu->addAction(changepin);
    Menu->addAction(removepin);
    Menu->addAction(refreshtb);
    Menu->addAction(cleartb);

    QFont font;
    font.setPixelSize(19);
    font.setFamily("微软雅黑 Light");
    font.setBold(true);
    Menu->setFont(font);

    connect(addpin, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));
    connect(insertpin, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));
    connect(changepin, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));
    connect(removepin, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));
    connect(refreshtb, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));
    connect(cleartb, SIGNAL(triggered()), this, SLOT(menu_item_change_event()));

    Menu->exec(cursor().pos());

    QList<QAction *> list = Menu->actions();
    foreach (QAction *pAction, list)
        delete pAction;
    delete Menu;
}

void PinsWidget::treeWidget_refresh()
{
    treeWidgetRefresh();
}

void PinsWidget::menu_item_change_event()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); // this->sender()就是发信号者 QAction
    QString type = pEven->data().toString();

    qDebug() << type << select_pin;
    if(type == "refresh")
    {
        treeWidgetRefresh();
    }
    else if(type == "clear")
    {
        if(rtduino->getAllPinInfoList().isEmpty())
        {
            QMessageBox::critical(this,"错误","数据已经被清空了！");
            return;
        }
        QMessageBox::StandardButton result = QMessageBox::question( this,"清空数据","你确定要清空所有数据吗？");

        if(result == QMessageBox::No)
            return;

        rtduino->clearPinInfoList();

        treeWidgetRefresh();
    }
    else if(type == "remove")
    {
        if(select_pin != "")
        {
            QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 " + \
                                                                        select_pin+" 这个引脚吗？");
            if(result == QMessageBox::No)
                return;

            rtduino->removePinInfo(select_pin);

            treeWidgetRefresh();
        }
    }
    else if(type == "change")
    {
        if (select_pin == "")
        {
            return;
        }
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initchangeWidget(rtduino->getPinInfo(select_pin));
        ui_pininfo->show();
    }
    else if(type == "insert")
    {
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initinsertWidget(rtduino->getPinInfo(select_pin));
        ui_pininfo->show();
    }
    else
    {
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initaddWidget();
        ui_pininfo->show();
    }
}

void PinsWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (item != nullptr)
    {
        select_pin = item->text(0);
        if (select_pin == "")
        {
            return;
        }
        ui_pininfo = new PinInfoWidget;
        connect(ui_pininfo, SIGNAL(refreshTreeWidget()), this, SLOT(treeWidget_refresh()));
        ui_pininfo->setAttribute(Qt::WA_DeleteOnClose);
        ui_pininfo->initchangeWidget(rtduino->getPinInfo(select_pin));
        ui_pininfo->show();
    }
}

