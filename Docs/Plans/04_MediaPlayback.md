
## 前提

- 本ターンではコード、設定ファイル、プロジェクトファイルの変更、およびターミナルコマンドの実行は行っていない。
- 本改訂は実装前レビューの指摘（Open完了判定、Delegate管理、MediaSource優先、状態遷移の明確化等）を反映したものである。
- 参照:
  - `.github/copilot-instructions.md`
  - `Docs/Requirements.md`
  - `Docs/Plans/01_ProjectBootstrap.md`
  - `Source/StageShowSimulator/Public/ShowClock/ShowClockComponent.h` / `.cpp`
  - `Source/StageShowSimulator/Public/Robot/RobotPosePlaybackComponent.h` / `.cpp`
  - `Source/StageShowSimulator/StageShowSimulator.Build.cs`
  - `StageShowSimulator.uproject`
  - `Source/StageShowSimulator/Public/StageShowSimulatorLog.h`
  - `Source/StageShowSimulator/Private/Tests/ShowClockTests.cpp`

---

## 1. 現在のMedia関連依存状況

- `StageShowSimulator.Build.cs` の `PublicDependencyModuleNames` は `Core`, `CoreUObject`, `Engine`, `InputCore`, `EnhancedInput`, `Json`, `JsonUtilities`, `Networking`, `Sockets` のみ。
- Media関連モジュール（`MediaAssets` など）は未追加。
- `StageShowSimulator.uproject` にMedia系Plugin明示追加はない（エンジンデフォルトの有効状態に依存）。
- 既存コードにMedia関連クラス（`UMediaPlayer`, `UMediaSource`, `UMediaTexture`, `UMediaSoundComponent` 等）への参照はない。

---

## 2. 使用するUE 5.7標準モジュール

- `MediaAssets`（`UMediaPlayer`, `UMediaSource` を含む標準Runtimeモジュール）。
- 既存の `Core`, `CoreUObject`, `Engine` は継続利用。
- `UMediaPlayer` の `OnMediaOpened` / `OnMediaOpenFailed` はマルチキャストデリゲート（`FOnMediaPlayerMediaOpened` / `FOnMediaPlayerMediaOpenFailed`）としてUE5.7でも提供されている前提とする。実装時にヘッダ差異があれば別途報告する。
- Runtimeモジュールのため `UnrealEd` への依存は追加しない。

---

## 3. 追加するクラス

- `ULedMediaPlaybackComponent`
  - `UActorComponent` 派生。
  - `UMediaPlayer` への参照を外部（Editor/Blueprint）から割り当てる。
  - `UMediaSource` への参照をオプションで割り当てる（設定時はこちらを優先）。
  - Open完了/失敗はDelegateで確定し、戻り値だけに依存しない。
  - Open / Play / Pause / Stop / Seek / GetCurrentMediaTime / GetDuration / IsMediaPlayerValid / IsMediaReady を提供。
  - Media Player未設定・未Ready時は安全にログを出し早期returnする。
- 独自State enumは追加しない。状態は `bIsOpenPending` / `bIsOpened` と `UMediaPlayer` 自身の状態（`IsReady()` 等）に委譲する。

---

## 4. 追加、変更するファイル

### 追加
- `Source/StageShowSimulator/Public/Media/LedMediaPlaybackComponent.h`
- `Source/StageShowSimulator/Private/Media/LedMediaPlaybackComponent.cpp`
- `Source/StageShowSimulator/Private/Tests/LedMediaPlaybackTests.cpp`

### 変更
- `Source/StageShowSimulator/StageShowSimulator.Build.cs`
  - `PublicDependencyModuleNames` に `MediaAssets` を追加。

### 変更しない
- `ShowClockComponent.h` / `.cpp`
- `RobotPosePlaybackComponent.h` / `.cpp`
- Art-Net関連の既存コード
- `StageShowSimulator.uproject`（Plugin明示追加は今回範囲外。必要になれば別途報告）

---

## 5. 修正後の責務

