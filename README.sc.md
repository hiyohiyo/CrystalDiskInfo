[英语](./README.md) | [日语](./README.ja.md) | 简体中文

# CrystalDiskInfo

## 简介
CrystalDiskInfo 是一款查看硬盘信息状况的工具。

CrystalDiskInfo 支持:
- PATA 设备
- SATA 设备

仅部分功能支持:
-USB驱动器
- RAID控制器（IRST）
- NVMe驱动器
- RAID控制器(Intel(R) VROC)
- RAID控制器(AMD-RAIDXpert2 *需要AMD_RC2t7*)
- USB RAID控制器(JMicron JMS56X,JMB39X)
## 小贴士
这个仓库不包括资源文件，请从点击下方链接下载。
- [下载资源文件](https://crystalmark.info/redirect.php?product=CrystalDiskInfo)

## 编译

打开 *DiskInfo.sln* 文件并编译。

### 注意
将[（下载 CdiResource）](https://crystalmark.info/redirect.php?product=CrystalDiskInfo)中的*CdiResource*文件夹复制到编译时创建的*../Rugenia*文件夹中。如果*CdiResource*文件夹在运行时不存在，应用程序会显示 "*Not Found 'Graph.html'"。

[AMD_RC2t7](https://thilmera.com/project/AMD_RC2t7/)是一个提供获取AMD RAID的SMART信息（SATA/NVMe）的功能的库，它由[佐佐木玄策](https://twitter.com/thilmera7)开发。

## 本项目的网站
- [Crystal Dew World](https://crystalmark.info/)

## 开源协议
本开源项目使用MIT开源协议
- [日语版本](https://crystalmark.info/ja/software/crystaldiskinfo/crystaldiskinfo-license/)
- [英语版本](https://crystalmark.info/en/software/crystaldiskinfo/crystaldiskinfo-license/)
