// =============================================================================
// GameSession.cpp
// ゲームセッション全体の戦績、難易度（HP補正）、および獲得スキル状態管理の実装ファイル
// =============================================================================
#include "GameSession.h"
#include "Constants.h"
#include "DxLib.h"

namespace
{
	// ゲームセッション用の静的メンバー状態変数
	GameOutcome s_outcome = GameOutcome::GameOver; // 勝敗結果
	int s_round = 1;                              // 現在のラウンド数
	int s_score = 0;                              // 現在のスコア
	int s_highScore = 0;                          // 過去最高スコア（システム上のハイスコア）
	int s_totalHpBonus = 0;                       // 敵の現在までの合計加算HPボーナス
	int s_fireRateLevel = 0;                      // 連射速度の現在レベル
	int s_chargeLevel = 0;                        // チャージ蓄積効率の現在レベル
	int s_damageLevel = 0;                        // ダメージの現在レベル
	int s_multiShotLevel = 0;                     // マルチショットの現在レベル
	int s_homingLevel = 0;                        // ホーミング追尾の現在レベル
	int s_midBossAppearances = 0;                 // 中ボスの通算出現回数
	int s_bossAppearances = 0;                    // 大ボスの通算出現回数
	bool s_pendingSpecialSkillPick = false;       // 特殊スキル選択可能フラグ（ボス撃破時に立ちます）

	// ハイスコア更新処理
	void UpdateHighScore()
	{
		if (s_score > s_highScore)
		{
			s_highScore = s_score;
		}
	}

	// 指定されたラウンドをクリアした際に追加される敵のHP上昇ボーナス量を算出して返す
	int RollHpBonusForRound(int clearedRound)
	{
		// 難度閾値ラウンド（定数：10）以上の場合、より高いHP上昇幅を選択
		if (clearedRound >= ROUND_HIGH_HP_THRESHOLD)
		{
			const int range = ENEMY_HP_BONUS_HIGH_MAX - ENEMY_HP_BONUS_HIGH_MIN + 1;
			return GetRand(range) + ENEMY_HP_BONUS_HIGH_MIN;
		}

		const int range = ENEMY_HP_BONUS_MAX - ENEMY_HP_BONUS_MIN + 1;
		return GetRand(range) + ENEMY_HP_BONUS_MIN;
	}
}

namespace GameSession
{
	// 新しいゲーム（Run）を開始する際のパラメータ初期化処理
	void StartNewRun()
	{
		s_outcome = GameOutcome::RoundClear;
		s_round = 1;
		s_score = 0;
		s_totalHpBonus = 0;
		s_fireRateLevel = 0;
		s_chargeLevel = 0;
		s_damageLevel = 0;
		s_multiShotLevel = 0;
		s_homingLevel = 0;
		s_midBossAppearances = 0;
		s_bossAppearances = 0;
		s_pendingSpecialSkillPick = false;
	}

	// デバッグ用起動時のパラメータ初期化処理
	void StartDebugRun()
	{
		s_outcome = GameOutcome::RoundClear;
		s_score = 0;
		s_pendingSpecialSkillPick = false;
		s_midBossAppearances = (s_round - 1) / MID_BOSS_ROUND_INTERVAL;
		s_bossAppearances = (s_round - 1) / BOSS_ROUND_INTERVAL;
	}

	// 指定ラウンドが出現テーブルのどの種別であるかを返す
	RoundSpawnType GetRoundSpawnType(int round)
	{
		// BOSS_ROUND_INTERVAL（定数：10）の間隔で大ボスラウンド
		if (round % BOSS_ROUND_INTERVAL == 0)
		{
			return RoundSpawnType::Boss;
		}
		// MID_BOSS_ROUND_INTERVAL（定数：5）の間隔で中ボスラウンド
		if (round % MID_BOSS_ROUND_INTERVAL == 0)
		{
			return RoundSpawnType::MidBoss;
		}
		return RoundSpawnType::Normal;
	}

	// ラウンドクリア時のスコア加算、ハイスコア更新、敵HP上昇補正、およびラウンド更新処理
	void OnRoundCleared(int shotsRemaining, bool bossWasDefeated)
	{
		s_outcome = GameOutcome::RoundClear;
		const int clearedRound = s_round;

		// スコア加算: 基本クリア点（ラウンド数 * 500点） + 余剰残弾数ボーナス（残弾1個につき10点）
		s_score += clearedRound * SCORE_PER_ROUND + shotsRemaining * SCORE_PER_REMAINING_SHOT;

		// 中ボスラウンドクリア時はさらに中ボスボーナス（1000点）を加算
		if (GetRoundSpawnType(clearedRound) == RoundSpawnType::MidBoss)
		{
			s_score += SCORE_MID_BOSS_BONUS;
		}
		// ボスを撃破した場合はさらに大ボスボーナス（2000点）を加算し、特殊スキル選択を許可する
		if (bossWasDefeated)
		{
			s_score += SCORE_BOSS_BONUS;
			s_pendingSpecialSkillPick = true;
		}

		UpdateHighScore();
		s_totalHpBonus += RollHpBonusForRound(clearedRound); // 次ラウンドからの敵HP加算ボーナスを累積
		++s_round; // 次ラウンドへ進む
	}

