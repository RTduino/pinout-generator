#include "rtduinoconfig.h"

RTduinoConfig *RTduinoConfig::instance = nullptr;

RTduinoConfig::RTduinoConfig()
{
    all_series.clear();
    all_pin_infos.clear();

    praseAllMcuSeries();
}

bool RTduinoConfig::parseMcuSeriesJson(QString file)
{
    mcu_series_t _series;

    QJsonObject json_root;

    if (getJsonObject(file, json_root) != true)
    {
        return false;
    }

    // ========== 解析文件 ========== //
    /*****************************************************/
    QJsonValue series_value = json_root.value("series");
    QJsonValue date_value = json_root.value("date");
    QJsonValue author_value = json_root.value("author");
    QJsonValue child_series_value = json_root.value("childseries");
    if (child_series_value.type() == QJsonValue::Array)
    {
        QJsonArray child_series_array = child_series_value.toArray();
        _series.childseries.clear();
        for (int i = 0; i < child_series_array.size(); i++)
        {
            QJsonValue child_s_value = child_series_array.at(i);
            _series.childseries.append(child_s_value.toString());
        }
        qDebug() << _series.childseries;
    }
    _series.series = series_value.toString();
    _series.date = date_value.toString();
    _series.author = author_value.toString();
    /*****************************************************/
    qDebug() << "-->" << _series.series;
    qDebug() << "-->" << _series.date;
    qDebug() << "-->" << _series.author;
    /*****************************************************/
    // ========= 解析pininfo ========= //
    QJsonValue pininfo_value = json_root.value("pininfo");
    if (pininfo_value.type() == QJsonValue::Object)
    {
        QJsonObject pininfo_object = pininfo_value.toObject();
        /*****************************************************/
        QJsonValue format_value = pininfo_object.value("format");
        _series.iopininfo.format = format_value.toString();
        qDebug() << "    -->" << _series.iopininfo.format;
        /*****************************************************/
        QJsonValue ports_value = pininfo_object.value("ports");
        if (ports_value.type() == QJsonValue::Array)
        {
            QJsonArray port_array = ports_value.toArray();
            _series.iopininfo.ports.clear();
            for (int i = 0; i < port_array.size(); i++)
            {
                QJsonValue port_value = port_array.at(i);
                _series.iopininfo.ports.append(port_value.toString());
            }
            qDebug() << "        -->" << _series.iopininfo.ports;
        }
        QJsonValue pins_value = pininfo_object.value("pins");
        if (pins_value.type() == QJsonValue::Array)
        {
            QJsonArray pin_array = pins_value.toArray();
            _series.iopininfo.pins.clear();
            for (int i = 0; i < pin_array.size(); i++)
            {
                QJsonValue pin_value = pin_array.at(i);
                _series.iopininfo.pins.append(pin_value.toString());
            }
            qDebug() << "        -->" << _series.iopininfo.pins;
        }
    }
    // ========= 解析timers ========= //
    QJsonValue timers_value = json_root.value("timers");
    if (timers_value.type() == QJsonValue::Array)
    {
        QJsonArray timers_array = timers_value.toArray();
        for (int i = 0; i < timers_array.size(); i++)
        {
            QJsonValue timer_value = timers_array.at(i);
            _series.timers.append(timer_value.toString());
        }
    }
    // ========= 解析iofunctions ========= //
    QJsonValue iofunctions_value = json_root.value("iofunctions");
    if (iofunctions_value.type() == QJsonValue::Array)
    {
        QJsonArray iofunctions_array = iofunctions_value.toArray();
        for (int i = 0; i < iofunctions_array.size(); i++)
        {
            QJsonValue iofunction_value = iofunctions_array.at(i);
            if (iofunction_value.type() == QJsonValue::Object)
            {
                io_function_t iofunc_info;
                QJsonObject iofunction_object = iofunction_value.toObject();
                /*****************************************************/
                QJsonValue function_value = iofunction_object.value("function");
                QJsonValue support_value = iofunction_object.value("support");
                QJsonValue help_value = iofunction_object.value("help");
                QJsonValue ioremaps_value = iofunction_object.value("ioremaps");
                QJsonValue names_value = iofunction_object.value("names");
                QJsonValue channels_value = iofunction_object.value("channels");

                qDebug() << "    -->" << support_value.toBool();
                if (support_value.toBool() == false)
                {
                    continue;
                }
                /*****************************************************/
                iofunc_info.function = function_value.toString();
                iofunc_info.help = help_value.toString();
                /*****************************************************/
                qDebug() << "    -->" << iofunc_info.function;
                qDebug() << "    -->" << iofunc_info.help;
                /*****************************************************/
                if (ioremaps_value.type() == QJsonValue::Array)
                {
                    QJsonArray ioremap_array = ioremaps_value.toArray();
                    for (int i = 0; i < ioremap_array.size(); i++)
                    {
                        QJsonValue ioremap_value = ioremap_array.at(i);
                        iofunc_info.ioremaps.append(ioremap_value.toString());
                    }
                    qDebug() << "        -->" << iofunc_info.ioremaps;
                }
                if (names_value.type() == QJsonValue::Array)
                {
                    QJsonArray name_array = names_value.toArray();
                    for (int i = 0; i < name_array.size(); i++)
                    {
                        QJsonValue name_value = name_array.at(i);
                        iofunc_info.names.append(name_value.toString());
                    }
                    qDebug() << "        -->" << iofunc_info.names;
                }
                if (channels_value.type() == QJsonValue::Array)
                {
                    QJsonArray channel_array = channels_value.toArray();
                    for (int i = 0; i < channel_array.size(); i++)
                    {
                        QJsonValue channel_value = channel_array.at(i);
                        iofunc_info.channels.append(channel_value.toString());
                    }
                    qDebug() << "        -->" << iofunc_info.channels;
                }
                _series.iofunctions.append(iofunc_info);
            }

        }
    }

    all_series.append(_series);

    // ========== 解析结束 ========== //
    return true;
}

