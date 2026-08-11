# StageShowSimulator 調査・実装計画

## 前提

- 本書は現状調査と実装計画のみを扱う。
- コード、設定ファイル、プロジェクトファイルは変更していない。
- ターミナルは実行していない。
- `Docs/Requirements.md` はワークスペース内で一意に取得できなかったため、主に `.github/copilot-instructions.md` と現行プロジェクト構成を基に整理している。
- UE 5.7 で存在確認できない API は採用候補に含めない。

---

## 1. 現在のプロジェクト構成

### ルート構成
確認できた主要構成は以下。

- `StageShowSimulator.uproject`
- `Source/`
  - `StageShowSimulator/`
    - `StageShowSimulator.Build.cs`
    - `StageShowSimulator.cpp`
    - `StageShowSimulator.h`
  - `StageShowSimulator.Target.cs`
  - `StageShowSimulatorEditor.Target.cs`
- `Config/`
  - `DefaultEngine.ini`
  - `DefaultGame.ini`
  - `DefaultEditor.ini`
- `.github/`
  - `copilot-instructions.md`

### 現状の実装状態
- C++ Runtime モジュール 1本の最小構成。
- `StageShowSimulator.cpp` は `IMPLEMENT_PRIMARY_GAME_MODULE(...)` のみ。
- ゲーム固有の Actor、Component、Subsystem、Test は未確認。
- 自動テスト用の専用モジュールや `Tests` ディレクトリは未確認。
- `.uproject` 上は Editor 向け Plugin が 1件有効。

### 設定上の特徴
- Engine Association は `5.7`。
- Target は `Game` / `Editor` の2種。
- `DefaultEngine.ini` では `GameDefaultMap=/Engine/Maps/Templates/OpenWorld`。
- Windows 向けは DX12 / SM6 構成。
- `CommonUI` 設定が `DefaultGame.ini` に存在。

---

## 2. Unreal Engine モジュール名

### 現在のモジュール
- `StageShowSimulator`

### 根拠
- `StageShowSimulator.uproject`
  - `Modules[0].Name = "StageShowSimulator"`
- `Source/StageShowSimulator.Target.cs`
  - `ExtraModuleNames.Add("StageShowSimulator");`
- `Source/StageShowSimulatorEditor.Target.cs`
  - `ExtraModuleNames.Add("StageShowSimulator");`

### モジュール種別
- Type: `Runtime`
- LoadingPhase: `Default`

---

## 3. Build.cs の現在の依存関係

`Source/StageShowSimulator/StageShowSimulator.Build.cs` の現状:

### PublicDependencyModuleNames
- `Core`
- `CoreUObject`
- `Engine`
- `InputCore`
- `EnhancedInput`

### PrivateDependencyModuleNames
- なし

### コメントアウトされている候補
- `Slate`
- `SlateCore`
- `OnlineSubsystem`

### 所見
今後の要件を満たすには、段階的に以下の UE 標準モジュール追加を検討する余地がある。

- Art-Net / UDP 受信:
  - `Sockets`
  - `Networking`
- JSON 読み込み:
  - `Json`
  - `JsonUtilities`
- MP4 / Media 再生:
  - `MediaAssets`
  - 必要に応じて `MediaUtils` 系
- 自動テスト:
  - Runtime test の配置方針に応じて `Core`, `CoreUObject`, `Engine` を維持
  - 専用 Test モジュール化する場合は別途設計

※ ただし本計画では、**実際に UE 5.7 上で確認できた標準モジュールのみ採用対象**とする。

---

## 4. 有効になっている Plugin

`StageShowSimulator.uproject` で明示的に有効なのは以下。

### 明示的に有効
- `ModelingToolsEditorMode`
  - `Enabled = true`
  - `TargetAllowList = ["Editor"]`

### 追加所見
- これは Editor 限定 Plugin。
- Runtime 機能には直接関与しない。
- `.uproject` に DMX、Media、Networking 系 Plugin の明示追加は未確認。

### 注意
MP4 再生や DMX 連携は、UE 標準機能・標準 Plugin の利用可否を **UE 5.7 実機環境で確認してから** 採用判断する。

---

## 5. 開発時のビルド方法

`.github/copilot-instructions.md` の前提:
- IDE: Visual Studio 2022 17.14 以降
- Build configuration: `Development Editor / Win64`

### 推奨ビルド単位
1. 小さな実装単位で変更
2. その都度 `Development Editor / Win64` でビルド
3. 成功後、関連自動テストを実行

### 想定ビルド対象
- `StageShowSimulatorEditor` Target
- 構成:
  - `Development Editor`
  - `Win64`

