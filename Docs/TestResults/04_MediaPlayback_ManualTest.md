# Media Playback Manual Test Result

## 1. Test Summary

StageShowSimulatorのMP4単体再生機能について、Unreal Editor上で手動受入テストを実施した。

すべての受入項目に合格した。

## 2. Test Target

- Feature: Standalone Media Playback
- Component: ULedMediaPlaybackComponent
- Plan: Docs/Plans/04_MediaPlayback.md
- Branch: feature/media-playback
- Build Target: StageShowSimulatorEditor
- Build Configuration: Development Editor
- Platform: Win64
- Unreal Engine: 5.7

## 3. Preconditions

- Development Editor / Win64ビルド成功
- Build warnings: 0
- Build errors: 0
- StageShowSimulator.Mediaテスト成功
- StageShowSimulator.Lightingテスト成功
- StageShowSimulator全24テスト成功
- Automation Test Exit Code: 0
- Media Player Asset作成済み
- File Media Source Asset作成済み
- Media Texture Asset作成済み
- LED表示用Material作成済み
- MP4ファイルをContent/Movies配下へ配置済み
- ULedMediaPlaybackComponentをLED Display Actorへ追加済み
- Media PlayerとMedia SourceをComponentへ設定済み

## 4. Test Environment

- MP4 Asset:
  - 使用したプロジェクト内パスを記載
- File Media Source:
  - 使用したAsset Pathを記載
- Media Player:
  - 使用したAsset Pathを記載
- Media Texture:
  - 使用したAsset Pathを記載
- LED Material:
  - 使用したAsset Pathを記載
- Test Map:
  - 使用したMap Pathを記載
- Test Actor:
  - 使用したActorまたはBlueprint Pathを記載
- Audio:
  - Media Sound Component未追加

ローカルPCの絶対パス、ユーザー名、認証情報は記録しない。

## 5. Manual Test Results

### MP-MAN-001: Open on BeginPlay

- Operation:
  - bOpenOnBeginPlayを有効にしてPIEを開始する
- Expected:
  - MediaのOpen要求が開始される
- Result:
  - Pass

### MP-MAN-002: Open Pending State

- Operation:
  - Open処理開始時の状態を確認する
- Expected:
  - Open処理中にbIsOpenPendingがtrueになる
- Result:
  - Pass

### MP-MAN-003: Open Completed State

- Operation:
  - OnMediaOpened受信後の状態を確認する
- Expected:
  - bIsOpenPendingがfalseになる
  - bIsOpenedがtrueになる
- Result:
  - Pass

### MP-MAN-004: LED Video Display

- Operation:
  - LED表示面を確認する
- Expected:
  - MP4映像がLED表示面に描画される
- Result:
  - Pass

### MP-MAN-005: Play

- Operation:
  - PlayMediaを実行する
- Expected:
  - 動画再生が開始または再開される
- Result:
  - Pass

### MP-MAN-006: Pause

- Operation:
  - PauseMediaを実行する
- Expected:
  - 動画が現在位置で一時停止する
- Result:
  - Pass

### MP-MAN-007: Stop

- Operation:
  - StopMediaを実行する
- Expected:
  - 動画が0秒へ戻り、一時停止状態になる
- Result:
  - Pass

### MP-MAN-008: Seek

- Operation:
  - 有効な時刻を指定してSeekMediaを実行する
- Expected:
  - 指定した再生位置へ移動する
- Result:
  - Pass

### MP-MAN-009: Negative Seek

- Operation:
  - 負の時刻を指定してSeekMediaを実行する
- Expected:
  - 0秒へクランプされる
  - Unreal Editorがクラッシュしない
- Result:
  - Pass

### MP-MAN-010: Seek Beyond Duration

- Operation:
  - 動画Durationを超える時刻を指定してSeekMediaを実行する
- Expected:
  - Durationへクランプされる
  - Unreal Editorがクラッシュしない
- Result:
  - Pass

### MP-MAN-011: Missing Media Source

- Operation:
  - MediaSourceとMediaFilePathを未設定にしてOpenMediaを実行する
- Expected:
  - OpenMediaが安全に失敗する
  - 警告ログが出力される
  - Unreal Editorがクラッシュしない
- Result:
  - Pass

### MP-MAN-012: PIE Shutdown

- Operation:
  - 動画利用後にPIEを終了する
- Expected:
  - Delegate解除を含む終了処理でクラッシュしない
- Result:
  - Pass

### MP-MAN-013: PIE Restart

- Operation:
  - PIE終了後に再度PIEを開始する
- Expected:
  - Mediaを再度Openできる
  - Delegateの二重登録による異常がない
- Result:
  - Pass

### MP-MAN-014: Muted Playback

- Operation:
  - Media Sound Componentを追加せずに動画を再生する
- Expected:
  - 音声が出力されない
- Result:
  - Pass

## 6. Automated Test Results

- Build:
  - Result: Succeeded
  - Warnings: 0
  - Errors: 0

- StageShowSimulator.Lighting:
  - Detected: 1
  - Success: 1
  - Exit Code: 0

- StageShowSimulator.Media:
  - Detected: 13
  - Success: 13
  - Exit Code: 0

- StageShowSimulator:
  - Detected: 24
  - Success: 24
  - Exit Code: 0

MediaPlayer未設定テストで出力される警告は、未設定時の安全動作を確認するための想定内ログである。

## 7. Defects

- Open defects: None
- Deferred defects: None

## 8. Acceptance Decision

Media Playback単体機能は、Docs/Plans/04_MediaPlayback.mdに定義された受入条件を満たした。

Decision: Accepted

## 9. Remaining Scope

次の項目は今回の受入対象外であり、後続段階で実装する。

- ShowClockとの同期
- Media Drift監視
- Drift閾値超過時の補正
- SetMute
- Media Sound Component制御
- Robot Motionとの統合
- PC B分離
- ENTTEC Bridge

## 10. Related Commits

- Lighting lifecycle test:
  - コミットハッシュを記載
- Standalone Media Playback:
  - コミットハッシュを記載

## 11. Evidence

必要に応じて、次の証跡をリポジトリ外または軽量な画像として保管する。

- LED面に動画が表示されているPIE画面
- Media PlayerおよびMedia Sourceの設定画面
- Automation Test成功画面
- Build成功画面

証跡にはローカル絶対パス、ユーザー名、認証情報を含めない。