bool RTduinoConfig::parsePinInfoJson(QString file)
{
    QJsonObject json_root;

    if (getJsonObject(file, json_root) != true)
    {
        return false;
    }

    // ========== 解析文件 ========== //
    /*****************************************************/
    QJsonValue series_value = json_root.value("series");
    QJsonValue child_series_value = json_root.value("childseries");
    QJsonValue author_value = json_root.value("author");
    QJsonValue board_value = json_root.value("board_name");
    QJsonValue proname_value = json_root.value("project_name");
    QJsonValue propath_value = json_root.value("project_path");
    QJsonValue image_value = json_root.value("pinmap_image");

    QJsonValue serial1_value = json_root.value("serial1");
    QJsonValue serial2_value = json_root.value("serial2");
    QJsonValue serial3_value = json_root.value("serial3");
    QJsonValue spi_value = json_root.value("spi");
    QJsonValue spiss_value = json_root.value("spiss");
    QJsonValue ledpin_value = json_root.value("ledpin");
    QJsonValue i2c_value = json_root.value("i2c");
    QJsonValue timer_value = json_root.value("timer");
    QJsonValue frequency_value = json_root.value("frequency");
    QJsonValue codedemo_value = json_root.value("codedemo");
    qDebug() << "-->" << series_value.toString();
    qDebug() << "-->" << author_value.toString();
    qDebug() << "-->" << serial1_value.toString();
    qDebug() << "-->" << serial2_value.toString();
    qDebug() << "-->" << serial3_value.toString();
    qDebug() << "-->" << spi_value.toString();
    qDebug() << "-->" << spiss_value.toString();
    qDebug() << "-->" << ledpin_value.toString();
    qDebug() << "-->" << i2c_value.toString();
    qDebug() << "-->" << timer_value.toString();
    qDebug() << "-->" << frequency_value.toString();
    qDebug() << "-->" << codedemo_value.toString();
    /*****************************************************/
    project_info.pinfo_series = series_value.toString();
    project_info.child_series = child_series_value.toString();
    project_info.pinfo_author = author_value.toString();
    project_info.board_name = board_value.toString();
    project_info.project_name = proname_value.toString();
    project_info.project_path = propath_value.toString();
    project_info.pinmap_image = image_value.toString();

    function_info.serial1 = serial1_value.toString();
    function_info.serial2 = serial2_value.toString();
    function_info.serial3 = serial3_value.toString();
    function_info.spi = spi_value.toString();
    function_info.spiss = spiss_value.toString();
    function_info.ledpin = ledpin_value.toString();
    function_info.i2c = i2c_value.toString();
    function_info.timer = timer_value.toString();
    function_info.frequency = frequency_value.toString();
    function_info.codedemo = codedemo_value.toString();
    /*****************************************************/
    // ========= 解析pwmtospi ========= //
    QJsonValue pwmtospi_value = json_root.value("pwmtospi");
    if (pwmtospi_value.type() == QJsonValue::Object)
    {
        QJsonObject pwmtospi_object = pwmtospi_value.toObject();
        QJsonValue sckport_value = pwmtospi_object.value("sck_port");
        QJsonValue sckpin_value = pwmtospi_object.value("sck_pin");
        QJsonValue misoport_value = pwmtospi_object.value("miso_port");
        QJsonValue misopin_value = pwmtospi_object.value("miso_pin");
        QJsonValue mosiport_value = pwmtospi_object.value("mosi_port");
        QJsonValue mosipin_value = pwmtospi_object.value("mosi_pin");
        QJsonValue spiname_value = pwmtospi_object.value("spi_name");
        function_info.pwmtospi.sck_port = sckport_value.toString();
        function_info.pwmtospi.sck_pin = sckpin_value.toString();
        function_info.pwmtospi.miso_port = misoport_value.toString();
        function_info.pwmtospi.miso_pin = misopin_value.toString();
        function_info.pwmtospi.mosi_port = mosiport_value.toString();
        function_info.pwmtospi.mosi_pin = mosipin_value.toString();
        function_info.pwmtospi.spi_name = spiname_value.toString();
    }
    // ========= 解析pininfos ========= //
    QJsonValue pininfos_value = json_root.value("pininfos");
    if (pininfos_value.type() == QJsonValue::Array)
    {
        all_pin_infos.clear();
        QJsonArray pininfos_array = pininfos_value.toArray();
        for (int i = 0; i < pininfos_array.size(); i++)
        {
            pin_info_t _pin_info;
            QJsonValue pininfo_value = pininfos_array.at(i);
            if (pininfo_value.type() == QJsonValue::Object)
            {
                QJsonObject pininfo_object = pininfo_value.toObject();
                /*****************************************************/
                QJsonValue arduino_value = pininfo_object.value("arduino");
                QJsonValue rtthread_value = pininfo_object.value("rtthread");
                QJsonValue rttport_value = pininfo_object.value("rttport");
                QJsonValue rttpin_value = pininfo_object.value("rttpin");
                QJsonValue function_value = pininfo_object.value("function");
                QJsonValue name_value = pininfo_object.value("name");
                QJsonValue channel_value = pininfo_object.value("channel");
                QJsonValue iomap_value = pininfo_object.value("iomap");
                QJsonValue notes_value = pininfo_object.value("notes");
                QJsonValue allow5v_value = pininfo_object.value("allow5v");
                qDebug() << "    -->" << arduino_value.toString();
                qDebug() << "    -->" << rtthread_value.toString();
                qDebug() << "    -->" << rttport_value.toString();
                qDebug() << "    -->" << rttpin_value.toString();
                qDebug() << "    -->" << function_value.toString();
                qDebug() << "    -->" << name_value.toString();
                qDebug() << "    -->" << channel_value.toString();
                qDebug() << "    -->" << iomap_value.toString();
                qDebug() << "    -->" << notes_value.toString();
                qDebug() << "    -->" << allow5v_value.toBool();
                /*****************************************************/
                _pin_info.pin_arduino =  arduino_value.toString();
                _pin_info.pin_rtthread = rtthread_value.toString();
                _pin_info.pin_rttport = rttport_value.toString();
                _pin_info.pin_rttpin = rttpin_value.toString();
                _pin_info.pin_function = function_value.toString();
                _pin_info.pin_name = name_value.toString();
                _pin_info.pin_channel = channel_value.toString();
                _pin_info.pin_iomap = iomap_value.toString();
                _pin_info.pin_notes = notes_value.toString();
                _pin_info.pin_5vallow = allow5v_value.toBool();
            }
            all_pin_infos.append(_pin_info);
        }
    }
    // ========== 解析结束 ========== //
    return true;
}