- `ULedMediaPlaybackComponent` は単一の `UMediaPlayer` インスタンスに対する再生操作の窓口を提供する。
- Open処理の「開始」と「完了（成功/失敗）」を明確に分離する。
  - Open要求の発行（`OpenUrl`/`OpenSource` 呼び出し）自体は「受理されたかどうか」のみを示し、Open完了とは判定しない。
  - Open完了（成功）は `OnMediaOpened` Delegate受信でのみ確定する。
  - Open失敗は `OnMediaOpenFailed` Delegate受信でのみ確定する。
- `UMediaSource` が設定されていればそれを優先して `OpenSource` を使用し、未設定で `MediaFilePath` のみ設定されている場合に限り `OpenUrl` によるPath方式を使用する。
- Component終了時（`EndPlay`）およびMediaPlayer再設定時に、古いDelegateバインドを確実に解除し、二重登録・ダングリング参照を防ぐ。
- ShowClockとの同期ロジックは持たない（今回の範囲外）。
- 独自の再生時計を持たない。時刻・Duration・再生状態は `UMediaPlayer` からのみ取得する。
- 音声制御（Mute等）は今回実装しない。無音は `UMediaSoundComponent` をActorへ追加しないことで実現する（Editor側作業）。

---

## 6. 状態遷移

状態は以下の2つのbool（`bIsOpenPending`, `bIsOpened`）と `UMediaPlayer::IsReady()` の組み合わせで表現する。独自State enumは追加しない。

| 状態 | bIsOpenPending | bIsOpened | 説明 |
|---|---|---|---|
| Idle（初期状態） | false | false | Open未実行、またはOpen前 |
| Opening | true | false | `OpenMedia()` 呼び出し後、Delegate未受信 |
| Opened | false | true | `OnMediaOpened` 受信済み |
| OpenFailed | false | false | `OnMediaOpenFailed` 受信済み（Idleへ復帰） |

遷移ルール:
1. `OpenMedia()` 呼び出し時
   - MediaPlayerがnull、またはMediaSource/MediaFilePathが両方未設定の場合は何もせず `false` を返す（状態変化なし）。
   - 呼び出し前に既存の未完了Open（`bIsOpenPending == true`）がある場合は新規Open要求を許可するかどうかを検討し、今回は多重Open防止のため `bIsOpenPending == true` の間は新規`OpenMedia()`呼び出しを拒否し `false` を返す。
   - 上記条件をクリアした場合、`bIsOpenPending = true`, `bIsOpened = false` に設定し、`OpenSource`（優先）または `OpenUrl` を呼び出す。呼び出し自体の戻り値は「要求が受理されたか」のログ用途にのみ使い、状態確定には使わない。
   - 要求発行自体が `false`（例外的に即時拒否された）の場合は `bIsOpenPending = false` に戻し、`false` を返す。
2. `OnMediaOpened` 受信時
   - `bIsOpenPending = false`
   - `bIsOpened = true`
   - ログ出力（Info）。
3. `OnMediaOpenFailed` 受信時
   - `bIsOpenPending = false`
   - `bIsOpened = false`
   - ログ出力（Error）。
4. `StopMedia()` 実行後も `bIsOpened` は変化しない（Open状態自体は維持し、Pause + Rewindのみ行う）。
5. Component破棄・Delegate解除時、`bIsOpenPending` / `bIsOpened` の値はリセットしない（Componentが破棄されるため実質意味を持たない）。

---

## 7. Delegate登録と解除

- 対象Delegate: `UMediaPlayer::OnMediaOpened`, `UMediaPlayer::OnMediaOpenFailed`。
- 登録タイミング:
  - `BeginPlay()` 時、`MediaPlayer` が有効なら現在の `MediaPlayer` にDelegateを登録する。
  - Blueprint等で実行時に `MediaPlayer` プロパティが再設定されるケースに備え、専用関数 `SetMediaPlayer(UMediaPlayer* NewMediaPlayer)`（Blueprint公開）を用意し、その中で「古いMediaPlayerのDelegate解除 → 新しいMediaPlayerへのDelegate登録 → `MediaPlayer`メンバ更新」を行う。
  - 直接 `UPROPERTY` を外部からEditor上で設定するケース（BeginPlay前）は、`BeginPlay()` 内で初回登録することでカバーする。