### Visual Studio での想定操作
- ソリューション生成済みであれば Visual Studio からビルド
- Unreal Editor 利用時は C++ 変更後に再コンパイルまたは IDE ビルド
- 必要に応じて __Build__ / __Rebuild__ を使い分ける

### 補足
現時点では `.sln` の存在を確認できていないため、**標準的な UE C++ プロジェクトの Editor Target ビルド運用**として記載している。

---

## 6. 現在の自動テストの有無

### 調査結果
- `IMPLEMENT_SIMPLE_AUTOMATION_TEST`
- `BEGIN_DEFINE_SPEC`
- `AutomationSpec`
- `Tests` 相当のプロジェクト内実装

上記は現行プロジェクト側では確認できなかった。

### 結論
- **現時点では自動テスト未実装と判断**
- 少なくとも、確認できた `Source/StageShowSimulator/` 直下には test code は存在しない

### 要件上の不足
`.github/copilot-instructions.md` の testing rules では以下が要求される。

- Unreal Automation Test Framework を使用
- JSON validation test
- interpolation test
- time seeking / boundary conditions test
- DMX hardware 非依存
- MP4 decoder 非依存

---

## 7. 今後追加するクラス候補

設計ルール:
- ShowClock を単一の再生時間ソースにする
- JSON は再生前ロード・検証
- パースと Actor 移動を分離
- 再利用可能な振る舞いは `ActorComponent` 優先
- Runtime modules は `UnrealEd` 非依存

以上を満たす候補。

### A. ShowClock 系
- `UShowClockComponent`
  - 再生時間の単一ソース
  - Play / Pause / Stop / Seek
  - 再生速度、現在時刻、終端判定
- `FShowClockState`
  - 非 UObject の軽量状態保持用構造体候補

### B. Art-Net / DMX 受信系
- `UArtNetReceiverComponent`
  - UDP loopback 受信
  - Universe 単位の最新 DMX バッファ保持
- `FArtNetPacket`
  - Art-Net packet 解析用構造体
- `FArtNetDmxFrame`
  - Universe / Sequence / Payload を保持
- `ULightingFixtureComponent`
  - DMX 値を受けてライト状態へ反映
- `AStageLightActor`
  - 視覚表現を持つ fixture Actor 候補

### C. Robot JSON 系
- `FRobotPoseSample`
  - `TimeSeconds`, `Position2D`, `YawDegrees`
- `FRobotPoseTrack`
  - サンプル配列と検証関数
- `URobotPoseDataAsset` は現時点では保留
  - 今回は JSON 入力が主なので必須ではない
- `URobotPoseLoader`
  - JSON 読み込み・検証
- `URobotPosePlaybackComponent`
  - ShowClock 時刻から補間済み姿勢を取得
- `ARobotDisplayActor`
  - 画面表示・位置反映対象

### D. MP4 / Media 系
- `ULedMediaPlaybackComponent`
  - Media 再生制御
  - ShowClock と同期
- `ALedScreenActor`
  - LED 表示面を持つ Actor
- `FMediaCueRange`
  - 再生区間や同期制御を管理する補助構造体候補

### E. 統合制御系
- `UStageShowCoordinatorComponent`
  - ShowClock、Robot、Media、DMX の接続点
  - BeginPlay / EndPlay の依存初期化順を管理
- `AStageShowController`
  - レベル上で手動配置しやすい統合 Actor 候補

### F. テスト系
- `FRobotPoseJsonValidationTest`
- `FRobotPoseInterpolationTest`
- `FShowClockSeekTest`
- `FArtNetPacketParseTest`
- `FStageShowSyncLogicTest`

### 命名方針
UE 慣例に合わせて:
- UObject 派生: `U`
- Actor 派生: `A`
- Plain struct: `F`

---

## 8. 実装順序

指定された順序を維持しつつ、依存関係に沿って細分化する。

### 第1段階: 同一PCのLoopback Art-Net
目的:
- QLC+ と同一 PC 上で UDP loopback により Art-Net DMX を受信
- Universe 単位で値取得できる状態にする

実装候補:
- `UArtNetReceiverComponent`
- `FArtNetPacket`
- `FArtNetDmxFrame`
- `ULightingFixtureComponent`
- 専用ログカテゴリ

### 第2段階: Robot JSON
目的:
- タイムスタンプ付き 2D pose JSON を事前読込・検証
- 補間可能な内部データへ変換

実装候補:
- `FRobotPoseSample`
- `FRobotPoseTrack`
- `URobotPoseLoader`
- `URobotPosePlaybackComponent`