bool RTduinoConfig::createPinInfoJson(QString file)
{
    // 定义 pininfos [ ] 对象
    QJsonArray pininfos_array;
    foreach (auto pin, all_pin_infos) {
        // 定义 { } 对象
        QJsonObject pininfo_object;

        pininfo_object.insert("arduino", pin.pin_arduino);
        pininfo_object.insert("rtthread", pin.pin_rtthread);
        pininfo_object.insert("rttport", pin.pin_rttport);
        pininfo_object.insert("rttpin", pin.pin_rttpin);
        pininfo_object.insert("function", pin.pin_function);
        pininfo_object.insert("name", pin.pin_name);
        pininfo_object.insert("channel", pin.pin_channel);
        pininfo_object.insert("iomap", pin.pin_iomap);
        pininfo_object.insert("notes", pin.pin_notes);
        pininfo_object.insert("allow5v", pin.pin_5vallow);

        pininfos_array.append(pininfo_object);
    }
    // 定义 Main { } 对象
    QJsonObject main_object;
    // 插入元素，对应键值对
    main_object.insert("pininfos", pininfos_array);

    main_object.insert("series", project_info.pinfo_series);
    main_object.insert("childseries", project_info.child_series);
    main_object.insert("author", project_info.pinfo_author);
    main_object.insert("board_name", project_info.board_name);
    main_object.insert("project_name", project_info.project_name);
    main_object.insert("project_path", project_info.project_path);
    main_object.insert("pinmap_image", project_info.pinmap_image);

    main_object.insert("serial1", function_info.serial1);
    main_object.insert("serial2", function_info.serial2);
    main_object.insert("serial3", function_info.serial3);
    main_object.insert("spi", function_info.spi);
    main_object.insert("spiss", function_info.spiss);
    main_object.insert("ledpin", function_info.ledpin);
    main_object.insert("i2c", function_info.i2c);
    main_object.insert("timer", function_info.timer);
    main_object.insert("frequency", function_info.frequency);
    main_object.insert("codedemo", function_info.codedemo);

    QJsonObject pwmtospi_object;
    pwmtospi_object.insert("sck_port", function_info.pwmtospi.sck_port);
    pwmtospi_object.insert("sck_pin", function_info.pwmtospi.sck_pin);
    pwmtospi_object.insert("miso_port", function_info.pwmtospi.miso_port);
    pwmtospi_object.insert("miso_pin", function_info.pwmtospi.miso_pin);
    pwmtospi_object.insert("mosi_port", function_info.pwmtospi.mosi_port);
    pwmtospi_object.insert("mosi_pin", function_info.pwmtospi.mosi_pin);
    pwmtospi_object.insert("spi_name", function_info.pwmtospi.spi_name);

    main_object.insert("pwmtospi", pwmtospi_object);
    // 将json对象里的数据转换为字符串
    QJsonDocument doc;
    // 将object设置为本文档的主要对象
    doc.setObject(main_object);

    QFile jsonfile(file);
    if (!jsonfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "can't open error!";
        return false;
    }

    QTextStream stream(&jsonfile);
    stream.setCodec("UTF-8");		// 设置写入编码是UTF8
    // 写入文件
    stream << doc.toJson();
    jsonfile.close();

    return true;
}

