#ifndef FILECONTENT_H
#define FILECONTENT_H

#define FILE_RTT_LICENSE_PART \
"/*\n"\
" * Copyright (c) 2006-"+sinfo.year+", RT-Thread Development Team\n"\
" *\n"\
" * SPDX-License-Identifier: Apache-2.0\n"\
" *\n"\
" * Change Logs:\n"\
" * Date           Author       Notes\n"\
" * "+sinfo.year+"-"+sinfo.month+"-"+sinfo.day+sinfo.autor+"\n"\
" */\n"

#define FILEC_RTT_INCLUDE \
"#include <Arduino.h>\n"\
"#include <board.h>\n"\
"#include \"pins_arduino.h\"\n"\
"\n"

#define FILEC_RTDUINO_NOTES \
"/*\n"\
"    {Arduino Pin, RT-Thread Pin [, Device Name(PWM or ADC), Channel]}\n"\
"    [] means optional\n"\
"    Digital pins must NOT give the device name and channel.\n"\
"    Analog pins MUST give the device name and channel(ADC, PWM or DAC).\n"\
"    Arduino Pin must keep in sequence.\n"\
"*/\n"

#define FILEH_HEADER \
"#ifndef Pins_Arduino_h\n"\
"#define Pins_Arduino_h\n"\
"\n"\
"/* pins alias. Must keep in sequence */\n"

#define FILEH_END \
"#endif /* Pins_Arduino_h */\n"

#define FILESCONS_CODE \
"from building import *\n"\
"\n"\
"cwd = GetCurrentDir()\n"\
"src = Glob('*.c') + Glob('*.cpp')\n"\
"inc = [cwd]\n"\
"\n"\
"group = DefineGroup('Arduino', src, depend = ['PKG_USING_RTDUINO'], CPPPATH = inc)\n"\
"\n"\
"Return('group')\n"

#endif // FILECONTENT_H
