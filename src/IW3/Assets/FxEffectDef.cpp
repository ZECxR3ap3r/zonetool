// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/FxEffectDef.hpp"

namespace ZoneTool
{
	namespace IW3
	{
		void IFxEffectDef::dump(FxEffectDef* asset, ZoneMemory* mem)
		{
			// Convert IW3 FxEffectDef to IW5 format for dumping
			const auto iw5_fx = mem->Alloc<IW5::FxEffectDef>();

			// Copy base FxEffectDef fields (IW3 and IW5 have same layout for these)
			iw5_fx->name = asset->name;
			iw5_fx->flags = asset->flags;
			iw5_fx->totalSize = asset->totalSize;
			iw5_fx->msecLoopingLife = asset->msecLoopingLife;
			iw5_fx->elemDefCountLooping = asset->elemDefCountLooping;
			iw5_fx->elemDefCountOneShot = asset->elemDefCountOneShot;
			iw5_fx->elemDefCountEmission = asset->elemDefCountEmission;

			// IW5 has additional occlusion query fields - set defaults
			iw5_fx->occlusionQueryDepthBias = 0.0f;
			iw5_fx->occlusionQueryFadeIn = 0;
			iw5_fx->occlusionQueryFadeOut = 0;
			iw5_fx->occlusionQueryScaleRange.base = 1.0f;
			iw5_fx->occlusionQueryScaleRange.amplitude = 0.0f;

			const auto elem_count = iw5_fx->elemDefCountEmission + iw5_fx->elemDefCountOneShot + iw5_fx->elemDefCountLooping;
			iw5_fx->elemDefs = mem->Alloc<IW5::FxElemDef>(elem_count);

			for (auto i = 0; i < elem_count; i++)
			{
				auto* src = &asset->elemDefs[i];
				auto* dst = &iw5_fx->elemDefs[i];

				// Copy common fields
				dst->flags = src->flags;
				dst->spawn = *reinterpret_cast<IW5::FxSpawnDef*>(&src->spawn);
				dst->spawnRange = *reinterpret_cast<IW5::FxFloatRange*>(&src->spawnRange);
				dst->fadeInRange = *reinterpret_cast<IW5::FxFloatRange*>(&src->fadeInRange);
				dst->fadeOutRange = *reinterpret_cast<IW5::FxFloatRange*>(&src->fadeOutRange);
				dst->spawnFrustumCullRadius = src->spawnFrustumCullRadius;
				dst->spawnDelayMsec = *reinterpret_cast<IW5::FxIntRange*>(&src->spawnDelayMsec);
				dst->lifeSpanMsec = *reinterpret_cast<IW5::FxIntRange*>(&src->lifeSpanMsec);
				memcpy(dst->spawnOrigin, src->spawnOrigin, sizeof(dst->spawnOrigin));
				dst->spawnOffsetRadius = *reinterpret_cast<IW5::FxFloatRange*>(&src->spawnOffsetRadius);
				dst->spawnOffsetHeight = *reinterpret_cast<IW5::FxFloatRange*>(&src->spawnOffsetHeight);
				memcpy(dst->spawnAngles, src->spawnAngles, sizeof(dst->spawnAngles));
				memcpy(dst->angularVelocity, src->angularVelocity, sizeof(dst->angularVelocity));
				dst->initialRotation = *reinterpret_cast<IW5::FxFloatRange*>(&src->initialRotation);
				dst->gravity = *reinterpret_cast<IW5::FxFloatRange*>(&src->gravity);
				dst->reflectionFactor = *reinterpret_cast<IW5::FxFloatRange*>(&src->reflectionFactor);
				dst->atlas = *reinterpret_cast<IW5::FxElemAtlas*>(&src->atlas);

				// Convert elemType (IW3 has different enum values for types >= 5)
				dst->elemType = src->elemType;
				if (dst->elemType >= 5)
				{
					dst->elemType += 2; // IW3 MODEL=5 -> IW5 MODEL=7, etc.
				}

				dst->visualCount = src->visualCount;
				dst->velIntervalCount = src->velIntervalCount;
				dst->visStateIntervalCount = src->visStateIntervalCount;
				dst->velSamples = reinterpret_cast<IW5::FxElemVelStateSample*>(src->velSamples);
				dst->visSamples = reinterpret_cast<IW5::FxElemVisStateSample*>(src->visSamples);
				dst->visuals = *reinterpret_cast<IW5::FxElemDefVisuals*>(&src->visuals);

				// Convert collision bounds (IW3 uses Bounds, IW5 uses separate mins/maxs)
				dst->collMins[0] = src->collBounds.midPoint[0] - src->collBounds.halfSize[0];
				dst->collMins[1] = src->collBounds.midPoint[1] - src->collBounds.halfSize[1];
				dst->collMins[2] = src->collBounds.midPoint[2] - src->collBounds.halfSize[2];
				dst->collMaxs[0] = src->collBounds.midPoint[0] + src->collBounds.halfSize[0];
				dst->collMaxs[1] = src->collBounds.midPoint[1] + src->collBounds.halfSize[1];
				dst->collMaxs[2] = src->collBounds.midPoint[2] + src->collBounds.halfSize[2];

				dst->effectOnImpact = reinterpret_cast<IW5::FxEffectDefRef*>(&src->effectOnImpact);
				dst->effectOnDeath = reinterpret_cast<IW5::FxEffectDefRef*>(&src->effectOnDeath);
				dst->effectEmitted = reinterpret_cast<IW5::FxEffectDefRef*>(&src->effectEmitted);
				dst->emitDist = *reinterpret_cast<IW5::FxFloatRange*>(&src->emitDist);
				dst->emitDistVariance = *reinterpret_cast<IW5::FxFloatRange*>(&src->emitDistVariance);

				// Handle trail conversion (IW3 elemType 3 = TRAIL)
				if (src->elemType == 3 && src->trailDef)
				{
					dst->extended.trailDef = mem->Alloc<IW5::FxTrailDef>();
					dst->extended.trailDef->scrollTimeMsec = src->trailDef->scrollTimeMsec;
					dst->extended.trailDef->repeatDist = src->trailDef->repeatDist;

					// Convert IW3 splitDist (int) to IW5 inverted float values
					const int splitDist = src->trailDef->splitDist;
					if (splitDist > 0)
					{
						dst->extended.trailDef->invSplitDist = 1.0f / static_cast<float>(splitDist);
					}
					else
					{
						dst->extended.trailDef->invSplitDist = 0.0f;
					}
					dst->extended.trailDef->invSplitArcDist = 0.0f;
					dst->extended.trailDef->invSplitTime = 0.0f;

					dst->extended.trailDef->vertCount = src->trailDef->vertCount;
					dst->extended.trailDef->verts = reinterpret_cast<IW5::FxTrailVertex*>(src->trailDef->verts);
					dst->extended.trailDef->indCount = src->trailDef->indCount;
					dst->extended.trailDef->inds = src->trailDef->inds;
				}
				else
				{
					dst->extended.trailDef = nullptr;
				}

				dst->sortOrder = src->sortOrder;
				dst->lightingFrac = src->lightingFrac;
				dst->useItemClip = src->useItemClip;
				dst->fadeInfo = 0; // IW3 has 'unused', IW5 has 'fadeInfo'
				dst->pad = 0; // IW5 only field
			}

			// Dump using IW5 format directly
			IW5::IFxEffectDef::dump(iw5_fx);
		}
	}
}