bool RTduinoConfig::getAllMcuSeries(QStringList &mlist)
{
    if (all_series.size() == 0)
    {
        qDebug() << "not series.";
        return false;
    }

    foreach (auto series, all_series) {
        mlist.append(series.series);
    }

    return false;
}

bool RTduinoConfig::setNowMcuSeries(QString series_name)
{
    mcu_series_t *_series = findMcuSeries(series_name);
    if (_series == nullptr)
    {
        return false;
    }

    now_series = *_series;

    return true;
}

io_function_t RTduinoConfig::getIoFunctionConfig(QString function)
{
    io_function_t io_function;
    for (int i = 0; i < now_series.iofunctions.size(); i++)
    {
        if (now_series.iofunctions.at(i).function.toUpper() == function.toUpper())
        {
            io_function = now_series.iofunctions.at(i);
        }
    }

    return io_function;
}

mcu_series_t RTduinoConfig::getMcuSeries(QString series_name)
{
    mcu_series_t _series = *findMcuSeries(series_name);

    return _series;
}

void RTduinoConfig::addPinInfo(pin_info_t info)
{
    all_pin_infos.append(info);
    fullPinInfo();
}

bool RTduinoConfig::insertPinInfo(QString arduino, pin_info_t info)
{
    int index = findIndexPinInfo(arduino);
    if (index == -1)
    {
        return false;
    }

    all_pin_infos.insert(index + 1, info);
    fullPinInfo();
    return true;
}

