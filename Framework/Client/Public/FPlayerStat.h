#pragma once

#include "EShell.h"
#include "ESkill.h"
#include "IDamaged.h"

BEGIN(Client)

struct FPlayerStat
{
	// 기본 Input 관련
	_bool bInputEnable{ true };
	_bool bMoveInputEnable{ true };
	const _float fInputBufferTime{ 0.4f };

	// 애니메이션 관련
	_bool bTransition{};
	_float fAnimSpeed{ 1.f };

	// Movement 관련
	const _float fAccel{ 50.f };
	const _float fDecel{ 20.f };
	const _float fMaxSpeed{ 5.f };
	const _float fForceFriction{ 20.f };
	const _float fMaxFallingSpeed{ 12.f };
	const _float fRotationScale{ 0.9f };
	const _float fGravityScale{ 7.f };

	// 달리기 관련
	_bool bRunning{};
	_bool bRunnable{};
	_float fRunMaxSpeedRatio{ 1.5f };
	const _float fRunThresholdRatio{ 0.8f };
	const _float fRunTimeThreshold{ 0.3f };
	_float fRunTimeAcc{};

	// Airbone 관련
	_bool bInAir{};
	_bool bCanAirAction{ true };
	const _float fInAirControlRatio{ 0.5f };
	const _float fInAirMaxSpeed{ 7.5f };

	// Swimming 관련
	_bool bSwimming{};
	_bool bSwimmable{};
	_float fSwimmingTimeAcc{};
	_float fSwimmingMaxTime{ 2.f };
	const _float fSwimmingFallingSpeed{ 5.f };

	// 락온 관련
	_bool bLockOn{};

	// Jump 관련
	const _float fJumpForce{ 30.f };

	// Dodge 관련
	_bool bEvade{};
	_bool bBackStep{};
	_bool bCanBackStepAttack{};

	// Grabbed 관련
	_bool bGrabbed{};

	// Attacked 관련
	_bool bDeath{};
	_bool bSuperArmor{};

	// 공격 관련
	_bool bCharging{};
	_bool bDashStart{};
	_uint iComboCount{};
	const _uint iComboMax{ 4 };
	IDamaged::DAMAGE eDamageType{};

	// 가드 / 패링 관련
	_bool bGuard{};
	_bool bParry{};
	_bool bCanParryAttack{};

	// 플레이어 스탯
	_bool bInvincible{};
	_uint iVitality{ 20 };
	_uint iResistance{ 20 };
	_uint iAttackPower{ 20 };
	_uint iMSG{ 20 };
	_uint iDefense{ 0 };
	_uint iLevel{ iVitality + iResistance + iAttackPower + iMSG };

	_float fMaxHP{ 100.f };
	_float fCurrHP{ 100.f };

	_float fDamageFactor{ 1.f };

	_float fMaxSkillPoint{ 5.f };
	_float fCurrSkillPoint{ 5.f };

	// 상호작용 관련
	_bool bInteract{};

	// 등껍질 관련
	_bool bHasShell{};

	// 등껍질 이동 관련
	_bool bRolling{};
	const _float fShellAccel{ 25.f };
	const _float fShellDecel{ 12.5f };
	const _float fShellMaxSpeed{ 2.5f };

	// 갈고리 관련
	_bool bGrappleAim{};
	_bool bGrapplable{};
	_bool bGrappling{};
	const _float fGrappleCheckDist{ 100.f };
	const _float fGrappleDist{ 40.f };
	const _float fGrappleForce{ 40.f };
	const _float fGrappleBoost{ 50.f };
	const _float fGrappleLifeTime{ 7.f };

	// 해머 관련
	_bool bCanBeHammer{};
	_bool bHasHammer{};
	_bool bCanHammerAnim{};
	const _uint iHammerComboMax{ 3 };

	// 액티브 스킬 관련
	ACTIVESKILL ePrimaryActiveSkill{ ACTIVESKILL::ACTIVESKILL_END };
	ACTIVESKILL eSecondaryActiveSkill{ ACTIVESKILL::ACTIVESKILL_END };

	_bool bFinishAttack{};
	const _float fFinishDist{ 15.f };
	_float fFinishTime{ 0.8f };

	_bool bUrchinThrow{};
	_bool bUrchinLaunch{};
	_bool bUrchinCinematic{};

	// 디버프 관련
	_bool bFeared{};
	_float fFearGauge{};
	const _float fFearMaxGauge{ 5.f };

	// 아이템 관련
	_bool bUseItem{};
	_bool bPreserveItem{};
	const _uint iMaxHookCount{ 5 };
	_uint iHookCount{ 5 };
	const _uint iMaxHeartKelpCount{ 5 };
	_uint iHeartKelpCount{ 5 };
	const _float fHeartKelpHeal{ 40.f };

	// 맵 상호작용 관련
	_bool bMagnetPulling{};
	_bool bMagnetPulled{};

	// 텔레포트 관련
	_bool bTeleport{};

	// 리스폰 관련
	_uint iShellLevel{};
	_uint iShellNum{};
	_float3 vShellPos{};
	_float3 vShellLook{};
	_float3 vShellRight{};
	const _float fRespawnTime{ 5.f };

	// 치트모드 관련
	_bool bCheat{};
	_float fCheatSpeedRatio{ 5.f };
	_float fCheatMaxSpeedRatio{ 10.f };
};

END