	// 通常スキルのレベルアップ処理
	void ApplySkill(SkillType skill)
	{
		switch (skill)
		{
		case SkillType::FireRate:
			++s_fireRateLevel;
			break;
		case SkillType::ChargeBoost:
			++s_chargeLevel;
			break;
		case SkillType::Damage:
			++s_damageLevel;
			break;
		}
	}

	// 特殊スキルのレベルアップ処理（最大レベル上限に達していなければ適用）
	bool ApplySpecialSkill(SpecialSkillType skill)
	{
		switch (skill)
		{
		case SpecialSkillType::MultiShot:
			if (s_multiShotLevel >= SPECIAL_SKILL_MAX_LEVEL)
			{
				return false;
			}
			++s_multiShotLevel;
			return true;
		case SpecialSkillType::Homing:
			if (s_homingLevel >= SPECIAL_SKILL_MAX_LEVEL)
			{
				return false;
			}
			++s_homingLevel;
			return true;
		}
		return false;
	}

	// 特殊スキル選択が完了したことを通知する
	void ConsumeSpecialSkillPick()
	{
		s_pendingSpecialSkillPick = false;
	}

	// ゲームオーバー状態をセットする
	void SetGameOver()
	{
		s_outcome = GameOutcome::GameOver;
		UpdateHighScore();
	}

	// 各種パラメータのゲッター
	GameOutcome GetOutcome() { return s_outcome; }
	int GetRound() { return s_round; }
	int GetScore() { return s_score; }
	int GetHighScore() { return s_highScore; }
	int GetRoundsCleared() { return s_round - 1; }
	bool ShouldPickSpecialSkill() { return s_pendingSpecialSkillPick; }

	// 通常スキルの現在レベルを取得
	int GetSkillLevel(SkillType skill)
	{
		switch (skill)
		{
		case SkillType::FireRate: return s_fireRateLevel;
		case SkillType::ChargeBoost: return s_chargeLevel;
		case SkillType::Damage: return s_damageLevel;
		}
		return 0;
	}

	// 特殊スキルの現在レベルを取得
	int GetSpecialSkillLevel(SpecialSkillType skill)
	{
		switch (skill)
		{
		case SpecialSkillType::MultiShot: return s_multiShotLevel;
		case SpecialSkillType::Homing: return s_homingLevel;
		}
		return 0;
	}

	// 特殊スキルがまだ上限（最大レベル）未満で、アップグレード可能か判定
	bool CanUpgradeSpecialSkill(SpecialSkillType skill)
	{
		return GetSpecialSkillLevel(skill) < SPECIAL_SKILL_MAX_LEVEL;
	}

	// マルチショット時に一度に発射される総弾数（スキルレベル1につき1発追加）
	int GetMultiShotCount()
	{
		if (s_multiShotLevel <= 0)
		{
			return 1;
		}
		return 1 + s_multiShotLevel;
	}

	// 追尾（ホーミング）弾の旋回力（レベル1で0.058、以降レベル毎に0.018ずつ上昇）
	float GetHomingTurnRate()
	{
		if (s_homingLevel <= 0)
		{
			return 0.0f;
		}
		return HOMING_TURN_RATE_BASE + (float)s_homingLevel * HOMING_TURN_RATE_PER_LEVEL;
	}

	// ホーミング能力をすでに取得しているかを判定
	bool HasHoming()
	{
		return s_homingLevel > 0;
	}

	// 一般敵出現時の最小HP値を取得
	int GetEnemyHpMin() { return ENEMY_HP_MIN + s_totalHpBonus; }
	// 一般敵出現時の最大HP値を取得
	int GetEnemyHpMax() { return ENEMY_HP_MAX + s_totalHpBonus; }

	// 中ボス出現時のHP値を取得（出現回数に応じて補正がかかる）
	int GetMidBossHp()
	{
		++s_midBossAppearances;
		return MID_BOSS_BASE_HP + MID_BOSS_HP_GAIN_PER_APPEARANCE * (s_midBossAppearances - 1);
	}

