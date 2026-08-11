|# StageShowSimulator Requirements

## 1. 文書の目的

この文書は、StageShowSimulatorの機能要求、非機能要求、外部インターフェース、受入条件、および対象外範囲を定義する。

この文書はプロジェクト全体の要求仕様を表す。

個別機能の実装順序、変更ファイル、実装方法、進捗状況は、Docs/Plans配下の計画文書で管理する。

## 2. プロジェクト概要

StageShowSimulatorは、Unreal Engine 5.7上でライブイベントのステージ演出をシミュレーションするWindowsアプリケーションである。

本システムは、次の要素を同一の仮想ステージ上で再現する。

1. QLC+からArt-Netで制御される照明機材
2. 時刻付き2次元位置姿勢データに従って移動する台車ロボット
3. 台車ロボット上に搭載されたLEDディスプレイ
4. LEDディスプレイ上のMP4動画再生
5. ロボット移動と動画再生を制御する共通ShowClock

## 3. 開発対象

### 3.1 Must構成

同一のWindows PC A上で、次を実行する。

- Unreal Engine 5.7
- StageShowSimulator
- QLC+
- Loopback Art-Net通信
- Robot JSON再生
- MP4再生
- ShowClock制御

QLC+からUnreal EngineへのArt-Net通信には、原則として127.0.0.1を使用する。

### 3.2 Want構成

将来、次の分離構成へ拡張可能にする。

- PC AでUnreal Engineを実行する
- PC BでQLC+を実行する
- PC BからPC AへLAN経由でArt-Netを送信する
- PC AにENTTEC DMX USB Pro 70304を接続する
- PC AでArt-Netから物理DMXへのBridge処理を行う

### 3.3 実装優先順位

実装は次の順序で進める。

1. 同一PCのLoopback Art-Net
2. Robot JSON
3. MP4
4. ShowClockとの統合
5. 統合テスト
6. PC B分離
7. ENTTEC Bridge

既に実装済みの機能が順序より先行していても、各段階の受入条件を満たしていれば継続利用できる。

## 4. 対象環境

### 4.1 実行環境

- OS: Windows 11
- Unreal Engine: 5.7
- IDE: Visual Studio 2022
- Build Configuration: Development Editor
- Platform: Win64
- Source Control: Git
- Repository: GitHub private repository

### 4.2 開発支援

- GitHub Copilotを使用する
- Visual Studioの質問モードを設計、調査、レビューに使用する
- Visual Studioのエージェントモードを実装、ビルド、テストに使用する
- C++実装を主な自動生成対象とする
- uassetはGitHub Copilotによる直接生成および直接編集の対象外とする

## 5. システム構成

基本データフローは次のとおりとする。

QLC+
→ Art-Net UDP
→ Art-Net Receiver
→ Lighting Fixture Component
→ Unreal Engine Light Component

Robot JSON
→ Robot Pose Loader
→ Robot Pose Track
→ Robot Pose Playback Component
→ Robot Actor Transform

MP4
→ Unreal Media Framework
→ Media Player
→ Media TextureまたはMedia Plate
→ LED Display Mesh

ShowClock
→ Robot Pose Playback Component
→ Media Playback Component

## 6. ShowClock機能要求

### 6.1 基本要求

ShowClockは、Robot MotionおよびMedia Playbackが参照する唯一のショー時刻源とする。

ShowClockは少なくとも次の操作を提供する。

- Play
- Pause
- Stop
- Seek
- GetCurrentTime

### 6.2 状態

ShowClockは少なくとも次の状態を扱う。

- Stopped
- Playing
- Paused
- Seeking
- Error

実装上、Seekingを永続状態として保持しない場合は、Seek操作中の一時的な状態として扱ってよい。

### 6.3 時刻

- 内部時刻は秒単位で保持する
- 時刻の型はdoubleを使用する
- 負の時刻は0秒へ制限する
- 終端時刻が設定されている場合、終端を超える時刻は定義された境界動作に従う
- Stop実行後は0秒へ戻る
- Pause中は時刻を進めない
- Playing中のみ時刻を進める

### 6.4 禁止事項

- Robot Motion Componentが独自の再生時計を持たない
- Media Playback Componentが独自のショー時計を持たない
- JSONをTickごとに再読み込みしない
- 毎フレームMedia PlayerへSeekしない

## 7. Robot Motion機能要求

### 7.1 入力

台車ロボットの移動データは、時刻付きの2次元位置姿勢データを含むJSONファイルで与える。

各サンプルは少なくとも次を含む。

- 時刻
- X位置
- Y位置
- Yaw角度

### 7.2 単位

- JSONの時刻単位は秒
- JSONの位置単位はメートル
- Unreal Engine内の位置単位はセンチメートル
- JSONの回転単位は度
- YawはUnreal EngineのZ軸回転へ変換する

### 7.3 JSONの論理スキーマ

JSONの論理形式は次のとおりとする。

```json
{
  "schemaVersion": "1.0",
  "timeUnit": "seconds",
  "positionUnit": "meter",
  "rotationUnit": "degree",
  "samples": [
    {
      "time": 0.0,
      "x": 0.0,
      "y": 0.0,
      "yaw": 0.0
    }
  ]
}