### 第3段階: MP4
目的:
- LED 表示用の MP4 再生経路を用意
- ただし同期基準はまだ仮でもよい

実装候補:
- `ULedMediaPlaybackComponent`
- `ALedScreenActor`

### 第4段階: ShowClock
目的:
- Robot / Media / 将来の DMX 演出同期の単一時刻源を導入
- Play / Pause / Seek / Stop を一元化

実装候補:
- `UShowClockComponent`
- `UStageShowCoordinatorComponent`

### 第5段階: 統合テスト
目的:
- JSON validation
- 補間
- seek / boundary
- 同期ロジック
を自動テスト化

実装候補:
- Automation Test 群
- 可能なら Runtime 寄りのロジックを純粋関数化して検証容易化

### 第6段階: PC B分離
目的:
- 受信元/再生先の分離を想定した構成整理
- loopback 前提から LAN 前提へ拡張可能にする

実装候補:
- Receiver の bind / target 設定分離
- 設定構造体化
- ネットワーク依存の抽象化

### 第7段階: ENTTEC Bridge
目的:
- 将来の実機 Bridge 利用に備えた I/O 境界を整理
- ただし現時点ではハード依存テストを避ける

実装候補:
- DMX 入出力アダプタ境界
- 実機未接続でも動作する mockable な受信経路

---

## 9. 各段階の受入条件

### 第1段階: 同一PCのLoopback Art-Net
受入条件:
- UE 起動中に loopback 上の UDP 受信が開始される
- Art-Net DMX packet を受信し、Universe とチャンネル値を抽出できる
- 受信失敗時にクラッシュしない
- fixture へ DMX 値を反映できる最小経路がある
- `UnrealEd` 依存が Runtime モジュールに入らない

### 第2段階: Robot JSON
受入条件:
- JSON を Tick ごとに parse しない
- 再生前に JSON をロードし、妥当性検証できる
- 不正 JSON / 欠損キー / 時刻逆転 / 空配列を検出できる
- 任意時刻に対して pose 補間結果を返せる
- Actor 移動ロジックと JSON パースが分離されている

### 第3段階: MP4
受入条件:
- UE 5.7 で存在確認できる標準 Media API のみで構成する
- MP4 の再生開始・停止・状態確認ができる
- 失敗時にログが出る
- デコーダ依存のテストを要求しない
- 画面表示コンポーネントと再生制御が分離されている

### 第4段階: ShowClock
受入条件:
- ShowClock が唯一の再生時間ソースになる
- Play / Pause / Stop / Seek を提供する
- Robot と Media が ShowClock 時刻を参照する
- 時刻 0、終端、範囲外 seek の境界挙動が定義される

### 第5段階: 統合テスト
受入条件:
- Automation Test Framework で実行できる
- JSON validation test がある
- interpolation test がある
- seek / boundary test がある
- DMX hardware 非依存
- MP4 decoder 非依存
- ロジックテストが headless に近い条件で成立する

### 第6段階: PC B分離
受入条件:
- loopback 固定実装になっていない
- IP / port / bind 設定を差し替え可能
- 単一 PC と別 PC の両方を想定できる
- 既存 loopback 動作を壊さない

### 第7段階: ENTTEC Bridge
受入条件:
- Bridge 導入時の境界が明確
- 実機未接続でもビルド・基本動作に影響しない
- 実機依存コードが中核ロジックへ侵食しない
- 切替不能な密結合を作らない

---

## 10. 人が Unreal Editor で操作する項目

自動化しにくい、または Editor 上での確認が必要な項目。

### レベル・Actor 配置
- `AStageShowController` など統合 Actor のレベル配置
- `ARobotDisplayActor` の配置
- `ALedScreenActor` の配置
- ライト Actor / fixture Actor の配置

### アセット関連
- Media Source / Media Player / Material の関連付け確認
- LED スクリーン用 Mesh / Material の適用
- JSON 入力ファイルの配置方針確認
- レベルの既定マップや再生対象レベルの選定

### Blueprint / Details 設定
- 受信 IP / Port / Universe
- JSON ファイルパス
- 再生開始設定
- ShowClock 自動再生有無
- Actor / Component 間参照の接続

### 動作確認
- PIE で Art-Net 受信確認
- Robot の移動確認
- MP4 の画面表示確認
- Play / Pause / Seek の Editor 上検証

---

## 11. Copilot Agent へ任せられる項目

### 設計・コード生成
- Runtime クラス雛形の追加
- `Build.cs` の依存追加提案
- JSON パーサ実装
- 補間ロジック実装
- ShowClock 実装
- Art-Net packet parser 実装
- ログカテゴリ追加
- Blueprint 公開用 `UFUNCTION` / `UPROPERTY` の整理

