#include "xmlfile.h"

#include <QDir>

XmlFile::XmlFile()
{

}

bool XmlFile::creatXmlFile()
{
    PinsMap * nowMcu = McuInfo::getInstance()->nowMcuSeries;
    pinInfoList = nowMcu->pinInfoList;

    QFile file(fileInfo);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    QTextStream out(&file);
    QDomDocument doc;
    QDomText text;
    QDomElement element;
    QDomAttr attr;
    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction( "xml", "version = \'1.0\' encoding=\'UTF-8\'" );
    doc.appendChild( instruction );

    // 添加根节点
    QDomElement root = doc.createElement( "RTDUINO" );
    doc.appendChild(root);

    element = doc.createElement("MCUSERIES");
    text = doc.createTextNode(nowMcu->mcuSeries);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("MCUBOARD");
    text = doc.createTextNode(nowMcu->mcuBoard);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("IMAGEFILE");
    text = doc.createTextNode(nowMcu->aimimagefile);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("USEPINNYMBER");
    text = doc.createTextNode(QString::number(nowMcu->pinInfoCfg.useNumber));
    element.appendChild(text);
    root.appendChild(element);

    // 添加引脚配置
    element = doc.createElement( "PINSTABLE" );
    root.appendChild(element);

    for(int i = 0; i < pinInfoList.size(); i++) {
        QDomElement pinElement = doc.createElement( "PININFO" );
        attr = doc.createAttribute( "ARDUINO" );
        attr.setValue(pinInfoList[i].pinArduino);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "RTTHREADNUMBER" );
        attr.setValue(pinInfoList[i].pinRtthread);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "RTTHREADPORT" );
        attr.setValue(pinInfoList[i].pinRttPort);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "RTTHREADPIN" );
        attr.setValue(pinInfoList[i].pinRttPin);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "DEVFUNC" );
        attr.setValue(pinInfoList[i].pinDevFunc);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "DEVNAME" );
        attr.setValue(pinInfoList[i].pinDevName);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "DEVCHNL" );
        attr.setValue(pinInfoList[i].pinDevChnl);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "ALLOW5V" );
        attr.setValue(pinInfoList[i].vol5vAllow);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "USEFUNC" );
        attr.setValue(pinInfoList[i].pinUseFunc);
        pinElement.setAttributeNode(attr);
        attr = doc.createAttribute( "MAPNOTE" );
        attr.setValue(pinInfoList[i].pinMapNote);
        pinElement.setAttributeNode(attr);
        element.appendChild(pinElement);
    }

    element = doc.createElement( "PWMTOSPI" );
    root.appendChild(element);

    if(nowMcu->pwmTospi == true)
    {
        QDomElement spiElement = doc.createElement( "SPIPININFO" );
        attr = doc.createAttribute( "SPINAME" );
        attr.setValue(nowMcu->spiPins.spi_name);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "SCKPORT" );
        attr.setValue(nowMcu->spiPins.sck.Port);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "SCKPIN" );
        attr.setValue(nowMcu->spiPins.sck.Pin);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "MISOPORT" );
        attr.setValue(nowMcu->spiPins.miso.Port);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "MISOPIN" );
        attr.setValue(nowMcu->spiPins.miso.Pin);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "MOSIPORT" );
        attr.setValue(nowMcu->spiPins.mosi.Port);
        spiElement.setAttributeNode(attr);
        attr = doc.createAttribute( "MOSIPIN" );
        attr.setValue(nowMcu->spiPins.mosi.Pin);
        spiElement.setAttributeNode(attr);
        element.appendChild(spiElement);
    }

    element = doc.createElement("SERIAL1");
    text = doc.createTextNode(nowMcu->serial1Dev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("SERIAL2");
    text = doc.createTextNode(nowMcu->serial2Dev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("SERIAL3");
    text = doc.createTextNode(nowMcu->serial3Dev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("SPIDEVICE");
    text = doc.createTextNode(nowMcu->spiDev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("SPISSPIN");
    text = doc.createTextNode(nowMcu->spissPin);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("LEDPIN");
    text = doc.createTextNode(nowMcu->ledPin);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("I2CDEVICE");
    text = doc.createTextNode(nowMcu->i2cDev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("TIMERDEVICE");
    text = doc.createTextNode(nowMcu->timerDev);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("AUTHORINFO");
    text = doc.createTextNode(nowMcu->authorInfo);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("FREQUENCY");
    text = doc.createTextNode(nowMcu->frequency);
    element.appendChild(text);
    root.appendChild(element);

    element = doc.createElement("CODEDEMO");
    text = doc.createTextNode(nowMcu->codeDemo);
    element.appendChild(text);
    root.appendChild(element);

    doc.save(out, 4);       //each line space of file is 4

    return true;
}

bool XmlFile::praseXmlFile()
{
    PinsMap * nowMcu = McuInfo::getInstance()->nowMcuSeries;
    //    pinInfoList = nowMcu->pinInfoList;

    QDir tmpDir(fileInfo);

    fileInfo = tmpDir.absolutePath(); // 获取相对路径
    fileInfo = QDir::fromNativeSeparators(fileInfo); //  \\  -->  /

    QDomDocument doc;
    QFile file(fileInfo);
    QString error = "";
    int row = 0, column = 0;
    if (!file.open(QIODevice::ReadOnly)) return false;

    if(!doc.setContent(&file, false, &error, &row, &column)){
        qDebug() << "parse file failed:" << row << "---" << column <<":" <<error;
        file.close();
        return false;
    }

    file.close();

    QDomElement root = doc.documentElement();

    QDomNode node = root.firstChild();
    while(!node.isNull()) {
        QDomElement element = node.toElement(); // try to convert the node to an element.
        if(!element.isNull()) {
            qDebug()<<element.tagName() << ":" << element.text();
            if(element.tagName() == "MCUSERIES")
            {
                // 不支持的时候直接返回
                if(!McuInfo::getInstance()->boardMcuList.contains(element.text()))
                {
                    return false;
                }
                McuInfo::getInstance()->mcuSeriesNowSet(element.text());
                nowMcu = McuInfo::getInstance()->nowMcuSeries;
                nowMcu->mcuSeries = element.text();
            }
            else if(element.tagName() == "MCUBOARD")
            {
                nowMcu->mcuBoard = element.text();
            }
            else if(element.tagName() == "USEPINNYMBER")
            {
                if(element.text() == "1")
                    nowMcu->pinInfoCfg.useNumber = true;
                else
                    nowMcu->pinInfoCfg.useNumber = false;
            }
            else if(element.tagName() == "IMAGEFILE")
            {
                nowMcu->srcimagefile = element.text();
            }
            else if(element.tagName() == "SERIAL1")
            {
                nowMcu->serial1Dev = element.text();
            }
            else if(element.tagName() == "SERIAL2")
            {
                nowMcu->serial2Dev = element.text();
            }
            else if(element.tagName() == "SERIAL3")
            {
                nowMcu->serial3Dev = element.text();
            }
            else if(element.tagName() == "SPIDEVICE")
            {
                nowMcu->spiDev = element.text();
            }
            else if(element.tagName() == "SPISSPIN")
            {
                nowMcu->spissPin = element.text();
            }
            else if(element.tagName() == "LEDPIN")
            {
                nowMcu->ledPin = element.text();
            }
            else if(element.tagName() == "I2CDEVICE")
            {
                nowMcu->i2cDev = element.text();
            }
            else if(element.tagName() == "TIMERDEVICE")
            {
                nowMcu->timerDev = element.text();
            }
            else if(element.tagName() == "AUTHORINFO")
            {
                nowMcu->authorInfo = element.text();
            }
            else if(element.tagName() == "FREQUENCY")
            {
                nowMcu->frequency = element.text();
            }
            else if(element.tagName() == "CODEDEMO")
            {
                nowMcu->codeDemo = element.text();
            }
            else {}

            QDomNode nodeson = element.firstChild();
            while(!nodeson.isNull()) {
                QDomElement elementson = nodeson.toElement();
                if(!elementson.isNull()) {
                    pinInfo info;
                    qDebug()<< "---" <<elementson.tagName();
                    if(elementson.tagName() == "SPIPININFO")
                    {
                        if(nowMcu->pwmTospi == true)
                        {
                            if (elementson.hasAttribute("SPINAME")) {
                                qDebug()<< "---" << "SPINAME=" << elementson.attributeNode("SPINAME").value();
                                nowMcu->spiPins.spi_name = elementson.attributeNode("SPINAME").value();
                            }
                            if (elementson.hasAttribute("SCKPORT")) {
                                qDebug()<< "---" << "SCKPORT=" << elementson.attributeNode("SCKPORT").value();
                                nowMcu->spiPins.sck.Port = elementson.attributeNode("SCKPORT").value();
                            }
                            if (elementson.hasAttribute("SCKPIN")) {
                                qDebug()<< "---" << "SCKPIN=" << elementson.attributeNode("SCKPIN").value();
                                nowMcu->spiPins.sck.Pin = elementson.attributeNode("SCKPIN").value();
                            }
                            if (elementson.hasAttribute("MISOPORT")) {
                                qDebug()<< "---" << "MISOPORT=" << elementson.attributeNode("MISOPORT").value();
                                nowMcu->spiPins.miso.Port = elementson.attributeNode("MISOPORT").value();
                            }
                            if (elementson.hasAttribute("MISOPIN")) {
                                qDebug()<< "---" << "MISOPIN=" << elementson.attributeNode("MISOPIN").value();
                                nowMcu->spiPins.miso.Pin = elementson.attributeNode("MISOPIN").value();
                            }
                            if (elementson.hasAttribute("MOSIPORT")) {
                                qDebug()<< "---" << "MOSIPORT=" << elementson.attributeNode("MOSIPORT").value();
                                nowMcu->spiPins.mosi.Port = elementson.attributeNode("MOSIPORT").value();
                            }
                            if (elementson.hasAttribute("MOSIPIN")) {
                                qDebug()<< "---" << "MOSIPIN=" << elementson.attributeNode("MOSIPIN").value();
                                nowMcu->spiPins.mosi.Pin = elementson.attributeNode("MOSIPIN").value();
                            }
                        }
                    }
                    if(elementson.tagName() == "PININFO")
                    {
                        if (elementson.hasAttribute("DEVNAME")) {
                            qDebug()<< "---" << "DEVNAME=" << elementson.attributeNode("DEVNAME").value();
                            info.pinDevName = elementson.attributeNode("DEVNAME").value();

                        }
                        if (elementson.hasAttribute("ALLOW5V")) {
                            qDebug()<< "---" << "ALLOW5V=" << elementson.attributeNode("ALLOW5V").value();
                            info.vol5vAllow = elementson.attributeNode("ALLOW5V").value();
                        }
                        if (elementson.hasAttribute("DEVCHNL")) {
                            qDebug()<< "---" << "DEVCHNL=" << elementson.attributeNode("DEVCHNL").value();
                            info.pinDevChnl = elementson.attributeNode("DEVCHNL").value();
                        }
                        if (elementson.hasAttribute("RTTHREADNUMBER")) {
                            qDebug()<< "---" << "RTTHREADNUMBER=" << elementson.attributeNode("RTTHREADNUMBER").value();
                            info.pinRtthread = elementson.attributeNode("RTTHREADNUMBER").value();
                        }
                        if (elementson.hasAttribute("RTTHREADPORT")) {
                            qDebug()<< "---" << "RTTHREADPORT=" << elementson.attributeNode("RTTHREADPORT").value();
                            info.pinRttPort = elementson.attributeNode("RTTHREADPORT").value();
                        }
                        if (elementson.hasAttribute("RTTHREADPIN")) {
                            qDebug()<< "---" << "RTTHREADPIN=" << elementson.attributeNode("RTTHREADPIN").value();
                            info.pinRttPin = elementson.attributeNode("RTTHREADPIN").value();
                        }
                        if (elementson.hasAttribute("ARDUINO")) {
                            qDebug()<< "---" << "ARDUINO=" << elementson.attributeNode("ARDUINO").value();
                            info.pinArduino = elementson.attributeNode("ARDUINO").value();
                        }
                        if (elementson.hasAttribute("MAPNOTE")) {
                            qDebug()<< "---" << "MAPNOTE=" << elementson.attributeNode("MAPNOTE").value();
                            info.pinMapNote = elementson.attributeNode("MAPNOTE").value();
                        }
                        if (elementson.hasAttribute("DEVFUNC")) {
                            qDebug()<< "---" << "DEVFUNC=" << elementson.attributeNode("DEVFUNC").value();
                            info.pinDevFunc = elementson.attributeNode("DEVFUNC").value();
                        }
                        if (elementson.hasAttribute("USEFUNC")) {
                            qDebug()<< "---" << "USEFUNC=" << elementson.attributeNode("USEFUNC").value();
                            info.pinUseFunc = elementson.attributeNode("USEFUNC").value();
                        }

                        nowMcu->pinInfoAdd(info);
                    }

                }
                nodeson = nodeson.nextSibling();
            }
        }
        node = node.nextSibling();
    }

    return true;
}




