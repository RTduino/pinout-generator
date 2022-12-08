#ifndef FILEINFO_H
#define FILEINFO_H

#define FILE_RTT_LICENSE_PART \
"/*\n"\
" * Copyright (c) 2006-"+dateinfo->year+", RT-Thread Development Team\n"\
" *\n"\
" * SPDX-License-Identifier: Apache-2.0\n"\
" *\n"\
" * Change Logs:\n"\
" * Date           Author       Notes\n"\
" * "+dateinfo->year+"-"+dateinfo->month+"-"+dateinfo->day+dateinfo->autor+"\n"\
" */\n\n"

#define FILESCONS_CODE \
"from building import *\n"\
"\n"\
"cwd = GetCurrentDir()\n"\
"src = Glob('*.c') + Glob('*.cpp')\n"\
"inc = [cwd]\n"\
"\n"\
"group = DefineGroup('RTduino', src, depend = ['PKG_USING_RTDUINO'], CPPPATH = inc)\n"\
"\n"\
"Return('group')\n"
#endif // FILEINFO_H