### テスト実装
- Automation Test 雛形作成
- JSON validation test
- interpolation test
- seek / boundary test
- parser 単体テスト

### リファクタリング
- Actor と parsing ロジックの分離
- Component 化
- 中核ロジックの pure 化
- 依存方向の整理

### ドキュメント
- 実装段階ごとの作業計画更新
- 受入条件チェックリスト化
- Editor 操作手順書の下書き

### 任せない方がよい項目
- 実機配線判断
- QLC+ 側ネットワーク設定の最終確定
- Media asset の見た目調整
- 実 Editor 上での視覚検証
- UE 5.7 に存在未確認の API 採用判断

---

## 12. リスクと切り戻し方法

### リスク1: UE 5.7 非対応 API を採用してしまう
対策:
- UE 標準 API / Module のみ採用
- 不確実な API は計画から除外
- 導入前に UE 5.7 で存在確認

切り戻し:
- 該当機能を境界クラスごと無効化
- フォールバック実装へ戻す
- 機能単位ブランチで差し戻す

### リスク2: Runtime モジュールに Editor 依存が混入する
対策:
- `UnrealEd` 非依存を厳守
- Editor 専用処理は分離
- `Build.cs` 追加時に依存レビュー

切り戻し:
- 追加依存を削除
- Editor 機能を別モジュール化、または一時撤去

### リスク3: JSON を Tick ごとに parse する設計になる
対策:
- 事前ロード・事前検証・内部キャッシュ化
- 補間専用データ構造を分離

切り戻し:
- parser と playback component を分離し直す
- Tick 内処理を lookup のみに戻す

### リスク4: ShowClock 以外の時刻源が混在する
対策:
- Robot / Media / DMX 演出の参照時刻を ShowClock に統一
- 各 component で独自時計を持たせない

切り戻し:
- 時刻参照 API を一箇所へ集約
- 既存コンポーネントの内部時刻更新を撤去

### リスク5: MP4 再生が環境依存になる
対策:
- Media 再生を同期対象から分離して実装開始
- decoder 非依存のロジックテストを優先

切り戻し:
- Media 機能を feature flag 的に無効化
- ShowClock と Robot のみ先行成立させる

### リスク6: Art-Net 実装が loopback 前提で固定化する
対策:
- bind address / destination / universe を設定化
- ソケット生成と packet 解釈を分離

切り戻し:
- loopback 固定コードを設定駆動へ戻す
- ネットワーク境界を adapter 化

### リスク7: ハード依存テストが混入する
対策:
- Automation Test は pure logic 中心
- DMX hardware / MP4 decoder 非依存を厳守

切り戻し:
- ハード依存テストを手動確認手順へ移す
- 自動テストから該当ケースを分離

---

## 推奨の実装方針まとめ

- 最初に `Art-Net loopback` を最小実装し、受信と fixture 反映の経路を確立する。
- bootstrap 段階の DMX 入力は `QLC+ -> UDP Art-Net -> UArtNetReceiverComponent -> ULightingFixtureComponent -> Light Component` を唯一経路として固定し、UE 標準 DMX Plugin 方式とは混在させない。
- 次に `Robot JSON` を **事前ロード + 妥当性検証 + 補間可能構造体** として固める。
- `MP4` は UE 5.7 で確認できる標準 Media API のみで導入する。
- `ShowClock` は少し後段だが、導入後は必ず唯一の時刻源へ寄せる。
- 統合前に pure logic を増やし、Automation Test を先に成立させる。
- `PC B分離` と `ENTTEC Bridge` は、最初から境界分離を意識して後方互換で拡張する。

---

## 現状の確認根拠

### `.github/copilot-instructions.md`
- UE 5.7 / VS 2022 / Windows 11
- ShowClock 単一時刻源
- JSON は Tick ごとに parse しない
- parsing と Actor movement を分離
- reusable behavior は ActorComponent 優先
- Runtime modules は `UnrealEd` 非依存
- Blueprint 公開は `UFUNCTION` / `UPROPERTY`
- Automation Test Framework 利用
- JSON / interpolation / seeking / boundary のテスト要求

### `StageShowSimulator.uproject`
- Module:
  - `StageShowSimulator` / `Runtime`
- Plugin:
  - `ModelingToolsEditorMode` 有効

### `Source/StageShowSimulator/StageShowSimulator.Build.cs`
- Public dependencies:
  - `Core`
  - `CoreUObject`
  - `Engine`
  - `InputCore`
  - `EnhancedInput`

