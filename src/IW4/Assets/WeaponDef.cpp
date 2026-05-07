// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/WeaponDef.hpp"
#include <cmath>

namespace ZoneTool
{
	namespace IW4
	{
		static float safe_f(float v)
		{
			if (std::isnan(v) || std::isinf(v)) return 0.0f;
			return v;
		}

		static const char* safe_s(const char* s)
		{
			return s ? s : "";
		}

		static const char* ptr_bool(void* p)
		{
			return p ? "true" : "false";
		}

		static void dump_f(Json& data, const char* key, float val)
		{
			data[key] = safe_f(val);
		}

		static void dump_s(Json& data, const char* key, const char* val)
		{
			data[key] = val ? val : "";
		}

		static void dump_p(Json& data, const char* key, void* val)
		{
			data[key] = val ? "true" : "false";
		}

		static Json dump_weapondef_iw4(WeaponDef* asset, const std::function<const char* (uint16_t)>& convertToString)
		{
			Json data;

			dump_s(data, "szOverlayName", asset->szOverlayName);
			dump_s(data, "szModeName", asset->szModeName);
			data["playerAnimType"] = asset->playerAnimType;
			data["weapType"] = asset->weapType;
			data["weapClass"] = asset->weapClass;
			data["penetrateType"] = asset->penetrateType;
			data["inventoryType"] = asset->inventoryType;
			data["fireType"] = asset->fireType;
			data["offhandClass"] = asset->offhandClass;
			data["stance"] = asset->stance;

			for (int i = 0; i < 24; i++)
			{
				if (asset->notetrackSoundMapKeys && asset->notetrackSoundMapKeys[i])
				{
					const char* s = convertToString(asset->notetrackSoundMapKeys[i]);
					data["notetrackSoundMapKeys"][i] = s ? s : "";
				}
				else
				{
					data["notetrackSoundMapKeys"][i] = "";
				}

				if (asset->notetrackSoundMapValues && asset->notetrackSoundMapValues[i])
				{
					const char* s = convertToString(asset->notetrackSoundMapValues[i]);
					data["notetrackSoundMapValues"][i] = s ? s : "";
				}
				else
				{
					data["notetrackSoundMapValues"][i] = "";
				}
			}

			for (int i = 0; i < 16; i++)
			{
				if (asset->notetrackRumbleMapKeys && asset->notetrackRumbleMapKeys[i])
				{
					const char* s = convertToString(asset->notetrackRumbleMapKeys[i]);
					data["notetrackRumbleMapKeys"][i] = s ? s : "";
				}
				else
				{
					data["notetrackRumbleMapKeys"][i] = "";
				}

				if (asset->notetrackRumbleMapValues && asset->notetrackRumbleMapValues[i])
				{
					const char* s = convertToString(asset->notetrackRumbleMapValues[i]);
					data["notetrackRumbleMapValues"][i] = s ? s : "";
				}
				else
				{
					data["notetrackRumbleMapValues"][i] = "";
				}
			}

			for (int i = 0; i < 42; i++)
			{
				if (i < 37 && asset->szXAnimsRightHanded && asset->szXAnimsRightHanded[i])
				{
					data["szXAnimsRightHanded"][i] = asset->szXAnimsRightHanded[i];
				}
				else
				{
					data["szXAnimsRightHanded"][i] = "";
				}

				if (i < 37 && asset->szXAnimsLeftHanded && asset->szXAnimsLeftHanded[i])
				{
					data["szXAnimsLeftHanded"][i] = asset->szXAnimsLeftHanded[i];
				}
				else
				{
					data["szXAnimsLeftHanded"][i] = "";
				}
			}

			{
				auto ol = &data["overlay"];
				dump_p(*ol, "shader", asset->overlayMaterial);
				dump_p(*ol, "shaderLowRes", asset->overlayMaterialLowRes);
				dump_p(*ol, "shaderEMP", asset->overlayMaterialEMP);
				dump_p(*ol, "shaderEMPLowRes", asset->overlayMaterialEMPLowRes);
				(*ol)["reticle"] = asset->overlayReticle;
				dump_f(*ol, "width", asset->overlayWidth);
				dump_f(*ol, "height", asset->overlayHeight);
				dump_f(*ol, "widthSplitscreen", asset->overlayWidthSplitscreen);
				dump_f(*ol, "heightSplitscreen", asset->overlayHeightSplitscreen);
			}
			data["overlayInterface"] = asset->overlayInterface;

			for (int i = 0; i < 16; i++)
			{
				if (asset->gunXModel && asset->gunXModel[i])
				{
					data["gunXModel"][i] = asset->gunXModel[i]->name ? asset->gunXModel[i]->name : "";
				}
				else
				{
					data["gunXModel"][i] = "";
				}

				if (asset->worldModel && asset->worldModel[i])
				{
					data["worldXModel"][i] = asset->worldModel[i]->name ? asset->worldModel[i]->name : "";
				}
				else
				{
					data["worldXModel"][i] = "";
				}
			}

			dump_f(data, "fAdsBobFactor", asset->fAdsBobFactor);
			dump_f(data, "fAdsViewBobMult", asset->fAdsViewBobMult);
			dump_f(data, "fHipSpreadStandMin", asset->fHipSpreadStandMin);
			dump_f(data, "fHipSpreadDuckedMin", asset->fHipSpreadDuckedMin);
			dump_f(data, "fHipSpreadProneMin", asset->fHipSpreadProneMin);
			dump_f(data, "hipSpreadStandMax", asset->hipSpreadStandMax);
			dump_f(data, "hipSpreadDuckedMax", asset->hipSpreadDuckedMax);
			dump_f(data, "hipSpreadProneMax", asset->hipSpreadProneMax);
			dump_f(data, "fHipSpreadDecayRate", asset->fHipSpreadDecayRate);
			dump_f(data, "fHipSpreadFireAdd", asset->fHipSpreadFireAdd);
			dump_f(data, "fHipSpreadTurnAdd", asset->fHipSpreadTurnAdd);
			dump_f(data, "fHipSpreadMoveAdd", asset->fHipSpreadMoveAdd);
			dump_f(data, "fHipSpreadDuckedDecay", asset->fHipSpreadDuckedDecay);
			dump_f(data, "fHipSpreadProneDecay", asset->fHipSpreadProneDecay);
			dump_f(data, "fHipReticleSidePos", asset->fHipReticleSidePos);
			dump_f(data, "fAdsIdleAmount", asset->fAdsIdleAmount);
			dump_f(data, "fHipIdleAmount", asset->fHipIdleAmount);
			dump_f(data, "adsIdleSpeed", asset->adsIdleSpeed);
			dump_f(data, "hipIdleSpeed", asset->hipIdleSpeed);
			dump_f(data, "fIdleCrouchFactor", asset->fIdleCrouchFactor);
			dump_f(data, "fIdleProneFactor", asset->fIdleProneFactor);
			dump_f(data, "fGunMaxPitch", asset->fGunMaxPitch);
			dump_f(data, "fGunMaxYaw", asset->fGunMaxYaw);
			dump_f(data, "adsIdleLerpStartTime", 0.5f);
			dump_f(data, "adsIdleLerpTime", 4.0f);

			dump_f(data, "swayMaxAngle", asset->swayMaxAngle);
			dump_f(data, "swayLerpSpeed", asset->swayLerpSpeed);
			dump_f(data, "swayPitchScale", asset->swayPitchScale);
			dump_f(data, "swayYawScale", asset->swayYawScale);
			dump_f(data, "swayHorizScale", asset->swayHorizScale);
			dump_f(data, "swayVertScale", asset->swayVertScale);
			dump_f(data, "swayShellShockScale", asset->swayShellShockScale);
			dump_f(data, "adsSwayMaxAngle", asset->adsSwayMaxAngle);
			dump_f(data, "adsSwayLerpSpeed", asset->adsSwayLerpSpeed);
			dump_f(data, "adsSwayPitchScale", asset->adsSwayPitchScale);
			dump_f(data, "adsSwayYawScale", asset->adsSwayYawScale);
			dump_f(data, "adsSwayHorizScale", asset->adsSwayHorizScale);
			dump_f(data, "adsSwayVertScale", asset->adsSwayVertScale);
			dump_f(data, "adsViewErrorMin", asset->adsViewErrorMin);
			dump_f(data, "adsViewErrorMax", asset->adsViewErrorMax);

			dump_p(data, "physCollmap", asset->physCollmap);
			dump_f(data, "dualWieldViewModelOffset", asset->dualWieldViewModelOffset);

			dump_p(data, "reticleCenter", asset->reticleCenter);
			dump_p(data, "reticleSide", asset->reticleSide);
			data["iReticleCenterSize"] = asset->iReticleCenterSize;
			data["iReticleSideSize"] = asset->iReticleSideSize;
			data["iReticleMinOfs"] = asset->iReticleMinOfs;
			data["activeReticleType"] = asset->activeReticleType;

			dump_p(data, "hudIcon", asset->hudIcon);
			dump_f(data, "hudIconRatio", static_cast<float>(asset->hudIconRatio));
			dump_p(data, "pickupIcon", asset->pickupIcon);
			dump_f(data, "pickupIconRatio", static_cast<float>(asset->pickupIconRatio));
			dump_p(data, "ammoCounterIcon", asset->ammoCounterIcon);
			dump_f(data, "ammoCounterIconRatio", static_cast<float>(asset->ammoCounterIconRatio));
			data["ammoCounterClip"] = asset->ammoCounterClip;
			data["iStartAmmo"] = asset->iStartAmmo;
			dump_s(data, "szAmmoName", asset->szAmmoName);
			data["iAmmoIndex"] = asset->iAmmoIndex;
			dump_s(data, "szClipName", asset->szClipName);
			data["iClipIndex"] = asset->iClipIndex;
			data["iMaxAmmo"] = asset->iMaxAmmo;
			data["shotCount"] = asset->shotCount;
			dump_s(data, "szSharedAmmoCapName", asset->szSharedAmmoCapName);
			data["iSharedAmmoCapIndex"] = asset->iSharedAmmoCapIndex;
			data["iSharedAmmoCap"] = asset->iSharedAmmoCap;
			data["damage"] = asset->damage;
			data["playerDamage"] = asset->playerDamage;
			data["iMeleeDamage"] = asset->iMeleeDamage;
			data["iDamageType"] = asset->iDamageType;

			{
				auto st = &data["stateTimers"];
				(*st)["iFireDelay"] = asset->iFireDelay;
				(*st)["iMeleeDelay"] = asset->iMeleeDelay;
				(*st)["meleeChargeDelay"] = asset->meleeChargeDelay;
				(*st)["iDetonateDelay"] = asset->iDetonateDelay;
				(*st)["iRechamberTime"] = asset->iRechamberTime;
				(*st)["rechamberTimeOneHanded"] = asset->rechamberTimeOneHanded;
				(*st)["iRechamberBoltTime"] = asset->iRechamberBoltTime;
				(*st)["iHoldFireTime"] = asset->iHoldFireTime;
				(*st)["iDetonateTime"] = asset->iDetonateTime;
				(*st)["iMeleeTime"] = asset->iMeleeTime;
				(*st)["meleeChargeTime"] = asset->meleeChargeTime;
				(*st)["iReloadTime"] = asset->iReloadTime;
				(*st)["reloadShowRocketTime"] = asset->reloadShowRocketTime;
				(*st)["iReloadEmptyTime"] = asset->iReloadEmptyTime;
				(*st)["iReloadAddTime"] = asset->iReloadAddTime;
				(*st)["iReloadStartTime"] = asset->iReloadStartTime;
				(*st)["iReloadStartAddTime"] = asset->iReloadStartAddTime;
				(*st)["iReloadEndTime"] = asset->iReloadEndTime;
				(*st)["iDropTime"] = asset->iDropTime;
				(*st)["iRaiseTime"] = asset->iRaiseTime;
				(*st)["iAltDropTime"] = asset->iAltDropTime;
				(*st)["quickDropTime"] = asset->quickDropTime;
				(*st)["quickRaiseTime"] = asset->quickRaiseTime;
				(*st)["iBreachRaiseTime"] = asset->iBreachRaiseTime;
				(*st)["iEmptyRaiseTime"] = asset->iEmptyRaiseTime;
				(*st)["iEmptyDropTime"] = asset->iEmptyDropTime;
				(*st)["sprintInTime"] = asset->sprintInTime;
				(*st)["sprintLoopTime"] = asset->sprintLoopTime;
				(*st)["sprintOutTime"] = asset->sprintOutTime;
				(*st)["stunnedTimeBegin"] = asset->stunnedTimeBegin;
				(*st)["stunnedTimeLoop"] = asset->stunnedTimeLoop;
				(*st)["stunnedTimeEnd"] = asset->stunnedTimeEnd;
				(*st)["nightVisionWearTime"] = asset->nightVisionWearTime;
				(*st)["nightVisionWearTimeFadeOutEnd"] = asset->nightVisionWearTimeFadeOutEnd;
				(*st)["nightVisionWearTimePowerUp"] = asset->nightVisionWearTimePowerUp;
				(*st)["nightVisionRemoveTime"] = asset->nightVisionRemoveTime;
				(*st)["nightVisionRemoveTimePowerDown"] = asset->nightVisionRemoveTimePowerDown;
				(*st)["nightVisionRemoveTimeFadeInStart"] = asset->nightVisionRemoveTimeFadeInStart;
				(*st)["fuseTime"] = asset->fuseTime;
				(*st)["aiFuseTime"] = asset->aiFuseTime;
				(*st)["blastFrontTime"] = 0;
				(*st)["blastRightTime"] = 0;
				(*st)["blastBackTime"] = 0;
				(*st)["blastLeftTime"] = 0;
				(*st)["raiseInterruptableTime"] = 0;
				(*st)["firstRaiseInterruptableTime"] = 0;
				(*st)["reloadInterruptableTime"] = 0;
				(*st)["reloadEmptyInterruptableTime"] = 0;
				(*st)["fireInterruptableTime"] = 0;
			}

			{
				auto st = &data["akimboStateTimers"];
				(*st)["iFireDelay"] = 0;
				(*st)["iMeleeDelay"] = 0;
				(*st)["meleeChargeDelay"] = 0;
				(*st)["iDetonateDelay"] = 0;
				(*st)["iRechamberTime"] = 0;
				(*st)["rechamberTimeOneHanded"] = 0;
				(*st)["iRechamberBoltTime"] = 0;
				(*st)["iHoldFireTime"] = 0;
				(*st)["iDetonateTime"] = 0;
				(*st)["iMeleeTime"] = 0;
				(*st)["meleeChargeTime"] = 0;
				(*st)["iReloadTime"] = 0;
				(*st)["reloadShowRocketTime"] = 0;
				(*st)["iReloadEmptyTime"] = 0;
				(*st)["iReloadAddTime"] = 0;
				(*st)["iReloadStartTime"] = 0;
				(*st)["iReloadStartAddTime"] = 0;
				(*st)["iReloadEndTime"] = 0;
				(*st)["iDropTime"] = 0;
				(*st)["iRaiseTime"] = 0;
				(*st)["iAltDropTime"] = 0;
				(*st)["quickDropTime"] = 0;
				(*st)["quickRaiseTime"] = 0;
				(*st)["iBreachRaiseTime"] = 0;
				(*st)["iEmptyRaiseTime"] = 0;
				(*st)["iEmptyDropTime"] = 0;
				(*st)["sprintInTime"] = 0;
				(*st)["sprintLoopTime"] = 0;
				(*st)["sprintOutTime"] = 0;
				(*st)["stunnedTimeBegin"] = 0;
				(*st)["stunnedTimeLoop"] = 0;
				(*st)["stunnedTimeEnd"] = 0;
				(*st)["nightVisionWearTime"] = 0;
				(*st)["nightVisionWearTimeFadeOutEnd"] = 0;
				(*st)["nightVisionWearTimePowerUp"] = 0;
				(*st)["nightVisionRemoveTime"] = 0;
				(*st)["nightVisionRemoveTimePowerDown"] = 0;
				(*st)["nightVisionRemoveTimeFadeInStart"] = 0;
				(*st)["fuseTime"] = 0;
				(*st)["aiFuseTime"] = 0;
				(*st)["blastFrontTime"] = 0;
				(*st)["blastRightTime"] = 0;
				(*st)["blastBackTime"] = 0;
				(*st)["blastLeftTime"] = 0;
				(*st)["raiseInterruptableTime"] = 0;
				(*st)["firstRaiseInterruptableTime"] = 0;
				(*st)["reloadInterruptableTime"] = 0;
				(*st)["reloadEmptyInterruptableTime"] = 0;
				(*st)["fireInterruptableTime"] = 0;
			}

			dump_f(data, "autoAimRange", asset->autoAimRange);
			dump_f(data, "aimAssistRange", asset->aimAssistRange);
			dump_f(data, "aimAssistRangeAds", asset->aimAssistRangeAds);
			dump_f(data, "aimPadding", asset->aimPadding);
			dump_f(data, "enemyCrosshairRange", asset->enemyCrosshairRange);
			dump_f(data, "moveSpeedScale", asset->moveSpeedScale);
			dump_f(data, "adsMoveSpeedScale", asset->adsMoveSpeedScale);
			dump_f(data, "sprintDurationScale", asset->sprintDurationScale);
			dump_f(data, "fAdsZoomInFrac", asset->fAdsZoomInFrac);
			dump_f(data, "fAdsZoomOutFrac", asset->fAdsZoomOutFrac);

			dump_p(data, "viewFlashEffect", asset->viewFlashEffect);
			dump_p(data, "worldFlashEffect", asset->worldFlashEffect);
			dump_p(data, "viewShellEjectEffect", asset->viewShellEjectEffect);
			dump_p(data, "worldShellEjectEffect", asset->worldShellEjectEffect);
			dump_p(data, "viewLastShotEjectEffect", asset->viewLastShotEjectEffect);
			dump_p(data, "worldLastShotEjectEffect", asset->worldLastShotEjectEffect);
			dump_p(data, "projExplosionEffect", asset->projExplosionEffect);
			dump_p(data, "projDudEffect", asset->projDudEffect);
			dump_p(data, "projTrailEffect", asset->projTrailEffect);
			dump_p(data, "projBeaconEffect", asset->projBeaconEffect);
			dump_p(data, "projIgnitionEffect", asset->projIgnitionEffect);
			dump_s(data, "projExplosionSound", asset->projExplosionSound ? asset->projExplosionSound->name : "");
			dump_s(data, "projDudSound", asset->projDudSound ? asset->projDudSound->name : "");
			dump_s(data, "projIgnitionSound", asset->projIgnitionSound ? asset->projIgnitionSound->name : "");
			dump_p(data, "turretOverheatEffect", asset->turretOverheatEffect);
			dump_s(data, "turretOverheatSound", asset->turretOverheatSound ? asset->turretOverheatSound->name : "");
			dump_s(data, "turretBarrelSpinMaxSnd", asset->turretBarrelSpinMaxSnd ? asset->turretBarrelSpinMaxSnd->name : "");
			for (int i = 0; i < 4; i++)
			{
				if (asset->turretBarrelSpinUpSnd && asset->turretBarrelSpinUpSnd[i])
					data["turretBarrelSpinUpSnd"][i] = safe_s(asset->turretBarrelSpinUpSnd[i]->name);
				else
					data["turretBarrelSpinUpSnd"][i] = "";
				if (asset->turretBarrelSpinDownSnd && asset->turretBarrelSpinDownSnd[i])
					data["turretBarrelSpinDownSnd"][i] = safe_s(asset->turretBarrelSpinDownSnd[i]->name);
				else
					data["turretBarrelSpinDownSnd"][i] = "";
			}

			data["iReloadAmmoAdd"] = asset->iReloadAmmoAdd;
			data["iReloadStartAdd"] = asset->iReloadStartAdd;
			data["ammoDropStockMin"] = asset->ammoDropStockMin;
			dump_f(data, "ammoDropClipPercentMin", static_cast<float>(asset->ammoDropClipPercentMin));
			dump_f(data, "ammoDropClipPercentMax", static_cast<float>(asset->ammoDropClipPercentMax));
			data["iExplosionRadius"] = asset->iExplosionRadius;
			data["iExplosionRadiusMin"] = asset->iExplosionRadiusMin;
			data["iExplosionInnerDamage"] = asset->iExplosionInnerDamage;
			data["iExplosionOuterDamage"] = asset->iExplosionOuterDamage;
			dump_f(data, "damageConeAngle", asset->damageConeAngle);
			dump_f(data, "bulletExplDmgMult", asset->bulletExplDmgMult);
			dump_f(data, "bulletExplRadiusMult", asset->bulletExplRadiusMult);
			data["iProjectileSpeed"] = asset->iProjectileSpeed;
			data["iProjectileSpeedUp"] = asset->iProjectileSpeedUp;
			data["iProjectileSpeedForward"] = asset->iProjectileSpeedForward;
			data["iProjectileActivateDist"] = asset->iProjectileActivateDist;
			dump_f(data, "projLifetime", asset->projLifetime);
			dump_f(data, "timeToAccelerate", asset->timeToAccelerate);
			dump_f(data, "projectileCurvature", asset->projectileCurvature);
			dump_p(data, "projectileModel", asset->projectileModel);
			data["projExplosion"] = asset->projExplosion;
			dump_f(data, "stickiness", static_cast<float>(asset->stickiness));
			dump_f(data, "lowAmmoWarningThreshold", asset->lowAmmoWarningThreshold);
			dump_f(data, "ricochetChance", asset->ricochetChance);
			data["riotShieldEnableDamage"] = false;
			data["riotShieldHealth"] = 0;
			dump_f(data, "riotShieldDamageMult", 0.0f);

			for (int i = 0; i < 3; i++) data["vStandMove"][i] = safe_f(asset->vStandMove[i]);
			for (int i = 0; i < 3; i++) data["vStandRot"][i] = safe_f(asset->vStandRot[i]);
			for (int i = 0; i < 3; i++) data["strafeMove"][i] = safe_f(asset->strafeMove[i]);
			for (int i = 0; i < 3; i++) data["strafeRot"][i] = safe_f(asset->strafeRot[i]);
			for (int i = 0; i < 3; i++) data["vDuckedOfs"][i] = safe_f(asset->vDuckedOfs[i]);
			for (int i = 0; i < 3; i++) data["vDuckedMove"][i] = safe_f(asset->vDuckedMove[i]);
			for (int i = 0; i < 3; i++) data["vDuckedRot"][i] = safe_f(asset->vDuckedRot[i]);
			for (int i = 0; i < 3; i++) data["vProneOfs"][i] = safe_f(asset->vProneOfs[i]);
			for (int i = 0; i < 3; i++) data["vProneMove"][i] = safe_f(asset->vProneMove[i]);
			for (int i = 0; i < 3; i++) data["vProneRot"][i] = safe_f(asset->vProneRot[i]);

			for (int i = 0; i < 3; i++) data["vProjectileColor"][i] = safe_f(asset->vProjectileColor[i]);

			dump_f(data, "iPositionReloadTransTime", asset->iPositionReloadTransTime);
			dump_f(data, "leftArc", asset->leftArc);
			dump_f(data, "rightArc", asset->rightArc);
			dump_f(data, "topArc", asset->topArc);
			dump_f(data, "bottomArc", asset->bottomArc);
			dump_f(data, "accuracy", asset->accuracy);
			dump_f(data, "aiSpread", asset->aiSpread);
			dump_f(data, "playerSpread", asset->playerSpread);
			for (int i = 0; i < 2; i++) data["minTurnSpeed"][i] = safe_f(asset->minTurnSpeed[i]);
			for (int i = 0; i < 2; i++) data["maxTurnSpeed"][i] = safe_f(asset->maxTurnSpeed[i]);
			dump_f(data, "pitchConvergenceTime", asset->pitchConvergenceTime);
			dump_f(data, "yawConvergenceTime", asset->yawConvergenceTime);
			dump_f(data, "suppressTime", asset->suppressTime);
			dump_f(data, "maxRange", asset->maxRange);
			dump_f(data, "fAnimHorRotateInc", asset->fAnimHorRotateInc);
			dump_f(data, "fPlayerPositionDist", asset->fPlayerPositionDist);
			dump_s(data, "szUseHintString", asset->szUseHintString);
			dump_s(data, "dropHintString", asset->dropHintString);
			data["iUseHintStringIndex"] = asset->iUseHintStringIndex;
			data["dropHintStringIndex"] = asset->dropHintStringIndex;
			dump_f(data, "horizViewJitter", asset->horizViewJitter);
			dump_f(data, "vertViewJitter", asset->vertViewJitter);
			dump_f(data, "scanSpeed", asset->scanSpeed);
			dump_f(data, "scanAccel", asset->scanAccel);
			data["scanPauseTime"] = asset->scanPauseTime;
			dump_s(data, "szScript", asset->szScript);

			for (int i = 0; i < 2; i++) data["fOOPosAnimLength"][i] = safe_f(asset->fOOPosAnimLength[i]);

			data["minDamage"] = asset->minDamage;
			data["minPlayerDamage"] = asset->minPlayerDamage;
			dump_f(data, "fMaxDamageRange", asset->fMaxDamageRange);
			dump_f(data, "fMinDamageRange", asset->fMinDamageRange);
			dump_f(data, "destabilizationRateTime", asset->destabilizationRateTime);
			dump_f(data, "destabilizationCurvatureMax", asset->destabilizationCurvatureMax);
			data["destabilizeDistance"] = asset->destabilizeDistance;

			for (int i = 0; i < 20; i++)
			{
				if (asset->locationDamageMultipliers)
					data["locationDamageMultipliers"][i] = safe_f(asset->locationDamageMultipliers[i]);
				else
					data["locationDamageMultipliers"][i] = 0.0f;
			}

			dump_p(data, "tracerType", asset->tracerType);
			dump_s(data, "fireRumble", asset->fireRumble);
			dump_s(data, "meleeImpactRumble", asset->meleeImpactRumble);
			dump_s(data, "turretBarrelSpinRumble", asset->turretBarrelSpinRumble);

			dump_f(data, "turretScopeZoomRate", asset->turretScopeZoomRate);
			dump_f(data, "turretScopeZoomMin", asset->turretScopeZoomMin);
			dump_f(data, "turretScopeZoomMax", asset->turretScopeZoomMax);
			dump_f(data, "turretOverheatUpRate", asset->turretOverheatUpRate);
			dump_f(data, "turretOverheatDownRate", asset->turretOverheatDownRate);
			dump_f(data, "turretOverheatPenalty", asset->turretOverheatPenalty);
			data["turretADSEnabled"] = false;
			dump_f(data, "turretADSTime", 0.0f);
			dump_f(data, "turretFov", 0.0f);
			dump_f(data, "turretFovADS", 0.0f);
			dump_f(data, "turretBarrelSpinSpeed", asset->turretBarrelSpinSpeed);
			dump_f(data, "turretBarrelSpinUpTime", asset->turretBarrelSpinUpTime);
			dump_f(data, "turretBarrelSpinDownTime", asset->turretBarrelSpinDownTime);

			dump_p(data, "handXModel", asset->handXModel);
			dump_p(data, "worldClipModel", asset->worldClipModel);
			dump_p(data, "rocketModel", asset->rocketModel);
			dump_p(data, "knifeModel", asset->knifeModel);
			dump_p(data, "worldKnifeModel", asset->worldKnifeModel);

			dump_f(data, "fPosMoveRate", asset->fPosMoveRate);
			dump_f(data, "fPosProneMoveRate", asset->fPosProneMoveRate);
			dump_f(data, "fStandMoveMinSpeed", asset->fStandMoveMinSpeed);
			dump_f(data, "fDuckedMoveMinSpeed", asset->fDuckedMoveMinSpeed);
			dump_f(data, "fProneMoveMinSpeed", asset->fProneMoveMinSpeed);
			dump_f(data, "fPosRotRate", asset->fPosRotRate);
			dump_f(data, "fPosProneRotRate", asset->fPosProneRotRate);
			dump_f(data, "fStandRotMinSpeed", asset->fStandRotMinSpeed);
			dump_f(data, "fDuckedRotMinSpeed", asset->fDuckedRotMinSpeed);
			dump_f(data, "fProneRotMinSpeed", asset->fProneRotMinSpeed);

			data["guidedMissileType"] = asset->guidedMissileType;
			dump_f(data, "maxSteeringAccel", asset->maxSteeringAccel);
			data["projIgnitionDelay"] = asset->projIgnitionDelay;

			dump_f(data, "fAdsAimPitch", asset->fAdsAimPitch);
			dump_f(data, "fAdsCrosshairInFrac", asset->fAdsCrosshairInFrac);
			dump_f(data, "fAdsCrosshairOutFrac", asset->fAdsCrosshairOutFrac);
			data["adsGunKickReducedKickBullets"] = asset->adsGunKickReducedKickBullets;
			dump_f(data, "adsGunKickReducedKickPercent", asset->adsGunKickReducedKickPercent);
			dump_f(data, "fAdsGunKickPitchMin", asset->fAdsGunKickPitchMin);
			dump_f(data, "fAdsGunKickPitchMax", asset->fAdsGunKickPitchMax);
			dump_f(data, "fAdsGunKickYawMin", asset->fAdsGunKickYawMin);
			dump_f(data, "fAdsGunKickYawMax", asset->fAdsGunKickYawMax);
			dump_f(data, "fAdsGunKickAccel", asset->fAdsGunKickAccel);
			dump_f(data, "fAdsGunKickSpeedMax", asset->fAdsGunKickSpeedMax);
			dump_f(data, "fAdsGunKickSpeedDecay", asset->fAdsGunKickSpeedDecay);
			dump_f(data, "fAdsGunKickStaticDecay", asset->fAdsGunKickStaticDecay);
			dump_f(data, "fAdsViewKickPitchMin", asset->fAdsViewKickPitchMin);
			dump_f(data, "fAdsViewKickPitchMax", asset->fAdsViewKickPitchMax);
			dump_f(data, "fAdsViewKickYawMin", asset->fAdsViewKickYawMin);
			dump_f(data, "fAdsViewKickYawMax", asset->fAdsViewKickYawMax);
			dump_f(data, "fAdsViewScatterMin", asset->fAdsViewScatterMin);
			dump_f(data, "fAdsViewScatterMax", asset->fAdsViewScatterMax);
			dump_f(data, "fAdsSpread", asset->fAdsSpread);
			data["hipGunKickReducedKickBullets"] = asset->hipGunKickReducedKickBullets;
			dump_f(data, "hipGunKickReducedKickPercent", asset->hipGunKickReducedKickPercent);
			dump_f(data, "fHipGunKickPitchMin", asset->fHipGunKickPitchMin);
			dump_f(data, "fHipGunKickPitchMax", asset->fHipGunKickPitchMax);
			dump_f(data, "fHipGunKickYawMin", asset->fHipGunKickYawMin);
			dump_f(data, "fHipGunKickYawMax", asset->fHipGunKickYawMax);
			dump_f(data, "fHipGunKickAccel", asset->fHipGunKickAccel);
			dump_f(data, "fHipGunKickSpeedMax", asset->fHipGunKickSpeedMax);
			dump_f(data, "fHipGunKickSpeedDecay", asset->fHipGunKickSpeedDecay);
			dump_f(data, "fHipGunKickStaticDecay", asset->fHipGunKickStaticDecay);
			dump_f(data, "fHipViewKickPitchMin", asset->fHipViewKickPitchMin);
			dump_f(data, "fHipViewKickPitchMax", asset->fHipViewKickPitchMax);
			dump_f(data, "fHipViewKickYawMin", asset->fHipViewKickYawMin);
			dump_f(data, "fHipViewKickYawMax", asset->fHipViewKickYawMax);
			dump_f(data, "fHipViewScatterMin", asset->fHipViewScatterMin);
			dump_f(data, "fHipViewScatterMax", asset->fHipViewScatterMax);

			dump_f(data, "fightDist", asset->fightDist);
			dump_f(data, "maxDist", asset->maxDist);

			for (int i = 0; i < 48; i++)
			{
				if (asset->sounds && i < 47 && asset->sounds[i])
				{
					data["sounds"][i] = asset->sounds[i]->name ? asset->sounds[i]->name : "";
				}
				else
				{
					data["sounds"][i] = "";
				}
			}

			for (int i = 0; i < 31; i++)
			{
				if (asset->bounceSound && asset->bounceSound[i])
					data["bounceSound"][i] = safe_s(asset->bounceSound[i]->name);
				else
					data["bounceSound"][i] = "";
				data["rollingSound"][i] = "";
			}

			dump_s(data, "accuracyGraphName[0]", asset->accuracyGraphName[0]);
			dump_s(data, "accuracyGraphName[1]", asset->accuracyGraphName[1]);

			data["originalAccuracyGraphKnotCount"] = asset->originalAccuracyGraphKnotCount[0];

			dump_f(data, "killIconRatio", static_cast<float>(asset->killIconRatio));

			data["sharedAmmo"] = asset->sharedAmmo;
			data["lockonSupported"] = asset->lockonSupported;
			data["requireLockonToFire"] = asset->requireLockonToFire;
			data["isAirburstWeapon"] = false;
			data["bigExplosion"] = asset->bigExplosion;
			data["noAdsWhenMagEmpty"] = asset->noAdsWhenMagEmpty;
			data["avoidDropCleanup"] = asset->avoidDropCleanup;
			data["inheritsPerks"] = asset->inheritsPerks;
			data["crosshairColorChange"] = asset->crosshairColorChange;
			data["bRifleBullet"] = asset->bRifleBullet;
			data["armorPiercing"] = asset->armorPiercing;
			data["bBoltAction"] = asset->bBoltAction;
			data["aimDownSight"] = asset->aimDownSight;
			data["canHoldBreath"] = false;
			data["canVariableZoom"] = false;
			data["bRechamberWhileAds"] = asset->bRechamberWhileAds;
			data["bBulletExplosiveDamage"] = asset->bBulletExplosiveDamage;
			data["bCookOffHold"] = asset->bCookOffHold;
			data["bClipOnly"] = asset->bClipOnly;
			data["noAmmoPickup"] = asset->noAmmoPickup;
			data["adsFireOnly"] = asset->adsFireOnly;
			data["cancelAutoHolsterWhenEmpty"] = asset->cancelAutoHolsterWhenEmpty;
			data["disableSwitchToWhenEmpty"] = asset->disableSwitchToWhenEmpty;
			data["suppressAmmoReserveDisplay"] = asset->suppressAmmoReserveDisplay;
			data["laserSightDuringNightvision"] = asset->laserSightDuringNightvision;
			data["markableViewmodel"] = asset->markableViewmodel;
			data["noDualWield"] = asset->noDualWield;
			data["flipKillIcon"] = asset->flipKillIcon;
			data["bNoPartialReload"] = asset->bNoPartialReload;
			data["bSegmentedReload"] = asset->bSegmentedReload;
			data["blocksProne"] = asset->blocksProne;
			data["silenced"] = asset->silenced;
			data["isRollingGrenade"] = asset->isRollingGrenade;
			data["projExplosionEffectForceNormalUp"] = asset->projExplosionEffectForceNormalUp;
			data["bProjImpactExplode"] = asset->bProjImpactExplode;
			data["stickToPlayers"] = asset->stickToPlayers;
			data["stickToVehicles"] = false;
			data["stickToTurrets"] = false;
			data["hasDetonator"] = asset->hasDetonator;
			data["disableFiring"] = asset->disableFiring;
			data["timedDetonation"] = asset->timedDetonation;
			data["rotate"] = asset->rotate;
			data["holdButtonToThrow"] = asset->holdButtonToThrow;
			data["freezeMovementWhenFiring"] = asset->freezeMovementWhenFiring;
			data["thermalScope"] = asset->thermalScope;
			data["altModeSameWeapon"] = asset->altModeSameWeapon;
			data["turretBarrelSpinEnabled"] = asset->turretBarrelSpinEnabled;
			data["missileConeSoundEnabled"] = asset->missileConeSoundEnabled;
			data["missileConeSoundPitchshiftEnabled"] = asset->missileConeSoundPitchshiftEnabled;
			data["missileConeSoundCrossfadeEnabled"] = asset->missileConeSoundCrossfadeEnabled;
			data["offhandHoldIsCancelable"] = asset->offhandHoldIsCancelable;
			data["doNotAllowAttachmentsToOverrideSpread"] = false;
			dump_s(data, "stowTag", "");
			dump_p(data, "stowOffsetModel", nullptr);
			data["killcamTreeType"] = -1;
			data["killcamDofType"] = -1;

			// missile cone fields
			dump_s(data, "missileConeSoundAlias", asset->missileConeSoundAlias ? asset->missileConeSoundAlias->name : "");
			dump_s(data, "missileConeSoundAliasAtBase", asset->missileConeSoundAliasAtBase ? asset->missileConeSoundAliasAtBase->name : "");
			dump_f(data, "missileConeSoundRadiusAtTop", asset->missileConeSoundRadiusAtTop);
			dump_f(data, "missileConeSoundRadiusAtBase", asset->missileConeSoundRadiusAtBase);
			dump_f(data, "missileConeSoundHeight", asset->missileConeSoundHeight);
			dump_f(data, "missileConeSoundOriginOffset", asset->missileConeSoundOriginOffset);
			dump_f(data, "missileConeSoundVolumescaleAtCore", asset->missileConeSoundVolumescaleAtCore);
			dump_f(data, "missileConeSoundVolumescaleAtEdge", asset->missileConeSoundVolumescaleAtEdge);
			dump_f(data, "missileConeSoundVolumescaleCoreSize", asset->missileConeSoundVolumescaleCoreSize);
			dump_f(data, "missileConeSoundPitchAtTop", asset->missileConeSoundPitchAtTop);
			dump_f(data, "missileConeSoundPitchAtBottom", asset->missileConeSoundPitchAtBottom);
			dump_f(data, "missileConeSoundPitchTopSize", asset->missileConeSoundPitchTopSize);
			dump_f(data, "missileConeSoundPitchBottomSize", asset->missileConeSoundPitchBottomSize);
			dump_f(data, "missileConeSoundCrossfadeTopSize", asset->missileConeSoundCrossfadeTopSize);
			dump_f(data, "missileConeSoundCrossfadeBottomSize", asset->missileConeSoundCrossfadeBottomSize);

			return data;
		}

