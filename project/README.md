# 工程主要文件介绍

## filein.cpp

完成了对pins_arduino.c文件的解析。得到pin_map_table结构体的每一条内容。

## fileout.cpp

通过界面的数据，生成pins_arduino.c，pins_arduino.和，Kconfig，Sconscript四个文件。

## mainUI.cpp

完成对界面上一些数据进行刷新，包括table表格，下拉框内容刷新等。

## pininfoui.cpp

主要是对子页面(添加，插入，修改)的处理。

## pinmap.cpp

所有的引脚数据都是在一个链表里面，这个文件里面主要是对这里面的数据进行处理，包括查找，删除，添加等。

## widget.cpp

主要是对界面的一些事件进行反应。

