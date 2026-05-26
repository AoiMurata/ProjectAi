// =============================================================================
// GameSession.h
// =============================================================================
#pragma once

enum class GameOutcome
{
	RoundClear,
	GameOver
};

enum class SkillType
{
	FireRate,
	ChargeBoost,
	Damage
};

enum class SpecialSkillType
{
	MultiShot,
	Homing
};

enum class RoundSpawnType
{
	Normal,
	MidBoss,
	Boss
};

namespace GameSession
{
	void StartNewRun();
	void StartDebugRun();
	void OnRoundCleared(int shotsRemaining, bool bossWasDefeated);
	void ApplySkill(SkillType skill);
	bool ApplySpecialSkill(SpecialSkillType skill);
	void SetGameOver();
	void ConsumeSpecialSkillPick();

	void SetSkillLevel(SkillType s, int lvl);
	void SetSpecialSkillLevel(SpecialSkillType s, int lvl);
	void SetRound(int r);
	int GetMaxChargeShots();

	GameOutcome GetOutcome();
	int GetRound();
	int GetScore();
	int GetHighScore();
	int GetRoundsCleared();
	bool ShouldPickSpecialSkill();

	RoundSpawnType GetRoundSpawnType(int round);
	int GetSkillLevel(SkillType skill);
	int GetSpecialSkillLevel(SpecialSkillType skill);
	bool CanUpgradeSpecialSkill(SpecialSkillType skill);
	int GetMultiShotCount();
	float GetHomingTurnRate();
	bool HasHoming();

	int GetEnemyHpMin();
	int GetEnemyHpMax();
	int GetMidBossHp();
	int GetBossHp();
	int GetAutoFireInterval();
	int GetChargePerClick();
	int GetIceDamage();

	const char* GetSkillName(SkillType skill);
	const char* GetSkillDescription(SkillType skill);
	const char* GetSpecialSkillName(SpecialSkillType skill);
	const char* GetSpecialSkillDescription(SpecialSkillType skill);
}