		static Json dump_complete_iw4(WeaponCompleteDef* asset, const std::function<const char* (uint16_t)>& convertToString)
		{
			Json data;
			dump_s(data, "baseAsset", asset->szInternalName);
			dump_s(data, "szInternalName", asset->szInternalName);
			data["weapDef"] = dump_weapondef_iw4(asset->weapDef, convertToString);

			for (int i = 0; i < 32; i++)
			{
				if (asset->hideTags && i < 16 && asset->hideTags[i])
				{
					const char* s = convertToString(asset->hideTags[i]);
					data["hideTags"][i] = s ? s : "";
				}
				else
				{
					data["hideTags"][i] = "";
				}
			}

			for (int i = 0; i < 6; i++) data["scopes"][i] = "";
			for (int i = 0; i < 3; i++) data["underBarrels"][i] = "";
			for (int i = 0; i < 4; i++) data["others"][i] = "";

			for (int i = 0; i < 42; i++)
			{
				if (i < 37 && asset->szXAnims && asset->szXAnims[i])
				{
					data["szXAnims"][i] = asset->szXAnims[i];
				}
				else
				{
					data["szXAnims"][i] = "";
				}
			}

			dump_s(data, "displayName", asset->szDisplayName);
			data["numAnimOverrides"] = 0;
			data["numSoundOverrides"] = 0;
			data["numFXOverrides"] = 0;
			data["numReloadStateTimerOverrides"] = 0;
			data["numNotetrackOverrides"] = 0;
			dump_f(data, "fAdsZoomFov", asset->fAdsZoomFov);
			data["iAdsTransInTime"] = asset->iAdsTransInTime;
			data["iAdsTransOutTime"] = asset->iAdsTransOutTime;
			data["iClipSize"] = asset->iClipSize;
			data["impactType"] = asset->impactType;
			data["iFireTime"] = asset->iFireTime;
			dump_f(data, "dpadIconRatio", static_cast<float>(asset->dpadIconRatio));
			dump_f(data, "penetrateMultiplier", asset->penetrateMultiplier);
			dump_f(data, "fAdsViewKickCenterSpeed", asset->fAdsViewKickCenterSpeed);
			dump_f(data, "fHipViewKickCenterSpeed", asset->fHipViewKickCenterSpeed);
			dump_s(data, "szAltWeaponName", asset->szAltWeaponName);
			data["altWeapon"] = asset->altWeaponIndex;
			data["iAltRaiseTime"] = asset->iAltRaiseTime;
			dump_p(data, "killIcon", asset->killIcon);
			dump_p(data, "dpadIcon", asset->dpadIcon);
			data["fireAnimLength"] = asset->fireAnimLength;
			data["iFirstRaiseTime"] = asset->iFirstRaiseTime;
			data["iFireTimeAkimbo"] = 0;
			data["iAltRaiseTimeAkimbo"] = 0;
			data["fireAnimLengthAkimbo"] = 0;
			data["iFirstRaiseTimeAkimbo"] = 0;
			data["ammoDropStockMax"] = asset->ammoDropStockMax;
			dump_f(data, "adsDofStart", asset->adsDofStart);
			dump_f(data, "adsDofEnd", asset->adsDofEnd);
			data["motionTracker"] = asset->motionTracker;
			data["enhanced"] = asset->enhanced;
			data["dpadIconShowsAmmo"] = asset->dpadIconShowsAmmo;

			data["animOverrides"] = Json::array();
			data["soundOverrides"] = Json::array();
			data["fxOverrides"] = Json::array();
			data["reloadOverrides"] = Json::array();
			data["notetrackOverrides"] = Json::array();

			return data;
		}