- 解除タイミング:
  - `EndPlay(EEndPlayReason::Type EndPlayReason)` オーバーライド内で、現在の `MediaPlayer` が有効なら `OnMediaOpened.RemoveDynamic(this, ...)` および `OnMediaOpenFailed.RemoveDynamic(this, ...)` を呼ぶ。
  - `SetMediaPlayer()` 内で古い `MediaPlayer` からのDelegate解除を必ず行ってから新しいものへ登録する。
- 実装方法:
  - `UFUNCTION()` 修飾した2つのハンドラ関数 `HandleMediaOpened(FString OpenedUrl)` / `HandleMediaOpenFailed(FString FailedUrl)` を用意し、`AddDynamic` / `RemoveDynamic` で登録・解除する（`UMediaPlayer` のDelegateはDynamic Multicast Delegateであるため）。
- 二重登録防止のため、登録前に必ず対象Delegateから同一ハンドラを `RemoveDynamic` してから `AddDynamic` する防御的実装とする。

---

## 8. UPROPERTY

```
UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
TObjectPtr<UMediaPlayer> MediaPlayer;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
TObjectPtr<UMediaSource> MediaSource;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
FString MediaFilePath;

UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Media")
bool bOpenOnBeginPlay;

UPROPERTY(BlueprintReadOnly, Category="Media")
bool bIsOpenPending;

UPROPERTY(BlueprintReadOnly, Category="Media")
bool bIsOpened;
```

- `bStartMuted` は今回追加しない（後述「今回の対象外」参照）。
- `MediaSource` と `MediaFilePath` は併存可能とし、優先順位は「11. Open方式の優先順位」節および状態遷移ルールの通り `MediaSource` を優先する。

---

## 9. UFUNCTION

```
UFUNCTION(BlueprintCallable, Category="Media")
bool OpenMedia();

UFUNCTION(BlueprintCallable, Category="Media")
bool PlayMedia();

UFUNCTION(BlueprintCallable, Category="Media")
bool PauseMedia();

UFUNCTION(BlueprintCallable, Category="Media")
bool StopMedia();

UFUNCTION(BlueprintCallable, Category="Media")
bool SeekMedia(double InTimeSeconds);

UFUNCTION(BlueprintCallable, Category="Media")
void SetMediaPlayer(UMediaPlayer* NewMediaPlayer);

UFUNCTION(BlueprintPure, Category="Media")
double GetCurrentMediaTimeSeconds() const;

UFUNCTION(BlueprintPure, Category="Media")
double GetDurationSeconds() const;

UFUNCTION(BlueprintPure, Category="Media")
bool IsMediaPlayerValid() const;

UFUNCTION(BlueprintPure, Category="Media")
bool IsMediaReady() const;
```

---

## 10. 戻り値の意味

- `OpenMedia() -> bool`
  - `true`: Open要求が正しく受理され、`bIsOpenPending = true` に遷移した（＝Open完了を意味しない）。
  - `false`: MediaPlayer未設定、MediaSourceとMediaFilePathが両方未設定、既にOpen処理中、または要求発行自体が拒否された場合。
- `PlayMedia() / PauseMedia() / StopMedia() -> bool`
  - `true`: MediaPlayerが有効かつ `IsReady()` がtrueで、操作要求を発行できた。
  - `false`: MediaPlayer未設定、または `IsReady() == false` のため安全に操作をスキップした。
- `SeekMedia(double InTimeSeconds) -> bool`
  - `true`: MediaPlayerが有効かつReadyで、クランプ済み時刻へのSeek要求を発行できた。
  - `false`: MediaPlayer未設定、またはReadyでないため操作をスキップした。
  - 負の値は0秒にクランプしたうえで要求する（クランプ自体は失敗要因にしない）。
- `GetCurrentMediaTimeSeconds() -> double`
  - MediaPlayer未設定またはReadyでない場合は `0.0`。
- `GetDurationSeconds() -> double`
  - MediaPlayer未設定またはReadyでない場合、または動画Duration取得不可の場合は `0.0`。
