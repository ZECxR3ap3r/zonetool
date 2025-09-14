// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "IW5/Assets/Sound.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		IW5::SoundChannel iw5_conversion_table[IW4::SND_CHANNEL_COUNT]
		{
			IW5::SND_CHANNEL_PHYSICS,
			IW5::SND_CHANNEL_AMBDIST1,
			IW5::SND_CHANNEL_AMBDIST2,
			IW5::SND_CHANNEL_AUTO,
			IW5::SND_CHANNEL_AUTO2,
			IW5::SND_CHANNEL_AUTODOG,
			IW5::SND_CHANNEL_BULLETIMPACT,
			IW5::SND_CHANNEL_BULLETWHIZBY,
			IW5::SND_CHANNEL_EXPLOSIVEIMPACT,
			IW5::SND_CHANNEL_ELEMENT,
			IW5::SND_CHANNEL_AUTO2D,
			IW5::SND_CHANNEL_VEHICLE,
			IW5::SND_CHANNEL_VEHICLELIMITED,
			IW5::SND_CHANNEL_MENU,
			IW5::SND_CHANNEL_BODY,
			IW5::SND_CHANNEL_BODY2D,
			IW5::SND_CHANNEL_RELOAD,
			IW5::SND_CHANNEL_RELOAD2D,
			IW5::SND_CHANNEL_ITEM,
			IW5::SND_CHANNEL_EFFECTS1,
			IW5::SND_CHANNEL_EFFECTS2,
			IW5::SND_CHANNEL_WEAPON,
			IW5::SND_CHANNEL_WEAPON2D,
			IW5::SND_CHANNEL_NONSHOCK,
			IW5::SND_CHANNEL_VOICE,
			IW5::SND_CHANNEL_LOCAL,
			IW5::SND_CHANNEL_LOCAL2,
			IW5::SND_CHANNEL_LOCAL3,
			IW5::SND_CHANNEL_AMBIENT,
			IW5::SND_CHANNEL_HURT,
			IW5::SND_CHANNEL_PLAYER1,
			IW5::SND_CHANNEL_PLAYER2,
			IW5::SND_CHANNEL_MUSIC,
			IW5::SND_CHANNEL_MUSICNOPAUSE,
			IW5::SND_CHANNEL_MISSION,
			IW5::SND_CHANNEL_ANNOUNCER,
			IW5::SND_CHANNEL_SHELLSHOCK,
		};

		IW5::SoundChannel ISound::channel_to_iw5(IW4::SoundChannel snd_channel)
		{
			return iw5_conversion_table[snd_channel];
		}

		IW4::SoundChannel iw4_conversion_table[IW5::SND_CHANNEL_COUNT]
		{
			IW4::SND_CHANNEL_PHYSICS,
			IW4::SND_CHANNEL_AMBDIST1,
			IW4::SND_CHANNEL_AMBDIST2,
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_ALARM
			IW4::SND_CHANNEL_AUTO,
			IW4::SND_CHANNEL_AUTO2,
			IW4::SND_CHANNEL_AUTO2D,
			IW4::SND_CHANNEL_AUTODOG,
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_EXPLOSIONDIST1
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_EXPLOSIONDIST2
			IW4::SND_CHANNEL_EXPLOSIVEIMPACT,
			IW4::SND_CHANNEL_ELEMENT,
			IW4::SND_CHANNEL_ELEMENT, // SND_CHANNEL_ELEMENT_INT
			IW4::SND_CHANNEL_ELEMENT, // SND_CHANNEL_ELEMENT_EXT
			IW4::SND_CHANNEL_BULLETIMPACT,
			IW4::SND_CHANNEL_BULLETIMPACT, // SND_CHANNEL_BULLETFLESH1
			IW4::SND_CHANNEL_BULLETIMPACT, // SND_CHANNEL_BULLETFLESH2
			IW4::SND_CHANNEL_BULLETWHIZBY,
			IW4::SND_CHANNEL_VEHICLE,
			IW4::SND_CHANNEL_VEHICLELIMITED,
			IW4::SND_CHANNEL_MENU,
			IW4::SND_CHANNEL_BODY,
			IW4::SND_CHANNEL_BODY2D,
			IW4::SND_CHANNEL_RELOAD,
			IW4::SND_CHANNEL_RELOAD2D,
			IW4::SND_CHANNEL_ITEM,
			IW4::SND_CHANNEL_EFFECTS1, // SND_CHANNEL_EXPLOSION1
			IW4::SND_CHANNEL_EFFECTS1, // SND_CHANNEL_EXPLOSION2
			IW4::SND_CHANNEL_EFFECTS1, // SND_CHANNEL_EXPLOSION3
			IW4::SND_CHANNEL_EFFECTS1, // SND_CHANNEL_EXPLOSION4
			IW4::SND_CHANNEL_EFFECTS1, // SND_CHANNEL_EXPLOSION5
			IW4::SND_CHANNEL_EFFECTS1,
			IW4::SND_CHANNEL_EFFECTS2,
			IW4::SND_CHANNEL_EFFECTS2, // SND_CHANNEL_EFFECTS3
			IW4::SND_CHANNEL_AUTO2D, // SND_CHANNEL_EFFECTS2D1
			IW4::SND_CHANNEL_AUTO2D, // SND_CHANNEL_EFFECTS2D2
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_NORESTRICT
			IW4::SND_CHANNEL_AUTO2D, // SND_CHANNEL_NORESTRICT2D
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_AIRCRAFT
			IW4::SND_CHANNEL_AUTO2D, // SND_CHANNEL_VEHICLE2D
			IW4::SND_CHANNEL_WEAPON, // SND_CHANNEL_WEAPON_DIST
			IW4::SND_CHANNEL_WEAPON, // SND_CHANNEL_WEAPON_MID
			IW4::SND_CHANNEL_WEAPON,
			IW4::SND_CHANNEL_WEAPON2D,
			IW4::SND_CHANNEL_NONSHOCK,
			IW4::SND_CHANNEL_NONSHOCK, // SND_CHANNEL_NONSHOCK2
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_GRONDO3D
			IW4::SND_CHANNEL_AUTO2D, // SND_CHANNEL_GRONDO2D
			IW4::SND_CHANNEL_VOICE,
			IW4::SND_CHANNEL_LOCAL,
			IW4::SND_CHANNEL_LOCAL2,
			IW4::SND_CHANNEL_LOCAL3,
			IW4::SND_CHANNEL_AMBIENT,
			IW4::SND_CHANNEL_HURT,
			IW4::SND_CHANNEL_PLAYER1,
			IW4::SND_CHANNEL_PLAYER2,
			IW4::SND_CHANNEL_MUSIC,
			IW4::SND_CHANNEL_MUSICNOPAUSE,
			IW4::SND_CHANNEL_MISSION,
			IW4::SND_CHANNEL_AUTO, // SND_CHANNEL_CRITICAL
			IW4::SND_CHANNEL_ANNOUNCER,
			IW4::SND_CHANNEL_SHELLSHOCK,
		};

		IW4::SoundChannel ISound::channel_to_iw4(IW5::SoundChannel snd_channel)
		{
			return iw4_conversion_table[snd_channel];
		}

		snd_alias_list_t* ISound::parse(const std::string& name, ZoneMemory* mem)
		{
			auto* iw5_asset = IW5::ISound::parse(name, mem);

			if (!iw5_asset)
			{
				return nullptr;
			}

			auto* asset = mem->Alloc<snd_alias_list_t>();
			memset(asset, 0, sizeof snd_alias_list_t);

			asset->name = iw5_asset->name;
			asset->count = iw5_asset->count;

			asset->head = mem->Alloc<snd_alias_t>(asset->count);
			memset(asset->head, 0, sizeof(snd_alias_t) * asset->count);

			for (auto i = 0; i < asset->count; i++)
			{
				auto* current_iw4 = &asset->head[i];
				auto* current_iw5 = &iw5_asset->head[i];

				memcpy(current_iw4, current_iw5, 36);
				memcpy(&current_iw4->pitchMin, &current_iw5->pitchMin, 20);

				IW5::SoundAliasFlags iw5_flags;
				iw5_flags.intValue = current_iw5->flags;

				IW4::SoundAliasFlags iw4_flags;
				iw4_flags.intValue = 0;

				iw4_flags.looping = iw5_flags.looping;
				iw4_flags.isMaster = iw5_flags.isMaster;
				iw4_flags.isSlave = iw5_flags.isSlave;
				iw4_flags.fullDryLevel = iw5_flags.fullDryLevel;
				iw4_flags.noWetLevel = iw5_flags.noWetLevel;
				iw4_flags.unknown1 = iw5_flags.unknown1;
				iw4_flags.unknown2 = iw5_flags.unknown2;
				iw4_flags.type = iw5_flags.type;
				iw4_flags.channel = channel_to_iw4(IW5::SoundChannel(iw5_flags.channel));

				current_iw4->flags = iw4_flags.intValue;

				if (current_iw5->masterPercentage == 0.0f || current_iw5->slavePercentage > current_iw5->masterPercentage)
				{
					current_iw4->___u15.slavePercentage = current_iw5->slavePercentage;
				}
				else
				{
					current_iw4->___u15.masterPercentage = current_iw5->masterPercentage;
				}

				memcpy(&current_iw4->probability, &current_iw5->probability, 36);
			}

			return asset;
		}

		void ISound::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data()).sound;
			}
		}

		void ISound::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void ISound::load_depending(IZone* zone)
		{
			auto* data = this->asset_;

			for (auto i = 0u; i < data->count; i++)
			{
				auto* head = &data->head[i];

				if (head->volumeFalloffCurve)
				{
					zone->add_asset_of_type(sndcurve, head->volumeFalloffCurve->filename);
				}

				if (head->soundFile)
				{
					if (head->soundFile->type == SAT_LOADED)
					{
						zone->add_asset_of_type(loaded_sound, head->soundFile->sound.loadSnd->name);
					}
				}
			}
		}

		std::string ISound::name()
		{
			return this->name_;
		}

		std::int32_t ISound::type()
		{
			return sound;
		}

		void ISound::write_soundfile(IZone* zone, ZoneBuffer* buf, SoundFile* data)
		{
			auto* dest = buf->write(data);

			if (data->type == SAT_STREAMED || data->type == SAT_PRIMED)
			{
				if (data->sound.streamSnd.dir)
				{
					dest->sound.streamSnd.dir = buf->write_str(data->sound.streamSnd.dir);
				}

				if (data->sound.streamSnd.name)
				{
					dest->sound.streamSnd.name = buf->write_str(data->sound.streamSnd.name);
				}
			}
			else if (data->type == SAT_LOADED)
			{
				if (data->sound.loadSnd)
				{
					dest->sound.loadSnd = static_cast<LoadedSound*>(zone->get_asset_pointer(
						loaded_sound, data->sound.loadSnd->name));
				}
			}
		}

		void ISound::write_head(IZone* zone, ZoneBuffer* buf, snd_alias_t* dest)
		{
			auto* data = dest;

			if (data->aliasName)
			{
				dest->aliasName = buf->write_str(data->aliasName);
			}

			if (data->subtitle)
			{
				dest->subtitle = buf->write_str(data->subtitle);
			}

			if (data->secondaryAliasName)
			{
				dest->secondaryAliasName = buf->write_str(data->secondaryAliasName);
			}

			if (data->chainAliasName)
			{
				dest->chainAliasName = buf->write_str(data->chainAliasName);
			}

			if (data->mixerGroup)
			{
				dest->mixerGroup = buf->write_str(data->mixerGroup);
			}

			if (data->soundFile)
			{
				buf->align(3);
				write_soundfile(zone, buf, data->soundFile);
				ZoneBuffer::clear_pointer(&dest->soundFile);
			}

			if (data->volumeFalloffCurve)
			{
				dest->volumeFalloffCurve = static_cast<SndCurve*>(zone->get_asset_pointer(
					sndcurve, data->volumeFalloffCurve->filename));
			}

			if (data->speakerMap)
			{
				buf->align(3);
				auto* speaker_map = buf->write(data->speakerMap);

				if (speaker_map->name)
				{
					speaker_map->name = buf->write_str(speaker_map->name);
				}

				ZoneBuffer::clear_pointer(&dest->speakerMap);
			}
		}

		void ISound::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (dest->head)
			{
				buf->align(3);
				auto* dest_sound = buf->write(data->head, data->count);

				for (auto i = 0; i < data->count; i++)
				{
					write_head(zone, buf, &dest_sound[i]);
				}

				ZoneBuffer::clear_pointer(&dest->head);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}
		
		void ISound::dump(snd_alias_list_t* asset)
		{
			auto* iw5_asset = new IW5::snd_alias_list_t;
			memset(iw5_asset, 0, sizeof IW5::snd_alias_list_t);

			iw5_asset->name = asset->name;
			iw5_asset->count = asset->count;
			
			iw5_asset->head = new IW5::snd_alias_t[iw5_asset->count];
			memset(iw5_asset->head, 0, sizeof(IW5::snd_alias_t) * iw5_asset->count);

			for (auto i = 0; i < iw5_asset->count; i++)
			{
				auto* current_iw4 = &asset->head[i];
				auto* current_iw5 = &iw5_asset->head[i];
				
				memcpy(current_iw5, &asset->head[i], 36);
				current_iw5->volModIndex = 0x12;
				memcpy(&current_iw5->pitchMin, &current_iw4->pitchMin, 20);

				IW4::SoundAliasFlags iw4_flags;
				iw4_flags.intValue = current_iw4->flags;

				IW5::SoundAliasFlags iw5_flags;
				iw5_flags.intValue = 0;

				iw5_flags.looping = iw4_flags.looping;
				iw5_flags.isMaster = iw4_flags.isMaster;
				iw5_flags.isSlave = iw4_flags.isSlave;
				iw5_flags.fullDryLevel = iw4_flags.fullDryLevel;
				iw5_flags.noWetLevel = iw4_flags.noWetLevel;
				iw5_flags.unknown1 = iw4_flags.unknown1;
				iw5_flags.unknown2 = iw4_flags.unknown2;
				iw5_flags.type = iw4_flags.type;
				iw5_flags.channel = channel_to_iw5(IW4::SoundChannel(iw4_flags.channel));

				current_iw5->flags = iw5_flags.intValue;

				current_iw5->masterPriority = '2';
				current_iw5->masterPercentage = current_iw4->___u15.masterPercentage;
				current_iw5->slavePercentage = current_iw4->___u15.slavePercentage;
				memcpy(&current_iw5->probability, &current_iw4->probability, 36);
			}
			
			IW5::ISound::dump(iw5_asset);

			delete[] iw5_asset->head;
			delete iw5_asset;
		}
	}
}
