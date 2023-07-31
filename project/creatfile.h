#ifndef CREATFILE_H
#define CREATFILE_H

#include <QString>
#include "pinsmap.h"

class CreatFile
{
public:
    CreatFile();

    bool creatAllFiles();
    void setProjectPath(QString path);
    bool creatPinoutDir();

protected:
    void getNeedUiInfo();
    bool creatPinoutCFile();
    bool creatPinoutHFile();
    bool creatMainCppFile();
    bool creatSconsFile();

    bool creatKconfigFile();
    bool creatReadmeFile();

private:

    PinsMap *nowMcu;
    QString projPath;

    QString localTime;
    QString year;
    QString month;
    QString day;
    QString author;
};

#endif // CREATFILE_H
