#ifndef XMLFILE_H
#define XMLFILE_H

#include <QDomDocument>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <mcuinfo.h>

class XmlFile
{
public:
    XmlFile();

    bool creatXmlFile();
    bool praseXmlFile();

    QString fileInfo;
private:
    QVector<struct pinInfo> pinInfoList;
};

#endif // XMLFILE_H