- `IsMediaPlayerValid() -> bool`
  - `MediaPlayer` がnullでないことのみを示す（Openやreadyとは無関係）。
- `IsMediaReady() -> bool`
  - `MediaPlayer` が有効かつ `MediaPlayer->IsReady() == true` の場合に `true`。

---

## 11. Open方式の優先順位

- `MediaSource` が設定されている場合: `MediaPlayer->OpenSource(MediaSource)` を使用する。
- `MediaSource` が未設定で `MediaFilePath` が空でない場合: `MediaPlayer->OpenUrl(MediaFilePath)` を使用する。
- 両方とも未設定の場合: `OpenMedia()` は `false` を返し、警告ログを出す（Open要求自体を発行しない）。
- `OpenSource` / `OpenUrl` の戻り値は「要求受理可否」のログ用途にのみ使用し、`bIsOpened` の確定には使わない（6章参照）。

---

## 12. Stop挙動の定義

- `StopMedia()` は以下の順で実行する。
  1. `MediaPlayer` がnull、または `IsReady() == false` の場合は何もせず `false` を返す。
  2. `MediaPlayer->Pause()` を呼ぶ。
  3. `MediaPlayer->Rewind()` を呼ぶ（0秒へ戻す）。
  4. `true` を返す。
- Open状態（`bIsOpened`）自体は変更しない（Close相当の完全クローズは行わない）。
- `Rewind()` がUE5.7で提供されていない場合は `Seek(FTimespan::Zero())` に置き換える（実装時にAPI確認し、差異があれば報告する）。

---

## 13. Seekの境界処理

- `SeekMedia(double InTimeSeconds)`:
  1. `MediaPlayer` がnull、または `IsReady() == false` の場合は `false` を返す。
  2. 負の値は `0.0` にクランプする。
  3. Durationが取得可能（`GetDurationSeconds() > 0.0`）な場合、クランプ後の時刻がDurationを超えるときはDuration値（末尾）にクランプする。Duration取得不可（`0.0`）の場合はDurationによる上限クランプを行わない。
  4. クランプ済み時刻で `MediaPlayer->Seek(FTimespan::FromSeconds(ClampedTime))` を呼び、結果を返す。
- 毎フレームSeekは行わない。`SeekMedia` は外部トリガー（Blueprintイベント、テスト等）からの呼び出しのみを想定し、Tick内での自動呼び出しは実装しない。

---

## 14. Component終了時のクリーンアップ

- `EndPlay(EEndPlayReason::Type EndPlayReason)` をオーバーライドする。
- 現在の `MediaPlayer` が有効な場合、`OnMediaOpened` / `OnMediaOpenFailed` からハンドラを `RemoveDynamic` する。
- `Super::EndPlay(EndPlayReason)` を呼ぶことを忘れない。

---

## 15. 音声Muteの扱い（今回未実装）

- `bStartMuted` / `SetMute()` は今回実装しない。
- 初期状態を無音にするため、LED Display Actorに `UMediaSoundComponent` を追加しないことをEditor作業手順とする。
- `SetMute()` はShowClock統合段階で対象とする未実装項目として明記し、本計画の「今回の対象外」に残す。

---

## 16. 自動テスト（`LedMediaPlaybackTests.cpp`）

Unreal Automation Test Framework（`IMPLEMENT_SIMPLE_AUTOMATION_TEST` 等）を使用し、実際のMP4デコードやDMX/Art-Netハードウェアに依存しないテストのみを実装する。

1. **MediaPlayer未設定時の全API**
   - `MediaPlayer == nullptr` の状態で `OpenMedia()`, `PlayMedia()`, `PauseMedia()`, `StopMedia()`, `SeekMedia()` を呼び、すべて `false` を返すこと。
   - `GetCurrentMediaTimeSeconds()`, `GetDurationSeconds()` が `0.0` を返すこと。
   - `IsMediaPlayerValid()`, `IsMediaReady()` が `false` を返すこと。
2. **MediaSourceとMediaFilePathが両方未設定**
   - 有効な `MediaPlayer` をアタッチしても `MediaSource == nullptr` かつ `MediaFilePath.IsEmpty()` の場合、`OpenMedia()` が `false` を返すこと。
