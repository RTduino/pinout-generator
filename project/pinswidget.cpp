#include "pinswidget.h"
#include "ui_pinswidget.h"
#include "mcuinfo.h"
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QTreeWidgetItem>

PinsWidget::PinsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PinsWidget)
{
    ui->setupUi(this);
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    uiPinInfo = new PinInfoWidget;

    connect(uiPinInfo, SIGNAL(refreshTreeWidget()), this, SLOT(on_treeWidget_refresh()));

    actionAdd = new QAction(this);
    actionInsert = new QAction(this);
    actionChange = new QAction(this);
    actionRemove = new QAction(this);

    actionAdd->setData("add");
    actionInsert->setData("insert");
    actionChange->setData( "change");
    actionRemove ->setData("remove");


    //把这个QAction的对象加入到当前窗口中去.
    this->addAction(actionAdd);
    this->addAction(actionInsert);
    this->addAction(actionChange);
    this->addAction(actionRemove);

    connect(actionAdd, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(actionInsert, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(actionChange, SIGNAL(triggered()), this, SLOT(on_ShortCut()));
    connect(actionRemove, SIGNAL(triggered()), this, SLOT(on_ShortCut()));

}

PinsWidget::~PinsWidget()
{
    delete ui;
}

void PinsWidget::treeWidgetAddItem(struct pinInfo &info)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0,info.pinArduino);
    if(McuInfo::getInstance()->nowMcuSeries->pinInfoCfg.useNumber == true)
    {
        item->setText(1,info.pinRtthread);
    }
    else
    {
        if(!info.pinRttPort.isEmpty() && !info.pinRttPin.isEmpty())
            item->setText(1,"GET_PIN(" + info.pinRttPort + ", " + info.pinRttPin + ")");
        else
            item->setText(1,"RT_NULL");
    }
    item->setText(2,info.pinDevFunc);
    item->setText(3,info.pinDevName);
    item->setText(4,info.pinUseFunc);
    item->setText(5,info.pinDevChnl);
    item->setText(6,info.vol5vAllow);

    ui->treeWidget->addTopLevelItem(item);

    return;
}

void PinsWidget::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* curItem=ui->treeWidget->itemAt(pos);

    McuInfo::getInstance()->nowMcuSeries->selectPin = nullptr;

    if(curItem != nullptr)
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoSelectedSet(curItem->text(0));
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
    changepin->setData( "change");
    removepin ->setData("remove");
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

    connect(addpin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(insertpin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(changepin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(removepin, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(refreshtb, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));
    connect(cleartb, SIGNAL(triggered()), this, SLOT(on_menu_item_change_event()));

    Menu->exec(cursor().pos());

    QList<QAction*> list = Menu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete Menu;
}

void PinsWidget::on_menu_item_change_event()
{
    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    QString type = pEven->data().toString();

    if(type == "refresh")
    {
        treeWidgetRefresh();
    }
    else if(type == "clear")
    {
        if(McuInfo::getInstance()->nowMcuSeries->pinInfoList.isEmpty())
        {
            QMessageBox::critical(this,"错误","数据已经被清空了！");
            return;
        }
        QMessageBox::StandardButton result = QMessageBox::question( this,"清空数据","你确定要清空所有数据吗？");

        if(result == QMessageBox::No)
            return;

        McuInfo::getInstance()->nowMcuSeries->pinInfoList.clear();

        treeWidgetRefresh();
    }
    else if(type == "remove")
    {
        if(McuInfo::getInstance()->nowMcuSeries->selectPin != nullptr)
        {
            QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 " + \
                                                                        McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino+" 这个引脚吗？");
            if(result == QMessageBox::No)
                return;

            McuInfo::getInstance()->nowMcuSeries->pinInfoDelete(McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino);

            treeWidgetRefresh();
        }
    }
    else
    {
        uiPinInfo->initInfoUi(type);
    }
}

void PinsWidget::treeWidgetRefresh()
{
    // 写入treeWidget
    ui->treeWidget->clear();
    foreach(auto pinItem,McuInfo::getInstance()->nowMcuSeries->pinInfoList)
    {
        treeWidgetAddItem(pinItem);
    }
}

void PinsWidget::ShortCut()
{
    //设置触发QAction对象的快捷操作.
    QSettings setting(qApp->applicationDirPath() +"/Setting.ini", QSettings::IniFormat);
    QString shortcut;

    shortcut = setting.value("addShortcut").toString();
    actionAdd->setShortcut(shortcut);
    shortcut = setting.value("insertShortcut").toString();
    actionInsert->setShortcut(shortcut);
    shortcut = setting.value("changeShortcut").toString();
    actionChange->setShortcut(shortcut);
    shortcut = setting.value("deleteShortcut").toString();
    actionRemove->setShortcut(shortcut);
}

void PinsWidget::on_ShortCut()
{
    qDebug() << "short";

    McuInfo::getInstance()->nowMcuSeries->selectPin = nullptr;

    if(ui->treeWidget->currentItem() != nullptr)
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoSelectedSet(ui->treeWidget->currentItem()->text(0));
    }

    QAction *pEven = qobject_cast<QAction *>(this->sender()); //this->sender()就是发信号者 QAction
    QString type = pEven->data().toString();

    if(type == "remove")
    {
        if(McuInfo::getInstance()->nowMcuSeries->selectPin != nullptr)
        {
            QMessageBox::StandardButton result = QMessageBox::question( this,"删除 Arduino Pin","你确定要删除 " + \
                                                                        McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino+" 这个引脚吗？");
            if(result == QMessageBox::No)
                return;

            McuInfo::getInstance()->nowMcuSeries->pinInfoDelete(McuInfo::getInstance()->nowMcuSeries->selectPin->pinArduino);

            treeWidgetRefresh();
        }
    }
    else
    {
        uiPinInfo->initInfoUi(type);
    }
}

void PinsWidget::loadUi()
{
    qDebug() << this->objectName() << "load";

    McuInfo::getInstance()->nowMcuSeries->pinInfoRefresh();

    treeWidgetRefresh(); // 初始化引脚界面

    ShortCut();
}

void PinsWidget::refreshUi()
{
    qDebug() << this->objectName() << "refresh";
}

void PinsWidget::quitUi()
{
    qDebug() << this->objectName() << "quit";
    McuInfo::getInstance()->nowMcuSeries->pinInfoRefresh();
}

void PinsWidget::on_treeWidget_refresh()
{
    // 处理pinInfoList
    McuInfo::getInstance()->nowMcuSeries->pinInfoRefresh();

    treeWidgetRefresh();

    // 保存工程文件
//    McuInfo::getInstance()->xmlFile->creatXmlFile();

    // 终端显示
    McuInfo::getInstance()->nowMcuSeries->pinInfoShow();
}

void PinsWidget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if(item != nullptr)
    {
        McuInfo::getInstance()->nowMcuSeries->pinInfoSelectedSet(item->text(0));
    }

    uiPinInfo->initInfoUi("change");
}

