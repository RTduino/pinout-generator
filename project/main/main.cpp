#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <version.h>

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

    MainWindow w;

    if(argc == 1) {		//设置条件不用每次都设置
        QString appPath = qApp->applicationFilePath();
        appPath = appPath.split("/").join("\\");
        QString className("rdpg");                     // 自定义的类别
        QString suffix(".rdpg");                       // 关联的文件类型
        QString value("自定义文件类型");
        regedit_new(appPath,className,suffix,value);
        w.initStackedWidget();
    }
    if(argc == 2)
    {
        RTduinoConfig::getInstance()->parsePinInfoJson(argv[1]);
        QFileInfo fileInfo(argv[1]);
        RTduinoConfig::getInstance()->getProjectInfo()->project_path = fileInfo.path();
        RTduinoConfig::getInstance()->getProjectInfo()->project_name = fileInfo.baseName();
        w.initStackedWidget();
    }

    w.show();
    QString version = "RTduino Pinout Generator V" + get_version_string();
    w.setWindowTitle(version);

    return a.exec();
}

QString get_version_string(void)
{
    return QString::number(PG_VERSION_MAJOR) + "." +
           QString::number(PG_VERSION_MINOR) + "." +
           QString::number(PG_VERSION_PATCH);
}
