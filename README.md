English | [Japanese](./README.ja.md) | [Chinese (Simplified)](./README.sc.md)

# CrystalDiskInfo

## Overview
CrystalDiskInfo is a disk information tool.

CrystalDiskInfo supports:
- PATA drives
- SATA drives

Partially supports:
- USB drives
- RAID controllers (IRST)
- NVMe drives
- RAID controllers (Intel(R) VROC)
- RAID controllers (AMD-RAIDXpert2 *Requires AMD_RC2t7*)
- USB RAID controllers (JMicron JMS56X,JMB39X)

## Notification
This repository does not include resource files, so please download them from below.
- [Download for resource files](https://crystalmark.info/redirect.php?product=CrystalDiskInfo)

## Building

Open *DiskInfo.sln* and build.

### Note
Copy *CdiResource* folder in the [Download CdiResource](https://crystalmark.info/redirect.php?product=CrystalDiskInfo) to *../Rugenia* folder created in the build. If the *CdiResource* folder does not exist at runtime, the app displays "*Not Found 'Graph.html'*".

[AMD_RC2t7](https://thilmera.com/project/AMD_RC2t7/) is a library that provides functionality to acquire SMART information (SATA/NVMe) of AMD RAID. It is developed by [Gakuto Matsumura](https://twitter.com/thilmera7).

## Project Page
- [Crystal Dew World](https://crystalmark.info/)

## License
The MIT License
- [Japanese version](https://crystalmark.info/ja/software/crystaldiskinfo/crystaldiskinfo-license/)
- [English version](https://crystalmark.info/en/software/crystaldiskinfo/crystaldiskinfo-license/)
