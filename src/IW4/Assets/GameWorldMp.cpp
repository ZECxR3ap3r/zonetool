// ======================= ZoneTool =======================
// zonetool, a fastfile linker for various
// Call of Duty titles. 
//
// Project: https://github.com/ZoneTool/zonetool
// Author: RektInator (https://github.com/RektInator)
// License: GNU GPL v3.0
// ========================================================
#include "stdafx.hpp"
#include "../IW5/Assets/GlassWorld.hpp"

namespace ZoneTool
{
	namespace IW4
	{
		GameWorldMp* IGameWorldMp::parse(const std::string& name, ZoneMemory* mem)
		{
			return reinterpret_cast<GameWorldMp*>(IW5::IGlassWorld::parse(name, mem));
		}

		void IGameWorldMp::init(void* asset, ZoneMemory* mem)
		{
			this->asset_ = reinterpret_cast<GameWorldMp*>(asset);
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
		}

		void IGameWorldMp::init(const std::string& name, ZoneMemory* mem)
		{
			this->name_ = "maps/"s + (currentzone.substr(0, 3) == "mp_" ? "mp/" : "") + currentzone + ".d3dbsp"; // name;
			this->asset_ = this->parse(name, mem);

			if (!this->asset_)
			{
				this->asset_ = DB_FindXAssetHeader(this->type(), name.data()).gameworldmp;
			}
		}

		void IGameWorldMp::prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		void IGameWorldMp::load_depending(IZone* zone)
		{
		}

		std::string IGameWorldMp::name()
		{
			return this->name_;
		}

		std::int32_t IGameWorldMp::type()
		{
			return game_map_mp;
		}

		void IGameWorldMp::write(IZone* zone, ZoneBuffer* buf)
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			assert(sizeof GameWorldMp, 8);
			assert(sizeof G_GlassData, 128);
			assert(sizeof G_GlassPiece, 12);
			assert(sizeof G_GlassName, 12);

			buf->push_stream(3);
			START_LOG_STREAM;

			dest->name = buf->write_str(this->name());

			if (data->g_glassData)
			{
				buf->align(3);

				auto glassdata = data->g_glassData;
				auto destdata = buf->write(glassdata);

				if (glassdata->glassPieces)
				{
					buf->align(3);
					buf->write(glassdata->glassPieces, glassdata->pieceCount);
					ZoneBuffer::clear_pointer(&destdata->glassPieces);
				}
				if (glassdata->glassNames)
				{
					buf->align(3);
					auto namedest = buf->write(glassdata->glassNames, glassdata->glassNameCount);

					for (unsigned int i = 0; i < glassdata->glassNameCount; i++)
					{
						namedest[i].nameStr = buf->write_str(glassdata->glassNames[i].nameStr);

						if (glassdata->glassNames[i].pieceCount)
						{
							buf->align(1);
							buf->write(glassdata->glassNames[i].pieceIndices, glassdata->glassNames[i].pieceCount);
							ZoneBuffer::clear_pointer(&glassdata->glassNames[i].pieceIndices);
						}
					}

					ZoneBuffer::clear_pointer(&destdata->glassNames);
				}

				ZoneBuffer::clear_pointer(&dest->g_glassData);
			}

			END_LOG_STREAM;
			buf->pop_stream();
		}

		void IGameWorldMp::dump(GameWorldMp* asset)
		{
			IW5::IGlassWorld::dump(reinterpret_cast<IW5::GlassWorld*>(asset));
		}
	}
}
