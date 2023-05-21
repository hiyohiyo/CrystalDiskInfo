[英語](./README.md) | 日本語 | [中国語（簡体字）](./README.sc.md)

# CrystalDiskInfo

## 概要
CrystalDiskInfo はディスク情報ツールです。

CrystalDiskInfo は以下に対応しています:
- PATA ドライブ
- SATA ドライブ

以下には部分的に対応しています:
- USB ドライブ
- RAID コントローラ (IRST)
- NVMe ドライブ
- RAID コントローラ (Intel(R) VROC)
- RAID コントローラ (AMD-RAIDXpert2 *AMD_RC2t7が必要*)
- USB RAID コントローラ (JMicron JMS56X,JMB39X)

## 通知
このリポジトリにはリソースファイルは含まれていないので、以下のリンクからダウンロードして下さい。
- [ダウンロードページへのリンク](https://crystalmark.info/redirect.php?product=CrystalDiskInfo)

## ビルド

*DiskInfo.sln* を開いてビルドします。

### ノート
[ダウンロードファイル](https://crystalmark.info/redirect.php?product=CrystalDiskInfo) にある *CdiResource* フォルダーをビルド時に作られた *Rugenia* フォルダーへコピーします。実行時に *CdiResource* フォルダーがない場合、「Not Found "Graph.html"」と表示されます。

[AMD_RC2t7](https://thilmera.com/project/AMD_RC2t7/)は[弦生ささとさん](https://twitter.com/thilmera7)によって開発されたAMD RAIDのSMART情報(SATA/NVMe)を取得する機能を提供するライブラリです。

## プロジェクトのページ
- [Crystal Dew World](https://crystalmark.info/)

## ライセンス
MIT ライセンス
- [日本語版](https://crystalmark.info/ja/software/crystaldiskinfo/crystaldiskinfo-license/)
- [英語版](https://crystalmark.info/en/software/crystaldiskinfo/crystaldiskinfo-license/)
