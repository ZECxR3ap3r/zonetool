// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"

namespace ZoneTool
{
#define SOUND_DUMP_STRING(entry) \
	if (asset->entry) sound[#entry] = std::string(asset->entry); \
	else sound[#entry] = nullptr;
#define SOUND_DUMP_CHAR(entry) \
	sound[#entry] = (char)asset->entry;
#define SOUND_DUMP_INT(entry) \
	sound[#entry] = (int)asset->entry;
#define SOUND_DUMP_FLOAT(entry) \
	sound[#entry] = (float)asset->entry;

#define SOUND_STRING(entry) \
	if (!snddata[#entry].is_null()) { \
		asset->entry = mem->StrDup(snddata[#entry].get<std::string>().data()); \
	} else { asset->entry = nullptr; }
#define SOUND_CHAR(entry) \
	asset->entry = snddata[#entry].get<char>()
#define SOUND_INT(entry) \
	asset->entry = snddata[#entry].get<int>()
#define SOUND_FLOAT(entry) \
	asset->entry = snddata[#entry].get<float>()

	namespace IW5
	{
		void ISound::json_parse_snd_alias(snd_alias_t* asset, nlohmann::json snddata, ZoneMemory* mem)
		{
			SOUND_STRING(aliasName);
			SOUND_STRING(subtitle);
			SOUND_STRING(secondaryAliasName);
			SOUND_STRING(chainAliasName);
			SOUND_STRING(mixerGroup);

			if (!snddata["type"].is_null())
			{
				asset->soundFile = mem->Alloc<SoundFile>();
				asset->soundFile->type = snddata["type"].get<int>();
				asset->soundFile->exists = true;

				if (asset->soundFile->type == 1)
				{
					asset->soundFile->sound.loadSnd = mem->Alloc<LoadedSound>();
					// (LoadedSound*)malloc(sizeof(LoadedSound*));
					asset->soundFile->sound.loadSnd->name = mem->StrDup(snddata["soundfile"].get<std::string>().c_str());
				}
				else
				{
					std::filesystem::path fullpath = snddata["soundfile"].get<std::string>();
					std::string dir = fullpath.parent_path().string();
					std::string name = fullpath.filename().string();

					asset->soundFile->sound.streamSnd.dir = mem->StrDup(dir.data());
					asset->soundFile->sound.streamSnd.name = mem->StrDup(name.data());
				}

				SOUND_INT(sequence);
				SOUND_INT(flags);
				SOUND_INT(startDelay);

				// floats
				SOUND_FLOAT(volMin);
				SOUND_FLOAT(volMax);
				SOUND_FLOAT(pitchMin);
				SOUND_FLOAT(pitchMax);
				SOUND_FLOAT(distMin);
				SOUND_FLOAT(distMax);

				SOUND_FLOAT(slavePercentage);
				SOUND_FLOAT(probability);
				SOUND_FLOAT(lfePercentage);
				SOUND_FLOAT(centerPercentage);
				SOUND_FLOAT(envelopMin);
				SOUND_FLOAT(envelopMax);
				SOUND_FLOAT(envelopPercentage);

				SOUND_INT(volModIndex);
				SOUND_INT(masterPriority);
				SOUND_FLOAT(velocityMin);
				SOUND_FLOAT(masterPercentage);
				SOUND_CHAR(masterPriority);

				if (!snddata["volumeFalloffCurve"].is_null())
				{
					asset->volumeFalloffCurve = mem->Alloc<SndCurve>(); // (SndCurve*)calloc(1, sizeof(SndCurve));
					asset->volumeFalloffCurve->filename = mem->StrDup(
						snddata["volumeFalloffCurve"].get<std::string>().c_str());
				}

				if (!snddata["speakerMap"].is_null())
				{
					asset->speakerMap = mem->Alloc<SpeakerMap>(); // (SpeakerMap*)calloc(1, sizeof(SpeakerMap));
					nlohmann::json speakerMap = snddata["speakerMap"];

					asset->speakerMap->name = mem->StrDup(speakerMap["name"].get<std::string>().c_str());
					asset->speakerMap->isDefault = speakerMap["isDefault"].get<bool>();

					if (!speakerMap["channelMaps"].is_null())
					{
						nlohmann::json channelMaps = speakerMap["channelMaps"];
						for (char x = 0; x < 2; x++)
						{
							for (char y = 0; y < 2; y++)
							{
								if (!channelMaps[(x & 0x01) << 1 | y & 0x01].is_null())
								{
									nlohmann::json channelMap = channelMaps[(x & 0x01) << 1 | y & 0x01];
									asset->speakerMap->channelMaps[x][y].entryCount = channelMap["entryCount"].get<int
									>();

									if (!channelMap["speakers"].is_null())
									{
										nlohmann::json speakers = channelMap["speakers"];

										for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].entryCount;
											speaker++)
										{
											if (!speakers[speaker].is_null())
											{
												nlohmann::json jspeaker = speakers[speaker];
												asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker =
													jspeaker["speaker"].get<int>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels =
													jspeaker["numLevels"].get<int>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0] =
													jspeaker["levels0"].get<float>();
												asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1] =
													jspeaker["levels1"].get<float>();
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

		snd_alias_list_t* ISound::json_parse(const std::string& name, ZoneMemory* mem)
		{
			const auto path = "sounds\\"s + name;

			ZONETOOL_INFO("Parsing sound \"%s\"...", name.c_str());

			auto file = FileSystem::FileOpen(path, "rb");
			auto size = FileSystem::FileSize(file);
			auto bytes = FileSystem::ReadBytes(file, size);
			FileSystem::FileClose(file);

			nlohmann::json snddata = nlohmann::json::parse(bytes);

			snd_alias_list_t* asset = mem->Alloc<snd_alias_list_t>();

			SOUND_STRING(aliasName);
			SOUND_INT(count);

			asset->head = mem->Alloc<snd_alias_t>(asset->count);

			nlohmann::json heads = snddata["head"];
			for (int i = 0; i < asset->count; i++)
			{
				json_parse_snd_alias(&asset->head[i], heads[i], mem);
			}

			return asset;
		}

		snd_alias_list_t* ISound::parse(const std::string& name, ZoneMemory* mem)
		{
			if (name.empty())
			{
				return nullptr;
			}

			if (FileSystem::FileExists("sounds\\"s + name))
			{
				return json_parse(name, mem);
			}

			const auto path = "sounds\\"s + name + ".xss";

			if (FileSystem::FileExists(path))
			{
				AssetReader reader(mem);
				if (!reader.open(path))
				{
					return nullptr;
				}

				ZONETOOL_INFO("Parsing sound %s...", name.c_str());

				auto* asset = reader.read_single<snd_alias_list_t>();
				asset->name = reader.read_string();
				asset->head = reader.read_array<snd_alias_t>();

				for (auto i = 0; i < asset->count; i++)
				{
					auto* current = &asset->head[i];

					if (current->aliasName)
					{
						current->aliasName = reader.read_string();
					}

					if (current->subtitle)
					{
						current->subtitle = reader.read_string();
					}

					if (current->secondaryAliasName)
					{
						current->secondaryAliasName = reader.read_string();
					}

					if (current->chainAliasName)
					{
						current->chainAliasName = reader.read_string();
					}

					if (current->mixerGroup)
					{
						current->mixerGroup = reader.read_string();
					}

					if (current->soundFile)
					{
						current->soundFile = reader.read_single<SoundFile>();

						if (current->soundFile->type == SAT_LOADED)
						{
							current->soundFile->sound.loadSnd = reader.read_asset<LoadedSound>();
						}
						else
						{
							current->soundFile->sound.streamSnd.name = reader.read_string();
							current->soundFile->sound.streamSnd.dir = reader.read_string();
						}
					}

					if (current->volumeFalloffCurve)
					{
						current->volumeFalloffCurve = reader.read_asset<SndCurve>();
					}

					if (current->speakerMap)
					{
						current->speakerMap = reader.read_single<SpeakerMap>();
						current->speakerMap->name = reader.read_string();
					}
				}

				reader.close();
				
				return asset;
			}

			return nullptr;
		}

		void ISound::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), this->name().data(), 1).sound;
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
					zone->add_asset_of_type(sndcurve, head->volumeFalloffCurve->name);
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
					sndcurve, data->volumeFalloffCurve->name));
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

				for (std::int32_t i = 0; i < data->count; i++)
				{
					write_head(zone, buf, &dest_sound[i]);
				}

				ZoneBuffer::clear_pointer(&dest->head);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void ISound::json_dump_snd_alias(nlohmann::json& sound, snd_alias_t* asset)
		{
			// strings
			SOUND_DUMP_STRING(aliasName);
			SOUND_DUMP_STRING(subtitle);
			SOUND_DUMP_STRING(secondaryAliasName);
			SOUND_DUMP_STRING(chainAliasName);
			SOUND_DUMP_STRING(mixerGroup);

			// soundfile shit
			if (asset->soundFile)
			{
				sound["type"] = (int)asset->soundFile->type;

				if (asset->soundFile->type == 1)
					sound["soundfile"] = asset->soundFile->sound.loadSnd->name;
				else
					sound["soundfile"] = std::string(asset->soundFile->sound.streamSnd.dir) + "/" + std::string(
						asset->soundFile->sound.streamSnd.name);
			}

			// ints
			SOUND_DUMP_INT(sequence);
			SOUND_DUMP_INT(volModIndex);
			SOUND_DUMP_INT(flags);
			SOUND_DUMP_INT(startDelay);

			// floats
			SOUND_DUMP_FLOAT(volMin);
			SOUND_DUMP_FLOAT(volMax);
			SOUND_DUMP_FLOAT(pitchMin);
			SOUND_DUMP_FLOAT(pitchMax);
			SOUND_DUMP_FLOAT(distMin);
			SOUND_DUMP_FLOAT(distMax);
			SOUND_DUMP_FLOAT(velocityMin);

			SOUND_DUMP_CHAR(masterPriority);
			SOUND_DUMP_FLOAT(masterPercentage);

			SOUND_DUMP_FLOAT(slavePercentage);
			SOUND_DUMP_FLOAT(probability);
			SOUND_DUMP_FLOAT(lfePercentage);
			SOUND_DUMP_FLOAT(centerPercentage);
			SOUND_DUMP_FLOAT(envelopMin);
			SOUND_DUMP_FLOAT(envelopMax);
			SOUND_DUMP_FLOAT(envelopPercentage);

			if (asset->volumeFalloffCurve && asset->volumeFalloffCurve->filename)
				sound["volumeFalloffCurve"] = asset->volumeFalloffCurve->filename;

			if (asset->speakerMap)
			{
				nlohmann::json speakerMap;
				speakerMap["name"] = asset->speakerMap->name;
				speakerMap["isDefault"] = asset->speakerMap->isDefault;

				nlohmann::json channelMaps;
				for (char x = 0; x < 2; x++)
				{
					for (char y = 0; y < 2; y++)
					{
						nlohmann::json channelMap;
						channelMap["entryCount"] = asset->speakerMap->channelMaps[x][y].entryCount;

						nlohmann::json speakers;

						for (int speaker = 0; speaker < asset->speakerMap->channelMaps[x][y].entryCount; speaker++)
						{
							nlohmann::json jspeaker;

							jspeaker["speaker"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].speaker;
							jspeaker["numLevels"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].numLevels;
							jspeaker["levels0"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[0];
							jspeaker["levels1"] = asset->speakerMap->channelMaps[x][y].speakers[speaker].levels[1];

							speakers[speaker] = jspeaker;
						}

						channelMap["speakers"] = speakers;

						channelMaps[(x & 0x01) << 1 | y & 0x01] = channelMap;
					}
				}

				speakerMap["channelMaps"] = channelMaps;

				sound["speakerMap"] = speakerMap;
			}
		}

		void ISound::json_dump(snd_alias_list_t* asset)
		{
			const auto path = "sounds\\"s + asset->name;

			nlohmann::ordered_json sound;
			nlohmann::ordered_json aliases;

			SOUND_DUMP_STRING(aliasName);
			SOUND_DUMP_INT(count);

			for (int i = 0; i < asset->count; i++)
			{
				nlohmann::json alias;
				json_dump_snd_alias(alias, &asset->head[i]);
				aliases[i] = alias;
			}

			sound["head"] = aliases;

			std::string assetstr = sound.dump(4);
			FILE* fp = FileSystem::FileOpen(path, "wb");

			if (fp)
			{
				fwrite(assetstr.data(), assetstr.size(), 1, fp);
				fclose(fp);
			}
		}

		void ISound::dump(snd_alias_list_t* asset)
		{
			// add json shit too
			if (true)
			{
				json_dump(asset);
				return;
			}

			const auto path = "sounds\\"s + asset->name + ".xss";

			AssetDumper dump;
			if (!dump.open(path))
			{
				return;
			}
			
			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_array(asset->head, asset->count);
			
			for (auto i = 0; i < asset->count; i++)
			{
				auto* current = &asset->head[i];

				if (current->aliasName)
				{
					dump.dump_string(current->aliasName);
				}

				if (current->subtitle)
				{
					dump.dump_string(current->subtitle);
				}

				if (current->secondaryAliasName)
				{
					dump.dump_string(current->secondaryAliasName);
				}

				if (current->chainAliasName)
				{
					dump.dump_string(current->chainAliasName);
				}

				if (current->mixerGroup)
				{
					dump.dump_string(current->mixerGroup);
				}

				if (current->soundFile)
				{
					dump.dump_single(current->soundFile);

					if (current->soundFile->type == SAT_LOADED)
					{
						dump.dump_asset(current->soundFile->sound.loadSnd);
					}
					else
					{
						dump.dump_string(current->soundFile->sound.streamSnd.name);
						dump.dump_string(current->soundFile->sound.streamSnd.dir);
					}
				}

				if (current->volumeFalloffCurve)
				{
					dump.dump_asset(current->volumeFalloffCurve);
				}

				if (current->speakerMap)
				{
					dump.dump_single(current->speakerMap);
					dump.dump_string(current->speakerMap->name);
				}
			}
			
			dump.close();
		}
	}
}
