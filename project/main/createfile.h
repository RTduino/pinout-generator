#ifndef CREATEFILE_H
#define CREATEFILE_H

#include <QDateTime>
#include <rtduinoconfig.h>

class CreateFile
{
public:
    CreateFile();
    bool createSConscript();

    bool creatPinoutDir();
    bool creatImage();
    bool creatPinoutCFile();
    QString createPwm2Spi();
    bool creatPinoutHFile();
    bool creatMainCppFile();
    bool creatKconfigFile();
    bool creatReadmeFile();
private:

    void updateRTThreadNoteInfo(void);

    RTduinoConfig *rtduino;

    QString localTime;
    QString year;
    QString month;
    QString day;
    QString author;
};

#endif // CREATEFILE_H