3. **負のSeek時刻のクランプ処理**
   - Ready状態を模擬できない場合はクランプ計算ロジック単体（内部ヘルパー関数として切り出す場合）を直接検証するか、`IsReady() == false` の状態で `SeekMedia(-5.0)` が安全に `false` を返すことを検証する。
4. **Duration境界ロジック**
   - Duration取得不可時に上限クランプが行われないこと、Duration取得可能を模擬できないため、ロジックをprivateヘルパー関数（例: `double ClampSeekTime(double InTime, double Duration) const`）として切り出し、そのヘルパー関数を直接テストする。
5. **Open状態フラグの初期値**
   - Component生成直後、`bIsOpenPending == false`, `bIsOpened == false` であること。
6. **Mediaデコーダと実動画に依存しないことの確認**
   - すべてのテストが実ファイルパスやネットワークアクセスを伴わないことをコードレビューで担保する（テストコード内で実MP4ファイルを参照しない）。
7. **既存StageShowSimulatorテストの回帰実行**
   - `StageShowSimulator.ShowClock.*` 等、既存の自動テストを再実行し全PASSを確認する。

- テスト対象のロジックを可能な限りDelegate/UMediaPlayerの実挙動に依存しない形（Ready判定やクランプ計算をprivateヘルパーに切り出す等）で設計し、テスト容易性を確保する。

---

## 17. Editor手動作業

- `Content/Movies/` フォルダを作成し、動画ファイル（MP4）は原則このフォルダ配下に配置する。
- Content Browser上で以下を作成する:
  - `UMediaSource`（`FileMediaSource` 等、動画ファイルを指定）
  - `UMediaPlayer`
  - `UMediaTexture`（`UMediaPlayer` に紐付け）
- LED表示用MaterialまたはMaterial Instanceに `UMediaTexture` を割り当てる。
- LED Display ActorへBlueprintまたはC++で `ULedMediaPlaybackComponent` をアタッチする。
- Componentの `MediaPlayer` プロパティに作成済み `UMediaPlayer` アセットを割り当てる。
- Componentの `MediaSource` プロパティに作成済み `UMediaSource` アセットを割り当てる（優先して使用されるため、`MediaFilePath` は省略可）。
- `MediaSource` を使わない場合のみ `MediaFilePath` に `Content/Movies` 配下のファイルパスを設定する。
- 初期状態を無音にするため、`UMediaSoundComponent` をLED Display Actorに追加しない。

---

## 18. 手動受入テスト

- `Development Editor / Win64` でPIE実行し、以下を確認する。
  1. `bOpenOnBeginPlay = true` の場合、BeginPlay直後に `bIsOpenPending = true` となり、その後まもなく `bIsOpened = true` に遷移すること（Outputログでも `OnMediaOpened` 受信ログを確認）。
  2. `PlayMedia()` 呼び出しでLED Display上に動画が再生されること。
  3. `PauseMedia()` で一時停止できること。
  4. `StopMedia()` で先頭（0秒）に戻り一時停止状態になること。
  5. `SeekMedia()` で任意時刻へジャンプできること（負値・Duration超過値でもクラッシュしないこと）。
  6. Actorを破棄（PIE終了等）してもクラッシュや警告ログ（Delegateの二重解除等）が出ないこと。
  7. 音声が既定でOFF（`UMediaSoundComponent` 未追加）であることを確認する。
  8. `MediaSource` と `MediaFilePath` を両方未設定にした場合、`OpenMedia()` がfalseとなりログに警告が出ることを確認する。

---

## 19. ビルドと回帰テスト

- Visual Studio 2022（17.14以降）で `Development Editor / Win64` 構成でビルドする。
- `Build.cs` 変更後は必ず再ビルドし、`MediaAssets` モジュールのリンクエラーがないことを確認する。
- 小さな実装単位ごとにビルドを実行する。
- ビルド成功後、Unreal Automation Test Framework（Session Frontend または `Automation RunTests` コマンド）で以下を実行する。
  - 既存の `StageShowSimulator.ShowClock.*` 等の回帰テスト。
  - 新規追加する `StageShowSimulator.Media.*` テスト群。
