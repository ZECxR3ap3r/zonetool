#include "stdafx.hpp"

namespace ZoneTool
{
	namespace IW5
	{
		void parse_visuals(AssetReader* read, FxElemDef* def, FxElemVisuals* vis)
		{
			switch (def->elemType)
			{
				case FX_ELEM_TYPE_MODEL:
					vis->xmodel = read->read_asset<XModel>();
					break;
				case FX_ELEM_TYPE_RUNNER:
					vis->effectDef = read->read_asset<FxEffectDefRef>();
					break;
				case FX_ELEM_TYPE_SOUND:
					vis->soundName = read->read_string();
					break;
				default:
					if (def->elemType - 13 <= 1u)
					{
						if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
							vis->lightDef = read->read_asset<GfxLightDef>();
					}
					else
						vis->material = read->read_asset<Material>();
					break;
			}
		}

		FxEffectDef* IFxEffectDef::parse(const std::string& name, ZoneMemory* mem)
		{
			AssetReader read(mem);
			if (!read.open("fx\\"s + name + ".fxe"))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing fx \"%s\"...", name.data());

			const auto asset = read.read_single<FxEffectDef>();
			asset->name = read.read_string();
			asset->elemDefs = read.read_array<FxElemDef>();

			for (int i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				auto def = &asset->elemDefs[i];

				def->velSamples = read.read_array<FxElemVelStateSample>();
				def->visSamples = read.read_array<FxElemVisStateSample>();

				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (def->visuals.markArray)
					{
						def->visuals.markArray = read.read_array<FxElemMarkVisuals>();

						for (unsigned char j = 0; j < def->visualCount; j++)
						{
							if (def->visuals.markArray[j][0])
							{
								def->visuals.markArray[j][0] = read.read_asset<Material>();
							}
							if (def->visuals.markArray[j][1])
							{
								def->visuals.markArray[j][1] = read.read_asset<Material>();
							}
						}
					}
				}
				else if (def->visualCount > 1)
				{
					def->visuals.array = read.read_array<FxElemVisuals>();

					for (unsigned char vis = 0; vis < def->visualCount; vis++)
					{
						parse_visuals(&read, def, &def->visuals.array[vis]);
					}
				}
				else
				{
					parse_visuals(&read, def, &def->visuals.instance);
				}

				def->effectOnImpact = read.read_asset<FxEffectDefRef>();
				def->effectOnDeath = read.read_asset<FxEffectDefRef>();
				def->effectEmitted = read.read_asset<FxEffectDefRef>();

				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						def->extended.trailDef = read.read_single<FxTrailDef>();

						if (def->extended.trailDef->verts)
						{
							def->extended.trailDef->verts = read.read_array<FxTrailVertex>();
						}

						if (def->extended.trailDef->inds)
						{
							def->extended.trailDef->inds = read.read_array<unsigned short>();
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
					{
						def->extended.sparkFountainDef = read.read_single<FxSparkFountainDef>();
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						def->extended.spotLightDef = read.read_single<FxSpotLightDef>();
					}
					else
					{
						def->extended.unknownDef = read.read_single<char>();
					}
				}
			}

			read.close();

