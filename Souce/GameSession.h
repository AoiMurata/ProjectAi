// =============================================================================
// GameSession.h
// ゲームセッション全体の戦績、難易度（HP補正）、および獲得スキル状態を管理するヘッダーファイル
// =============================================================================
#pragma once

// ゲームの勝敗結果状態
enum class GameOutcome
{
	RoundClear, // ラウンドクリア状態
	GameOver    // ゲームオーバー状態
};

// 通常スキルの種類
enum class SkillType
{
	FireRate,    // 自動連射速度アップ
	ChargeBoost, // チャージ蓄積効率アップ（1クリックあたりの増加数アップ）
	Damage       // 氷弾の基本攻撃力アップ
};

// ボス討伐時に獲得できる特殊スキルの種類
enum class SpecialSkillType
{
	MultiShot, // マルチショット（1回に複数発を扇状に同時射撃する）
	Homing     // 追尾弾（敵を自動でホーミングする）
};

// ラウンドの出現テーブルタイプ
enum class RoundSpawnType
{
	Normal,  // 一般敵のみ出現する通常ラウンド
	MidBoss, // 中ボスが出現するラウンド
	Boss     // 大ボスが出現するラウンド
};

// セッション進行状態を管理するグローバル名前空間
namespace GameSession
{
	// 新しいゲームプレイ（Run）を開始し、戦績やスキルレベルを初期値にリセットする
	void StartNewRun();
	
	// デバッグ用のカスタム開始処理
	void StartDebugRun();
	
	// ラウンドをクリアした際の処理（余剰残弾ボーナス加算、ハイスコア更新、敵HP上昇ボーナスの累積など）
	void OnRoundCleared(int shotsRemaining, bool bossWasDefeated);
	
	// 通常スキルをレベルアップする
	void ApplySkill(SkillType skill);
	
	// 特殊スキルをレベルアップする（最大値に達していなければレベル加算）
	bool ApplySpecialSkill(SpecialSkillType skill);
	
	// ゲームオーバー状態をセットする
	void SetGameOver();
	
	// 特殊スキル獲得処理の終了を通知する
	void ConsumeSpecialSkillPick();

	// スキルレベルを直接変更する（デバッグ・テスト用）
	void SetSkillLevel(SkillType s, int lvl);
	// 特殊スキルレベルを直接変更する（デバッグ・テスト用）
	void SetSpecialSkillLevel(SpecialSkillType s, int lvl);
	// 現在のラウンドを強制変更する（デバッグ・テスト用）
	void SetRound(int r);
	// 現在のラウンドに応じた最大チャージ可能弾数を算出する
	int GetMaxChargeShots();

	// ゲームセッションの結果状態を取得する
	GameOutcome GetOutcome();
	// 現在のラウンド数を取得する
	int GetRound();
	// 現在のスコアを取得する
	int GetScore();
	// システムの過去最高スコア（ハイスコア）を取得する
	int GetHighScore();
	// クリア済みのラウンド合計数を取得する
	int GetRoundsCleared();
	// ボスを撃破し、特殊スキルの選択待ち状態であるかを判定する
	bool ShouldPickSpecialSkill();

	// 指定されたラウンドが出現テーブルのどの種類に該当するかを判定する
	RoundSpawnType GetRoundSpawnType(int round);
	// 通常スキルの現在の取得レベルを取得する
	int GetSkillLevel(SkillType skill);
	// 特殊スキルの現在の取得レベルを取得する
	int GetSpecialSkillLevel(SpecialSkillType skill);
	// 特殊スキルがまだ上限（最大レベル）に達しておらず、アップグレード可能か判定する
	bool CanUpgradeSpecialSkill(SpecialSkillType skill);
	// マルチショットの現在の同時発射弾数を取得する（スキルレベルに応じる）
	int GetMultiShotCount();
	// 弾の追尾（ホーミング）旋回能力値（ターンレート）を取得する
	float GetHomingTurnRate();
	// ホーミングスキルをすでに1レベル以上取得しているかを判定する
	bool HasHoming();

	// 一般敵の出現時の最小HP値を取得する
	int GetEnemyHpMin();
	// 一般敵の出現時の最大HP値を取得する
	int GetEnemyHpMax();
	// 中ボスの現在の出現時HP値を取得する（出現回数に応じて補正がかかる）
	int GetMidBossHp();
	// 大ボスの現在の出現時HP値を取得する（出現回数に応じて補正がかかる）
	int GetBossHp();
	// 連射速度レベルを反映した現在の自動射撃フレーム間隔を取得する
	int GetAutoFireInterval();
	// チャージ効率レベルを反映した1クリックあたりの蓄積弾数を取得する
	int GetChargePerClick();
	// ダメージアップレベルを反映した氷弾1発当たりのダメージ数値を取得する
	int GetIceDamage();

	// 各種通常スキルの日本語名表示文字列を取得する
	const char* GetSkillName(SkillType skill);
	// 各種通常スキルの効果説明表示用文字列を取得する
	const char* GetSkillDescription(SkillType skill);
	// 各種特殊スキルの日本語名表示文字列を取得する
	const char* GetSpecialSkillName(SpecialSkillType skill);
	// 各種特殊スキルの効果説明表示用文字列を取得する
	const char* GetSpecialSkillDescription(SpecialSkillType skill);
}