bool RTduinoConfig::removePinInfo(QString arduino)
{
    int index = findIndexPinInfo(arduino);
    if (index == -1)
    {
        return false;
    }

    all_pin_infos.removeAt(index);
    fullPinInfo();
    return true;
}

bool RTduinoConfig::changePinInfo(QString arduino, pin_info_t info)
{
    int index = findIndexPinInfo(arduino);
    if (index == -1)
    {
        return false;
    }

    all_pin_infos.replace(index, info);
    fullPinInfo();
    return true;
}

QStringList RTduinoConfig::getArduinoDigital()
{
    QStringList _list;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_arduino.mid(0, 1) == "D")
            _list.append(info.pin_arduino);
    }

    return _list;
}

QString RTduinoConfig::getArduinoNumber(QString port, QString pin)
{
    QString arduino;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_rttport == port && info.pin_rttpin == pin)
            arduino = info.pin_arduino;
    }

    return arduino;
}

pin_info_t RTduinoConfig::getPinInfo(QString arduino)
{
    pin_info_t _pin_info;
    int index = findIndexPinInfo(arduino);
    if (index == -1)
    {
        return _pin_info;
    }

    _pin_info = all_pin_infos.at(index);

    return _pin_info;
}

QStringList RTduinoConfig::getAllDeviceName(QString device)
{
    QStringList dlist;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_function.toUpper() == device.toUpper())
        {
            if(!dlist.contains(info.pin_name))
            {
                dlist.append(info.pin_name);
            }
        }
    }

    if("SPI" == device.toUpper())
    {
        if(!dlist.contains(function_info.pwmtospi.spi_name)
                && !function_info.pwmtospi.spi_name.isEmpty())
        {
            dlist.append(function_info.pwmtospi.spi_name);
        }
    }

    qSort(dlist.begin(), dlist.end(), sortRules);

    return dlist;
}

QStringList RTduinoConfig::getDeviceChannel(QString name)
{
    QStringList dlist;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_name.toUpper() == name.toUpper())
        {
            if(!dlist.contains(info.pin_channel) && info.pin_channel.toInt() >= 0)
            {
                dlist.append(info.pin_channel);
            }
        }
    }
    qSort(dlist.begin(), dlist.end(), sortRules);

    return dlist;
}

QString RTduinoConfig::getDeviceNumber(QString name)
{
    QRegExp Rename("\\d+");

    QStringList number_list;
    int pos = 0;

    while ((pos = Rename.indexIn(name, pos)) != -1) {
          number_list << Rename.cap(0);
          pos += Rename.matchedLength();
    }

    return number_list.last();
}

QString RTduinoConfig::getDeviceIoPort(QString name, QString iomap)
{
    QString port;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_name.toUpper() == name.toUpper())
        {
            if(info.pin_iomap.toUpper() == iomap.toUpper())
            {
                port = info.pin_rttport;
                return port;
            }
        }
    }

    return port;
}