	// 大ボス出現時のHP値を取得（出現回数に応じて補正がかかる）
	int GetBossHp()
	{
		++s_bossAppearances;
		return BOSS_BASE_HP + BOSS_HP_GAIN_PER_APPEARANCE * (s_bossAppearances - 1);
	}

	// 自動連射速度を適用した射撃の間隔フレーム数（最小限界値5フレーム）
	int GetAutoFireInterval()
	{
		const int interval = AUTO_FIRE_INTERVAL - s_fireRateLevel * FIRE_RATE_SKILL_REDUCTION;
		return (interval < AUTO_FIRE_INTERVAL_MIN) ? AUTO_FIRE_INTERVAL_MIN : interval;
	}

	// 1クリックあたりのチャージ蓄積効率
	int GetChargePerClick() { return 1 + s_chargeLevel; }
	// 氷弾の基本ダメージ量（レベルアップ毎に+1ダメージ）
	int GetIceDamage() { return ICE_DAMAGE_BASE + s_damageLevel; }

	// 各種通常スキルの表示名（英語名）
	const char* GetSkillName(SkillType skill)
	{
		switch (skill)
		{
		case SkillType::FireRate: return "Fire Rate Up";
		case SkillType::ChargeBoost: return "Charge Boost";
		case SkillType::Damage: return "Damage Up";
		}
		return "";
	}

	// 各種通常スキルの効果説明文（英語名）
	const char* GetSkillDescription(SkillType skill)
	{
		switch (skill)
		{
		case SkillType::FireRate: return "Auto-fire shoots faster";
		case SkillType::ChargeBoost: return "More balls per click in charge phase";
		case SkillType::Damage: return "Ice balls deal more damage";
		}
		return "";
	}

	// 各種特殊スキルの表示名（英語名）
	const char* GetSpecialSkillName(SpecialSkillType skill)
	{
		switch (skill)
		{
		case SpecialSkillType::MultiShot: return "Multi Shot";
		case SpecialSkillType::Homing: return "Homing Ice";
		}
		return "";
	}

	// 各種特殊スキルの効果説明文（英語名）
	const char* GetSpecialSkillDescription(SpecialSkillType skill)
	{
		switch (skill)
		{
		case SpecialSkillType::MultiShot:
			return "Lv up: more ice balls per shot (2,3,4...)";
		case SpecialSkillType::Homing:
			return "Lv up: stronger homing pull toward enemies";
		}
		return "";
	}

	// スキルレベルを外部から強制セットする（デバッグ用）
	void SetSkillLevel(SkillType s, int lvl)
	{
		if (lvl < 0) lvl = 0;
		switch (s)
		{
		case SkillType::FireRate: s_fireRateLevel = lvl; break;
		case SkillType::ChargeBoost: s_chargeLevel = lvl; break;
		case SkillType::Damage: s_damageLevel = lvl; break;
		}
	}

	// 特殊スキルレベルを外部から強制セットする（デバッグ用）
	void SetSpecialSkillLevel(SpecialSkillType s, int lvl)
	{
		if (lvl < 0) lvl = 0;
		if (lvl > SPECIAL_SKILL_MAX_LEVEL) lvl = SPECIAL_SKILL_MAX_LEVEL;
		switch (s)
		{
		case SpecialSkillType::MultiShot: s_multiShotLevel = lvl; break;
		case SpecialSkillType::Homing: s_homingLevel = lvl; break;
		}
	}

	// 現在のラウンド値を外部から設定する（難易度HPボーナスもラウンドに合わせて自動で累積計算する）
	void SetRound(int r)
	{
		if (r < 1) r = 1;
		s_round = r;
		s_totalHpBonus = 0;
		for (int i = 1; i < s_round; ++i)
		{
			if (i >= ROUND_HIGH_HP_THRESHOLD)
			{
				const int range = ENEMY_HP_BONUS_HIGH_MAX - ENEMY_HP_BONUS_HIGH_MIN + 1;
				s_totalHpBonus += GetRand(range) + ENEMY_HP_BONUS_HIGH_MIN;
			}
			else
			{
				const int range = ENEMY_HP_BONUS_MAX - ENEMY_HP_BONUS_MIN + 1;
				s_totalHpBonus += GetRand(range) + ENEMY_HP_BONUS_MIN;
			}
		}
	}

	// 現在のラウンドに基づく最大チャージ可能弾数を算出する（5ラウンド進むごとに最大弾数+20発）
	int GetMaxChargeShots()
	{
		return 200 + ((s_round - 1) / 5) * 20;
	}
}