		void __declspec(noinline) dump_weapon_seh_wrapper(WeaponCompleteDef* weapon)
		{
			try
			{
				ZONETOOL_INFO("[IW4 Dump] Building complete JSON...");
				auto data = dump_complete_iw4(weapon, SL_ConvertToString);
				ZONETOOL_INFO("[IW4 Dump] Complete JSON built, serializing...");

				std::string json_data = data.dump(4, ' ', false, Json::error_handler_t::replace);
				ZONETOOL_INFO("[IW4 Dump] Serialized OK, size: %zu", json_data.size());

				std::string asset_name = weapon->szInternalName ? weapon->szInternalName : "unknown";
				std::string path = "weapons/mp/" + asset_name;

				auto file = FileSystem::FileOpen(path, "w");
				if (!file)
				{
					ZONETOOL_ERROR("[IW4 Dump] Failed to open file: %s", path.c_str());
					return;
				}
				fwrite(json_data.data(), json_data.size(), 1, file);
				FileSystem::FileClose(file);
				ZONETOOL_INFO("[IW4 Dump] Done: %s", asset_name.c_str());
			}
			catch (...)
			{
				ZONETOOL_ERROR("[IW4 Dump] Exception caught during weapon dump!");
			}
		}

		void IWeaponDef::dump(WeaponCompleteDef* weapon)
		{
			ZONETOOL_INFO("[IW4 Dump] Starting weapon dump: %s", weapon->szInternalName ? weapon->szInternalName : "(null)");

			if (!weapon || !weapon->weapDef)
			{
				ZONETOOL_WARNING("[IW4 Dump] Null weapon or weapDef, aborting");
				return;
			}

			dump_weapon_seh_wrapper(weapon);
		}
	}
}