QString RTduinoConfig::getDeviceIoPin(QString name, QString iomap)
{
    QString pin;
    foreach(auto info, all_pin_infos)
    {
        if(info.pin_name.toUpper() == name.toUpper())
        {
            if(info.pin_iomap.toUpper() == iomap.toUpper())
            {
                pin = info.pin_rttpin;
                return pin;
            }
        }
    }

    return pin;
}

int RTduinoConfig::getIOrtthreadNumber(QString port, QString pin)
{
    int port_index = now_series.iopininfo.ports.indexOf(port);
    int pin_index = now_series.iopininfo.pins.indexOf(pin);

    return port_index * now_series.iopininfo.pins.size() + pin_index;
}

bool RTduinoConfig::getJsonObject(QString file, QJsonObject &json_object)
{
    QFile json_file(file);
    if (!json_file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "can't open " + file << "!";
        return false;
    }

    // 读取文件的全部内容
    QTextStream stream(&json_file);
    stream.setCodec("UTF-8");		// 设置读取编码是UTF8
    QString json_str = stream.readAll();

    json_file.close();

    // QJsonParseError类用于在JSON解析期间报告错误。
    QJsonParseError json_error;
    // 将json解析为UTF-8编码的json文档，并从中创建一个QJsonDocument。
    // 如果解析成功，返回QJsonDocument对象，否则返回null
    QJsonDocument json_doc = QJsonDocument::fromJson(json_str.toUtf8(), &json_error);
    // 判断是否解析失败
    if (json_error.error != QJsonParseError::NoError && !json_doc.isNull()) {
        qDebug() << "Json格式错误！" << json_error.error;
        return false;
    }

    json_object = json_doc.object();

    return true;
}

mcu_series_t *RTduinoConfig::findMcuSeries(QString series_name)
{
    for (int i = 0; i < all_series.size(); i++)
    {
        if (all_series[i].series.toUpper() == series_name.toUpper())
        {
            return &all_series[i];
        }
    }

    return nullptr;
}

int RTduinoConfig::findIndexPinInfo(QString arduino)
{
    for (int i = 0; i < all_pin_infos.size(); i++)
    {
        if (all_pin_infos[i].pin_arduino == arduino)
        {
            return i;
        }
    }

    qDebug() << "No have Arduino pin" << arduino;

    return -1;
}

void RTduinoConfig::praseAllMcuSeries(void)
{
    // 判断路径是否存在
    QDir dir(qApp->applicationDirPath() + MCU_STORAGE_PATH);
    if(!dir.exists())
    {
        return;
    }
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();

    int file_count = list.count();
    if(file_count <= 0)
    {
        return;
    }

    for(int i=0; i< file_count; i++)
    {
        QFileInfo file_info = list.at(i);
        QString suffix = file_info.suffix();
        if(QString::compare(suffix, QString("json"), Qt::CaseInsensitive) == 0)
        {
            QString absolute_file_path = file_info.absoluteFilePath();
            if (absolute_file_path.contains("mcu_series"))
                parseMcuSeriesJson(absolute_file_path);
        }
    }
}

void RTduinoConfig::sortPinInfo()
{
    if(all_pin_infos.isEmpty())
        return;

    // 把Ax移动到Dx后面
    foreach (auto pininfo, all_pin_infos) {
        if(pininfo.pin_function == "ADC")
        {
            all_pin_infos.move(findIndexPinInfo(pininfo.pin_arduino), all_pin_infos.size() - 1);
        }
    }
    foreach (auto pininfo, all_pin_infos) {
        if(pininfo.pin_channel == "VREFINT")
        {
            all_pin_infos.move(findIndexPinInfo(pininfo.pin_arduino), all_pin_infos.size() - 1);
        }
    }
    foreach (auto pininfo, all_pin_infos) {
        if(pininfo.pin_channel == "TEMPSENSOR")
        {
            all_pin_infos.move(findIndexPinInfo(pininfo.pin_arduino), all_pin_infos.size() - 1);
        }
    }
    // 把DACx移动到Ax后面
    foreach (auto pininfo, all_pin_infos) {
        if(pininfo.pin_function == "DAC")
        {
            all_pin_infos.move(findIndexPinInfo(pininfo.pin_arduino), all_pin_infos.size() - 1);
        }
    }
}