- 全テストがPASSすることを確認するまで「成功」を主張しない。

---

## 20. リスク

- `MediaAssets` モジュール追加時、実行環境で `WindowsMedia` / `WmfMedia` などのDecoder Pluginが無効の場合、MP4再生自体ができない可能性がある（今回はPlugin追加を行わないため実機確認が必要）。
- `UMediaPlayer::OnMediaOpened` / `OnMediaOpenFailed` のDelegateシグネチャ（引数の型・個数）がUE5.7で変更されている場合、`AddDynamic`/`RemoveDynamic` のハンドラシグネチャ不一致でコンパイルエラーとなる可能性がある。実装時にヘッダを確認し、差異があれば計画を修正する。
- `Rewind()` がUE5.7の `UMediaPlayer` に存在しない場合、`Seek(FTimespan::Zero())` へ置き換える必要がある。
- `OpenSource` / `OpenUrl` の非同期性により、テスト環境（Automation Test）では `OnMediaOpened` の受信タイミングが不定となり、状態確定を伴う統合的なテストが困難な場合がある。そのため自動テストはDelegate受信を待たない範囲（未設定時の安全性、クランプロジック等）に限定する。
- Delegateの多重登録・解除漏れがあると、Componentの再利用時に古いハンドラが呼ばれ続ける可能性がある。`SetMediaPlayer()` と `EndPlay()` の両方で確実に解除することが必須。

---

## 21. 切り戻し方法

- 追加ファイル（`LedMediaPlaybackComponent.h/.cpp`, `LedMediaPlaybackTests.cpp`）を削除する。
- `StageShowSimulator.Build.cs` の `MediaAssets` 追加行を削除する。
- Git上でコミット前であれば `git checkout -- <path>`、コミット後であれば該当コミットの `git revert` を使用する。
- Content内で作成した `UMediaSource` / `UMediaPlayer` / `UMediaTexture` アセットはEditor上で手動削除する（uasset操作はAIエージェントの対象外）。

---

## 22. 今回の対象外

- `bStartMuted` プロパティの実装。
- `SetMute()` などの音声制御API（ShowClock統合段階で対象とする未実装項目として残す）。
- ShowClockとの再生同期ロジック。
- Robot Motion、Art-Netの既存コード変更。
- `StageShowSimulator.uproject` のPlugin明示追加。
- uassetの生成・変更（`UMediaSource`, `UMediaPlayer`, `UMediaTexture` はEditorで人が作成する）。
- Media音量・フェード制御。
- 複数MediaPlayer/複数LED Displayの同時管理（今回は単一Component=単一MediaPlayerの前提）。

---

## 23. 実装および受入結果

### C++実装

- MediaAssets依存追加: 完了
- ULedMediaPlaybackComponent: 完了
- MediaPlayerおよびMediaSource外部割り当て: 完了
- OnMediaOpenedおよびOnMediaOpenFailed: 完了
- Delegate登録および解除: 完了
- Open、Play、Pause、Stop、Seek: 完了
- MediaTimeおよびDuration取得: 完了
- Missing Media設定時の安全処理: 完了
- ローカル絶対パスをログへ出さない対応: 完了

### ビルドおよび自動テスト

- Development Editor / Win64: 成功
- Build warnings: 0
- Build errors: 0
- Lighting tests: 1件成功
- Media tests: 13件成功
- StageShowSimulator tests: 24件成功
- Automation Test Exit Code: 0

### 手動受入テスト

以下をすべて確認し、合格した。

- bOpenOnBeginPlayによるOpen
- bIsOpenPendingの状態遷移
- OnMediaOpened後のbIsOpened
- LED面への映像表示
- Play
- Pause
- Stop
- Seek
- 負のSeek
- Duration超過Seek
- MediaSource未設定時の安全動作
- PIE終了時の安全動作
- PIE再実行
- 無音再生

詳細はDocs/TestResults/04_MediaPlayback_ManualTest.mdを参照する。

### 受入判定

Standalone Media Playback段階はAcceptedとする。

ShowClock同期、Media Drift補正、音声制御は後続段階の対象とする。