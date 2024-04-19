# 如何参与此项目的开发

推荐使用如下流程进行开发，对于具备Qt桌面安装环境不做要求，但是也可以使用此流程进行开发。

目前仅支持使用PowerShell终端进行开发，不支持使用其他终端进行开发。

## 1 安装打包工具NSIS

在NSIS官网下载对应的安装包安装即可：[Download - NSIS (sourceforge.io)](https://nsis.sourceforge.io/Download)

安装完成后，请将makensis.exe所在目录`C:\Program Files (x86)\NSIS\Bin`添加至环境变量。

## 2 项目开发(源码修改)

不做具体要求，根据实际情况修改即可。

## 3 修改项目版本信息

这里为大家提供了`version.ps1`脚本供大家一键修改工程中所有的版本相关信息。

### 3.1 仅修改数字版本号

```powershell
.\scripts\version.ps1 -version x.x.x
```

### 3.2 修改版本号和版本类型

目前仅支持Alpha,Beta,Release三种版本类型。

```powershell
.\scripts\version.ps1 -version x.x.x -type Alpha
.\scripts\version.ps1 -version x.x.x -type Beta
.\scripts\version.ps1 -version x.x.x -type Release
```

## 4 配置相关编译环境

这里为大家提供了`settings.ps1`脚本供大家配置编译环境相关信息。

### 4.1 无Qt桌面开发环境

执行此命令后会从 [Release qt-mingw-tool · liYony/qt-mingw-tool (github.com)](https://github.com/liYony/qt-mingw-tool/releases/tag/v1.0.0) 网站上去下载`qt-ming.zip`压缩包，并将其解压到本项目的tools/qt-mingw目录下。并且配置相关的环境变量。

由于该压缩包大小约为1.48G，使用PowerShell下载可能不是特别稳定，大家也可以自行下载并解压到tools/qt-mingw目录下。

```powershell
.\scripts\settings.ps1
```

### 4.2 有Qt桌面开发环境

如果系统具备Qt的桌面开发环境，我们将不需要在去下载qt-mingw，这里我们只需要导出相关的环境变量即可。

```powershell
.\scripts\settings.ps1 -sysqt F:\Qt\5.12.12\mingw73_64\bin F:\Qt\Tools\mingw730_64\bin
```

执行上述示例命令即可，第一个路径为Qt的mingw路径，里面具有qmake.exe这样的可执行文件。第二个路径则为mingw路径，里面具有gcc.exe这样的可执行文件。

## 5 编译源码

这里为大家提供了`mingqt.ps1`脚本供大家用于编译工程源码。

### 5.1 配置qmake工程并编译

```powershell
.\scripts\mingwqt.ps1
```

### 5.2 仅配置qmake工程

```powershell
.\scripts\mingwqt.ps1 -config
```

### 5.3 仅编译qmake工程

```powershell
.\scripts\mingwqt.ps1 -build
```

### 5.3 清除qmake工程

```powershell
.\scripts\mingwqt.ps1 -clean
```

## 6 打包安装包

这里为大家提供了`software.ps1`脚本供大家用于打包安装包。

### 6.1 直接打包(不会更新库文件)

```powershell
.\scripts\software.ps1
```

### 6.2 重新打包(会更新库文件)

```powershell
.\scripts\software.ps1 -repack
```

### 6.3 指定exe文件路径进行打包

参考前两种，这里可以使用`-exefile`来指定可执行文件的路径，对于使用桌面开发的用户来说是比较实用的，毕竟exe文件存放路径是不确定。

```powershell
.\scripts\software.ps1 -repack -exefile my_exe_file_path
```