void RTduinoConfig::updatePinInfoArduino()
{

    int Anum = 0,Dnum = 0,DACnum = 0;
    if(all_pin_infos.isEmpty())
        return;

    for(int i = 0; i < all_pin_infos.size(); i++) {
        if(all_pin_infos[i].pin_function == "ADC")
        {
            all_pin_infos[i].pin_arduino = "A" + QString::number(Anum++);
        }
        else
        {
            if(all_pin_infos[i].pin_function == "DAC")
            {
                all_pin_infos[i].pin_arduino = "DAC" + QString::number(DACnum++);
            }
            else
            {
                all_pin_infos[i].pin_arduino = "D" + QString::number(Dnum++);
            }
        }
    }
}

void RTduinoConfig::createPinInfoNote()
{
    if(all_pin_infos.isEmpty())
        return;

    for(int i = 0; i < all_pin_infos.size(); i++) {
        if(all_pin_infos[i].pin_function == "GPIO")
        {
            if(function_info.spiss == all_pin_infos[i].pin_arduino)
            {
                all_pin_infos[i].pin_notes = "/* SPI-SS */";
            }
            if(function_info.ledpin == all_pin_infos[i].pin_arduino)
            {
                all_pin_infos[i].pin_notes = "/* LED_BUILTIN */";
            }
        }
        if(all_pin_infos[i].pin_function == "UART")
        {
            if(function_info.serial2 == all_pin_infos[i].pin_name)
            {
                all_pin_infos[i].pin_notes = "/* Serial2-" + all_pin_infos[i].pin_iomap + " */";
            }
            else if(function_info.serial3 == all_pin_infos[i].pin_name)
            {
                all_pin_infos[i].pin_notes = "/* Serial3-" + all_pin_infos[i].pin_iomap + " */";
            }
            else
            {
                all_pin_infos[i].pin_notes = "/* Serial-" + all_pin_infos[i].pin_iomap + " */";
            }
        }
        if(all_pin_infos[i].pin_function == "PWM")
        {
            all_pin_infos[i].pin_notes = "/* PWM */";
        }
        if(all_pin_infos[i].pin_function == "ADC")
        {
            if(all_pin_infos[i].pin_channel == "VREFINT")
            {
                all_pin_infos[i].pin_notes = "/* ADC, On-Chip: internal reference voltage, ADC_CHANNEL_VREFINT */";
            }
            else if(all_pin_infos[i].pin_channel == "TEMPSENSOR")
            {
                all_pin_infos[i].pin_notes = "/* ADC, On-Chip: internal temperature sensor, ADC_CHANNEL_TEMPSENSOR */";
            }
            else
            {
                all_pin_infos[i].pin_notes = "/* ADC */";
            }
        }
        if(all_pin_infos[i].pin_function == "DAC")
        {
            all_pin_infos[i].pin_notes = "/* DAC */";
        }
        if(all_pin_infos[i].pin_function == "USB")
        {
            all_pin_infos[i].pin_notes = "/* SerialUSB-" + all_pin_infos[i].pin_iomap + " */";
        }
        if(all_pin_infos[i].pin_function == "I2C")
        {
            all_pin_infos[i].pin_notes = "/* I2C-" + all_pin_infos[i].pin_iomap + " (Wire) */";
        }
        if(all_pin_infos[i].pin_function == "SPI")
        {
            all_pin_infos[i].pin_notes = "/* SPI-" + all_pin_infos[i].pin_iomap + " */";
        }
    }
}

void RTduinoConfig::fullPinInfo()
{
    sortPinInfo();
    updatePinInfoArduino();
    createPinInfoNote();
}

bool sortRules(QString &p1, QString &p2)
{
    QRegExp p1rx("\\d+");
    QRegExp p2rx("\\d+");
    p1rx.indexIn(p1, 0);
    p2rx.indexIn(p2, 0);

    int _p1 = p1rx.cap(0).toInt();
    int _p2 = p2rx.cap(0).toInt();

    return (bool)(_p1 < _p2);
}