### `Source/StageShowSimulator.Target.cs`
- `Type = Game`
- `DefaultBuildSettings = V6`
- `IncludeOrderVersion = Unreal5_7`

### `Source/StageShowSimulatorEditor.Target.cs`
- `Type = Editor`
- `DefaultBuildSettings = V6`
- `IncludeOrderVersion = Unreal5_7`

### `Source/StageShowSimulator/StageShowSimulator.cpp`
- `IMPLEMENT_PRIMARY_GAME_MODULE(...)` のみ

### `Config/DefaultEngine.ini`
- Default Map: `/Engine/Maps/Templates/OpenWorld`
- Windows graphics: DX12 / SM6
- project redirect: `StageShowSimulator`

---

## 次アクション提案

変更はまだ行わず、次の順で詳細化すると進めやすい。

1. `Docs/Requirements.md` の所在を確定する
2. Art-Net loopback の対象 Universe / Port / 期待 packet を明文化する
3. Robot JSON のスキーマを確定する
4. MP4 再生で使う UE 5.7 標準 API / Asset 構成を確認する
5. ShowClock の責務境界を確定する
6. Automation Test の配置方針を決める

以上。

---

## 実装反映チェックリスト（feature/bootstrap 時点）

本節は、本計画の第1〜第7段階に対して、今回実装した内容を対応付けたチェックリスト。

### 第1段階: 同一PCのLoopback Art-Net
- [x] `UArtNetReceiverComponent` を追加（UDP 受信、Universe ごとの最新フレーム保持）
- [x] `FArtNetPacketParser` / `FArtNetDmxFrame` を追加（ArtDMX 最小パース）
- [x] `ULightingFixtureComponent` を追加（DMX 値を Light Intensity へ反映）
- [x] 受信設定を `BindAddress` / `Port` / `UniverseFilter` で外部設定化
- [x] PIE での実送信確認（QLC+ 連携）は未実施（手動確認項目）

#### DMX 入力方針（bootstrap 段階で固定）
- [x] DMX 入力経路を `QLC+ -> UDP Art-Net -> UArtNetReceiverComponent -> ULightingFixtureComponent -> Light Component` に固定する
- [x] bootstrap 段階では UE 標準 DMX Plugin / DMX Library / Fixture Patch を必須構成に含めない
- [x] bootstrap 範囲では、独自受信経路と UE 標準 DMX Plugin 方式を混在させない
- [x] 将来の移行または併用は、既存実装へ直接混在させず DMX 入力境界を追加して切り替える

### 第2段階: Robot JSON
- [x] `FRobotPoseSample` / `FRobotPoseTrack` を追加
- [x] `URobotPoseLoader::LoadPoseTrackFromJsonFile` を追加（事前ロード + 構文/スキーマ検証）
- [x] `URobotPosePlaybackComponent` を追加（ShowClock 時刻で補間評価）
- [x] Tick ごとの JSON parse を回避（ロード時のみ parse）

### 第3段階: MP4
- [ ] 未着手（今回範囲外）

### 第4段階: ShowClock
- [x] `UShowClockComponent` を追加
- [x] `Play` / `Pause` / `Stop` / `Seek` を実装
- [x] 境界クランプ（0 未満・終端超過）を実装
- [x] `AdvanceTime` を追加し、時刻進行ロジックを単一化

### 第5段階: 統合テスト
- [x] Unreal Automation Test を追加（ハード非依存 / decoder 非依存）
  - [x] `StageShowSimulator.RobotPose.JsonValidation`
  - [x] `StageShowSimulator.RobotPose.Interpolation`
  - [x] `StageShowSimulator.RobotPose.Boundary`
  - [x] `StageShowSimulator.ShowClock.SeekBoundary`
  - [x] `StageShowSimulator.ArtNet.PacketParse`
- [x] `Automation RunTests StageShowSimulator.` 実行で成功ログ確認（EXIT CODE: 0）

### 第6段階: PC B分離
- [~] 受信アドレス/ポート設定化までは対応済み
- [ ] 別PC運用を前提にした設定プロファイル整備は未着手

### 第7段階: ENTTEC Bridge
- [ ] 未着手（境界設計のみ今後対応）

### 共通ルール適合チェック
- [x] Runtime モジュールへ `UnrealEd` 依存を追加していない
- [x] `UFUNCTION` / `UPROPERTY` で Blueprint 公開対象を付与
- [x] 専用ログカテゴリ `LogStageShowSimulator` を追加
- [x] Engine ソース変更なし
- [x] `uasset` 生成・変更なし
- [x] 外部ライブラリ追加なし