			return asset;
		}

		void IFxEffectDef::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).fx;
			}
		}

		void IFxEffectDef::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IFxEffectDef::load_depending(IZone* zone)
		{
			auto data = this->asset_;

			auto load_FxElemVisuals = [zone](FxElemDef* def, FxElemDefVisuals* vis)
			{
				auto load_visuals = [zone](FxElemDef* def, FxElemVisuals* vis)
				{
					if (!vis->anonymous)
					{
						return;
					}

					switch (def->elemType)
					{
						case FX_ELEM_TYPE_MODEL:
							zone->add_asset_of_type(xmodel, vis->xmodel->name);
							break;
						case FX_ELEM_TYPE_RUNNER:
							zone->add_asset_of_type(fx, vis->effectDef->name);
							break;
						case FX_ELEM_TYPE_SOUND:
							zone->add_asset_of_type(sound, vis->soundName);
							break;
						default:
							if (def->elemType - 10 <= 1u) {
								if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT) {
									if (vis->lightDef)
										zone->add_asset_of_type(lightdef, vis->lightDef->name);
								}
							}
							else
							{
								if (vis->material)
									zone->add_asset_of_type(material, vis->material->name);
							}
							break;
						}
				};

				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					for (unsigned char i = 0; i < def->visualCount; i++)
					{
						if (vis->markArray[i])
						{
							if (vis->markArray[i][0])
								zone->add_asset_of_type(material, vis->markArray[i][0]->name);
							if (vis->markArray[i][1])
								zone->add_asset_of_type(material, vis->markArray[i][1]->name);
						}
					}
				}
				else if (def->visualCount > 1)
				{
					for (unsigned char i = 0; i < def->visualCount; i++)
					{
						load_visuals(def, &vis->array[i]);
					}
				}
				else
				{
					load_visuals(def, &vis->instance);
				}
			};

			// Loop through frames
			for (int i = 0; i < data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission; i++)
			{
				auto& def = data->elemDefs[i];

				// Sub-FX effects
				if (def.effectEmitted)
					zone->add_asset_of_type(fx, def.effectEmitted->name);
				if (def.effectOnDeath)
					zone->add_asset_of_type(fx, def.effectOnDeath->name);
				if (def.effectOnImpact)
					zone->add_asset_of_type(fx, def.effectOnImpact->name);

				// Visuals
				load_FxElemVisuals(&def, &def.visuals);
			}
		}

		std::string IFxEffectDef::name()
		{
			return this->name_;
		}

		std::int32_t IFxEffectDef::type()
		{
			return fx;
		}

		void IFxEffectDef::write_fx_elem_visuals(IZone* zone, ZoneBuffer* buf, FxElemDef* def,
			FxElemVisuals* dest)
		{
			auto data = dest;

			switch (def->elemType)
			{
				case FX_ELEM_TYPE_MODEL:
					dest->xmodel = reinterpret_cast<XModel*>(zone->get_asset_pointer(xmodel, data->xmodel->name));
					break;
				case FX_ELEM_TYPE_RUNNER:
					dest->effectDef->name = buf->write_str(data->effectDef->name);
					break;
				case FX_ELEM_TYPE_SOUND:
					dest->soundName = buf->write_str(data->soundName);
					break;
				default:
					if (def->elemType - 10 <= 1u)
					{
						if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
						{
							dest->lightDef = reinterpret_cast<GfxLightDef*>(zone->get_asset_pointer(lightdef, data->lightDef->name));
						}
					}
					else
					{
						if (dest->material)
							dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(material, data->material->name));
					}
					break;
			}
		}

		void IFxEffectDef::write_fx_elem_def_visuals(IZone* zone, ZoneBuffer* buf, FxElemDef* def,
			FxElemDefVisuals* dest)
		{
			auto data = dest;

			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (data->markArray)
				{
					buf->align(3);
					auto destvisuals = buf->write(data->markArray, def->visualCount);

					for (unsigned char i = 0; i < def->visualCount; i++)
					{
						destvisuals[i][0] = (data->markArray[i][0])
							? reinterpret_cast<Material*>(zone->get_asset_pointer(
								material, data->markArray[i][0]->name))
							: nullptr;
						destvisuals[i][1] = (data->markArray[i][1])
							? reinterpret_cast<Material*>(zone->get_asset_pointer(
								material, data->markArray[i][1]->name))
							: nullptr;
					}

					buf->clear_pointer(&dest->markArray);
				}
			}
			else if (def->visualCount > 1)
			{
				if (data->array)
				{
					buf->align(3);
					auto vis = buf->write(data->array, def->visualCount);

					for (unsigned char i = 0; i < def->visualCount; i++)
					{
						write_fx_elem_visuals(zone, buf, def, &vis[i]);
					}

					buf->clear_pointer(&dest->array);
				}
			}
			else
			{
				write_fx_elem_visuals(zone, buf, def, &dest->instance);
			}
		}

		void IFxEffectDef::write_fx_elem_def(IZone* zone, ZoneBuffer* buf, FxElemDef* dest)
		{
			auto data = dest;

			if (data->velSamples)
			{
				buf->align(3);
				buf->write(data->velSamples, data->velIntervalCount + 1);
				buf->clear_pointer(&dest->velSamples);
			}

			if (data->visSamples)
			{
				buf->align(3);
				buf->write(data->visSamples, data->visStateIntervalCount + 1);
				buf->clear_pointer(&dest->velSamples);
			}

			write_fx_elem_def_visuals(zone, buf, data, &dest->visuals);

			if (data->effectOnImpact)
			{
				buf->write_str_raw(data->effectOnImpact->name);
				ZoneBuffer::clear_pointer(&dest->effectOnImpact);
			}

			if (data->effectOnDeath)
			{
				buf->write_str_raw(data->effectOnDeath->name);
				ZoneBuffer::clear_pointer(&dest->effectOnDeath);
			}

			if (data->effectEmitted)
			{
				buf->write_str_raw(data->effectEmitted->name);
				ZoneBuffer::clear_pointer(&dest->effectEmitted);
			}

			if (data->extended.trailDef)
			{
				if (data->elemType == FX_ELEM_TYPE_TRAIL)
				{
					if (data->extended.trailDef)
					{
						buf->align(3);
						auto dest_traildef = buf->write(data->extended.trailDef);

						if (data->extended.trailDef->verts)
						{
							buf->align(3);
							buf->write(data->extended.trailDef->verts, data->extended.trailDef->vertCount);
							ZoneBuffer::clear_pointer(&dest_traildef->verts);
						}

						if (data->extended.trailDef->inds)
						{
							buf->align(1);
							buf->write(data->extended.trailDef->inds, data->extended.trailDef->indCount);
							ZoneBuffer::clear_pointer(&dest_traildef->inds);
						}

						ZoneBuffer::clear_pointer(&dest->extended.trailDef);
					}
				}
				else if (data->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
				{
					if (data->extended.sparkFountainDef)
					{
						buf->align(3);
						buf->write(data->extended.sparkFountainDef);
						ZoneBuffer::clear_pointer(&dest->extended.sparkFountainDef);
					}
				}
				else if (data->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					if (data->extended.spotLightDef)
					{
						buf->align(3);
						buf->write(data->extended.spotLightDef);
						ZoneBuffer::clear_pointer(&dest->extended.spotLightDef);
					}
				}
				else
				{
					if (data->extended.unknownDef)
					{
						buf->align(1);
						buf->write_stream(data->extended.unknownDef, sizeof(BYTE));
						ZoneBuffer::clear_pointer(&dest->extended.unknownDef);
					}
				}
			}
		}


		void IFxEffectDef::write(IZone* zone, ZoneBuffer* buf)
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(3);

			dest->name = buf->write_str(this->name());

			if (data->elemDefs)
			{
				buf->align(3);
				auto destdef = buf->write(data->elemDefs,
					data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission);

				for (int i = 0; i < (data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission); i++)
				{
					write_fx_elem_def(zone, buf, &destdef[i]);
				}

				buf->clear_pointer(&dest->elemDefs);
			}

			buf->pop_stream();
		}

		void dump_visuals(AssetDumper* dump, FxElemDef* def, FxElemVisuals* vis)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_RUNNER:
				if (vis->effectDef)
				{
					dump->dump_asset(vis->effectDef);
				}
				break;
			case FX_ELEM_TYPE_SOUND:
				if (vis->soundName)
				{
					dump->dump_string(vis->soundName);
				}
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				if (vis->lightDef)
				{
					dump->dump_asset(vis->lightDef);
				}
				break;
			case FX_ELEM_TYPE_MODEL:
				if (vis->xmodel)
				{
					dump->dump_asset(vis->xmodel);
				}
				break;
			default:
				if (def->elemType != FX_ELEM_TYPE_OMNI_LIGHT)
				{
					if (vis->material)
					{
						dump->dump_asset(vis->material);
					}
				}
			}
		}

		void IFxEffectDef::dump(FxEffectDef* asset)
		{
			AssetDumper dump;

			if (!dump.open("fx\\"s + asset->name + ".fxe"))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_array(asset->elemDefs,
				asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot);

			// dump elemDefs
			for (auto i = 0; i < asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot; i
				++)
			{
				auto def = &asset->elemDefs[i];

				// dump elem samples
				dump.dump_array(def->velSamples, def->velIntervalCount + 1);
				dump.dump_array(def->visSamples, def->visStateIntervalCount + 1);

				// dump visuals
				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (def->visuals.markArray)
					{
						dump.dump_array(def->visuals.markArray, def->visualCount);

						for (int i = 0; i < def->visualCount; i++)
						{
							if (def->visuals.markArray[i][0])
							{
								dump.dump_asset(def->visuals.markArray[i][0]);
							}
							if (def->visuals.markArray[i][1])
							{
								dump.dump_asset(def->visuals.markArray[i][1]);
							}
						}
					}
				}
				else if (def->visualCount > 1)
				{
					dump.dump_array(def->visuals.array, def->visualCount);
					for (auto vis = 0; vis < def->visualCount; vis++)
					{
						dump_visuals(&dump, def, &def->visuals.array[vis]);
					}
				}
				else if (def->visualCount)
				{
					dump_visuals(&dump, def, &def->visuals.instance);
				}

				// dump reference FX defs
				dump.dump_asset(def->effectOnImpact);
				dump.dump_asset(def->effectOnDeath);
				dump.dump_asset(def->effectEmitted);

				// dump extended FX data
				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						dump.dump_single(def->extended.trailDef);

						if (def->extended.trailDef->verts)
						{
							dump.dump_array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
						}

						if (def->extended.trailDef->inds)
						{
							dump.dump_array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARKFOUNTAIN)
					{
						dump.dump_single(def->extended.sparkFountainDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump.dump_array(def->extended.unknownDef, 24);
					}
					else
					{
						dump.dump_single(def->extended.unknownDef);
					}
				}
			}

			dump.close();
		}
	}
}