#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
void regedit_new(QString appPath,QString className,QString suffix,QString value)
{
    QString baseUrl("HKEY_CURRENT_USER\\Software\\Classes");    // 注册表的位置
    QSettings settings (baseUrl, QSettings::NativeFormat);
    settings.setValue("/" + className + "/.", value);       //新建类别
    settings.setValue("/" + className + "/Shell/Open/Command/.", "\"" + appPath + "\" \"%1\"");    //添加打开指令,传给main函数的参数
    settings.setValue("/" + className + "/DefaultIcon/.", appPath + ",0");        //该类型文件图标，这里设置与程序图标相同
    settings.setValue("/" + suffix+ "/OpenWithProgIds/" + className, "");    // 关联suffix 和 类别
    settings.sync(); 		//立即保存该修改
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    McuInfo::getInstance()->mcuSeriesRegister(new Stm32PinsMap, "STM32");
//    McuInfo::getInstance()->mcuSeriesRegister(new Ch32PinsMap, "CH32");
//    McuInfo::getInstance()->mcuSeriesRegister(new DefaultPinsMap, "DEFAULT");
    McuInfo::getInstance()->mcuSeriesShow(); // 注册支持的MCU

    McuInfo::getInstance()->mcuSeriesNowSet(McuInfo::getInstance()->boardMcuList[0]);

    MainWindow w;

    w.setXmlfileName(""); // 默认XMLfile是没有的

    if(argc == 1) {		//设置条件不用每次都设置
        QString appPath = qApp->applicationFilePath();
        appPath = appPath.split("/").join("\\");
        QString className("rdpg");                     // 自定义的类别
        QString suffix(".rdpg");                       // 关联的文件类型
        QString value("自定义文件类型");
        regedit_new(appPath,className,suffix,value);
    }
    if(argc == 2)
    {
        w.setXmlfileName(argv[1]);
    }
    w.loadXmlfileToUi();
    w.show();
    return a.exec();
}
